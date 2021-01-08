#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <jaegertracing/Tracer.h>
#include <simgrid/s4u/Host.hpp>
#include "simgrid/s4u/Engine.hpp"
#include "simgrid/s4u/Comm.hpp"
#include "simgrid/s4u/Semaphore.hpp"
#include "simgrid/kernel/future.hpp"
#include "simgrid/plugins/load.h"
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/Exception.hpp>

#include "ElasticTask.hpp"

using namespace simgrid;
using namespace s4u;

XBT_LOG_NEW_DEFAULT_CATEGORY(elasticInstance, "elastic task instance");

TaskInstance::TaskInstance(ElasticTaskManager* etm,  std::string mbName,
  std::function<void(TaskDescription*)> outputFunction, int maxReqInInst, double bootTime)
  : etm_(etm), mbName_(mbName), outputFunction_(outputFunction), keepGoing_(true), maxReqInInst_(maxReqInInst), bootTime_(bootTime)
{
  n_empty_ = s4u::Semaphore::create(maxReqInInst);
  n_full_ = s4u::Semaphore::create(0);
}

TaskInstance::TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(TaskDescription*)> outputFunction, int par)
  :TaskInstance(etm,mbName,outputFunction,par, 0)
  {}



void TaskInstance::pollTasks()
{
  simgrid::s4u::Mailbox* mbp = simgrid::s4u::Mailbox::by_name(mbName_);
  while(keepGoing_) {
    int n = n_empty_->acquire_timeout(10);
    if(!(n==0))
      continue;
    try{
      TaskDescription* taskRequest = static_cast<TaskDescription*>(mbp->get(5));
      //TaskDescription* tr = new TaskDescription(*taskRequest);
      taskRequest->dSize = taskRequest->dSize*etm_->getDataSizeRatio();
      //XBT_INFO("SIZE: %d", taskRequest->parentSpans.size());
      //delete taskRequest;
      reqs.push_back(taskRequest);
      XBT_DEBUG("instance received a request, queue size: %d", reqs.size());
      n_full_->release();
    }catch(TimeoutException e){}
  }
}

void TaskInstance::run()
{
  host_ = this_actor::get_host();
  simgrid::s4u::ActorPtr poll = simgrid::s4u::Actor::create(mbName_+boost::uuids::to_string(uuidGen_()),s4u::Host::current(),[&]{pollTasks();});

  // boot duration (just sleep so that we don't process any request in the node until bootime elapsed)
  this_actor::sleep_for(bootTime_);

  while(keepGoing_){
    try{
      int n = n_full_->acquire_timeout(10);
      if(!(n==0))
        continue;

      if(reqs.size()==0){
        continue;
      }
      // receive data from mailbox
      TaskDescription* a = reqs.at(0);// = static_cast<std::map<std::string, double>*>(recMb_->get(10));
      reqs.erase(reqs.begin());
      XBT_DEBUG("instance received req %f %f", a->flops, a->dSize);

      Actor::create("exec"+boost::uuids::to_string(uuidGen_()), this_actor::get_host(), [this, a] {

        auto t1 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        //XBT_INFO("SSSSSSSSSSSS start %d %d", t1, a->parentSpans.size());
        auto span = a->parentSpans.size()==0?
          etm_->getTracer().get()->StartSpan( etm_->getServiceName(),
            {opentracing::v3::StartTimestamp(t1)}) :
          etm_->getTracer().get()->StartSpan( etm_->getServiceName(),
            {opentracing::v3::StartTimestamp(t1), opentracing::v3::ChildOf(&a->parentSpans.at(a->parentSpans.size()-1)->get()->context())});
        span->Log({ {"nbInst:", etm_->getInstanceAmount()},{"waitingReqAtStart:", etm_->getAmountOfWaitingRequests()},{"alreadyExecutingAtStart:", etm_->getAmountOfExecutingRequests()},{"startExec", Engine::get_instance()->get_clock()}});
        std::unique_ptr<opentracing::v3::Span>* t = new std::unique_ptr<opentracing::v3::Span>();
        *t = std::move(span);
        a->parentSpans.push_back(t);

        etm_->modifWaitingReqAmount(-1);
        etm_->modifExecutingReqAmount(1);
        //XBT_INFO("load %f %f", this_actor::get_host()->get_load(),sg_host_get_current_load(this_actor::get_host()));
        this_actor::execute(a->flops);
        etm_->modifExecutingReqAmount(-1);
        etm_->setCounterExecSlot(etm_->getCounterExecSlot()+1);
        n_empty_->release();

        //XBT_INFO("s %d %d", t1, a->parentSpans.size());
        auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        t->get()->Log({{"endExec", Engine::get_instance()->get_clock()}});
        outputFunction_(a);
        //XBT_INFO("%d %d", t1, t2)
        //span->Finish({opentracing::v3::FinishTimestamp( t2)});
      });
    }catch(Exception e){}
  }
  poll->kill();
}

void TaskInstance::kill()
{
  keepGoing_ = false;
}