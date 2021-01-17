#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>
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

  // create and init parallel reception of tasks
  int parComSize = 20;
  void* taskV;

  for(int i=0;i<parComSize;i++){
    commV.push_back(mbp->get_async(&taskV));
  }

  while(keepGoing_) {
    int newMsgPos = simgrid::s4u::Comm::wait_any(&commV);
    //XBT_INFO("inst rec %d %d", newMsgPos, commV.size());
    TaskDescription* taskRequest = static_cast<TaskDescription*>(taskV);

    //  XBT_INFO("finished,delete");
    commV.erase(commV.begin() + newMsgPos);
    commV.push_back(mbp->get_async(&taskV));
    // we wait until there is a slot to execute a task before putting the request in the slot
    //n_empty_->acquire();
    int n = n_empty_->acquire_timeout(1000);
    if(!(n==0))
      continue;
    //TODO modify size of output ? here?? not sure MAYBE MORE AT THE END OF EXEC()
    reqs.push_back(taskRequest);
    XBT_DEBUG("instance received a request, queue size: %d", reqs.size());

    n_full_->release();

/*


    int n = n_empty_->acquire_timeout(1000);
    if(!(n==0))
      continue;
    try{
      TaskDescription* taskRequest = static_cast<TaskDescription*>(mbp->get());
      //TaskDescription* tr = new TaskDescription(*taskRequest);
      taskRequest->dSize = taskRequest->dSize*etm_->getDataSizeRatio();
      reqs.push_back(taskRequest);
      XBT_DEBUG("instance received a request, queue size: %d", reqs.size());
      n_full_->release();
    }catch(TimeoutException e){}
*/
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
      int n = n_full_->acquire_timeout(1000);
      if(!(n==0))
        continue;

      if(reqs.size()==0){
        continue;
      }
      // receive data from mailbox
      TaskDescription* a = reqs.at(0);// = static_cast<std::map<std::string, double>*>(recMb_->get(10));
      reqs.erase(reqs.begin());
//      XBT_DEBUG("instance received req %f %f", a->flops, a->dSize);
      Actor::create("exec"+boost::uuids::to_string(uuidGen_()), this_actor::get_host(), [this, a] {
#ifdef USE_JAEGERTRACING
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
#endif /*USE_JAEGERTRACING*/

        etm_->modifWaitingReqAmount(-1);
        etm_->modifExecutingReqAmount(1);
        this_actor::execute(a->flops);
        etm_->modifExecutingReqAmount(-1);
        etm_->setCounterExecSlot(etm_->getCounterExecSlot()+1);
        n_empty_->release();
#ifdef USE_JAEGERTRACING
        auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        t->get()->Log({{"endExec", Engine::get_instance()->get_clock()}});
        //XBT_INFO("%d %d", t1, t2)
        //t->Finish({opentracing::v3::FinishTimestamp( t2)});
#endif
        XBT_DEBUG("FINISHED EXECUTION %lf", a->flops);
        outputFunction_(a);
      });
    }catch(Exception e){}
  }
  poll->kill();
}

void TaskInstance::kill()
{
  keepGoing_ = false;
  // don't receive requests anymore
  //simgrid::s4u::Comm::wait_all(&commV);

}