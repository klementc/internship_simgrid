#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Engine.hpp>
#include <simgrid/s4u/Exec.hpp>
#include <simgrid/s4u/Comm.hpp>
#include <simgrid/s4u/Semaphore.hpp>
#include <simgrid/kernel/future.hpp>
#include <simgrid/plugins/load.h>
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
  n_bl_ = s4u::Semaphore::create(maxReqInInst_);
  for(int i=0;i<maxReqInInst_;i++){n_bl_->acquire();}
}

TaskInstance::TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(TaskDescription*)> outputFunction, int par)
  :TaskInstance(etm,mbName,outputFunction,par, 0)
  {}

void TaskInstance::pollTasks()
{
  simgrid::s4u::Mailbox* mbp = simgrid::s4u::Mailbox::by_name(mbName_);

  // create and init parallel reception of tasks
  int parComSize =  1; // 1 to avoid the segfault
  void* taskV;

  for(int i=0;i<parComSize;i++){
    commV.push_back(mbp->get_async(&taskV));
  }

  while(keepGoing_) {
    int newMsgPos = simgrid::s4u::Comm::wait_any(&commV);
    TaskDescription* taskRequest = static_cast<TaskDescription*>(taskV);
    taskRequest->instArrival = simgrid::s4u::Engine::get_clock();
    commV.erase(commV.begin() + newMsgPos);
    commV.push_back(mbp->get_async(&taskV));
    // we wait until there is a slot to execute a task before putting the request in the slot
    //n_empty_->acquire();
    int n = n_empty_->acquire_timeout(1000);
    if(!(n==0))
      continue;
    reqs.push_back(taskRequest);
    XBT_DEBUG("instance received a request, queue size: %d", reqs.size());

    n_full_->release();
  }

}

void TaskInstance::pollEndOfTasks()
{
  //this_actor::sleep_for(10);
  while(keepGoing_){
    n_bl_->acquire();

    int index = simgrid::s4u::Exec::wait_any(&pending_execs);
    // finished one exec, call output function and allow for a new execution
    TaskDescription* a = execMap_.find(pending_execs.at(index))->second;
    a->endExec = simgrid::s4u::Engine::get_clock();
    etm_->modifExecutingReqAmount(-1);
    etm_->setCounterExecSlot(etm_->getCounterExecSlot()+1);


#ifdef USE_JAEGERTRACING
        auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        // TODO Check if no problem on spans
        if(a->parentSpans.size()>0)
          a->parentSpans.at(a->parentSpans.size()-1)->get()->Log({{"endExec", Engine::get_instance()->get_clock()}});
#endif
    simgrid::s4u::ExecPtr ep = pending_execs.at(index);

    pending_execs.erase(pending_execs.begin()+index);

    /*
    * set size for future transmissions
    * used to model the size of the processed data (either bigger or smaller)
    */
    a->dSize *= etm_->getDataSizeRatio();
    XBT_DEBUG("output for %p", a);
    simgrid::s4u::ActorPtr out = simgrid::s4u::Actor::create(mbName_+"outputf"+boost::uuids::to_string(uuidGen_()),s4u::Host::current(),[&]{outputFunction_(a);});

    if(keepGoing_)n_empty_->release();
  }
}

void TaskInstance::run()
{
  host_ = this_actor::get_host();

  poll = simgrid::s4u::Actor::create(mbName_+boost::uuids::to_string(uuidGen_()),s4u::Host::current(),[&]{pollTasks();});
  pollEnd = simgrid::s4u::Actor::create(mbName_+"pollEnd",s4u::Host::current(),[&]{pollEndOfTasks();});
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
      TaskDescription* a = reqs.at(0);
      a->startExec = simgrid::s4u::Engine::get_clock();
      reqs.erase(reqs.begin());
      XBT_DEBUG("instance received req %p", a);

#ifdef USE_JAEGERTRACING
        auto t1 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        auto span = a->parentSpans.size()==0?
          etm_->getTracer().get()->StartSpan( etm_->getServiceName(),
            {opentracing::v3::StartTimestamp(t1)}) :
          etm_->getTracer().get()->StartSpan( etm_->getServiceName(),
            {opentracing::v3::StartTimestamp(t1), opentracing::v3::ChildOf(&a->parentSpans.at(a->parentSpans.size()-1)->get()->context())});
        span->Log({ {"nbInst:", etm_->getInstanceAmount()},{"waitingReqAtStart:", etm_->getAmountOfWaitingRequests()},{"alreadyExecutingAtStart:", etm_->getAmountOfExecutingRequests()},{"startExec", Engine::get_instance()->get_clock()}});
        std::unique_ptr<opentracing::v3::Span>* t = new std::unique_ptr<opentracing::v3::Span>();
        *t = std::move(span);
        a->parentSpans.push_back(t);
#endif

        etm_->modifWaitingReqAmount(-1);
        etm_->modifExecutingReqAmount(1);
        //XBT_INFO("NEXT %lf %s %d", a->flops, this_actor::get_cname(), keepGoing_);
        simgrid::s4u::ExecPtr execP = this_actor::exec_async(etm_->getProcessRatio(a));
        pending_execs.push_back(execP);
        execMap_.insert(std::pair<simgrid::s4u::ExecPtr, TaskDescription*>(execP, a));
        n_bl_->release();


    }catch(Exception e){XBT_INFO("woops: %s", e.what());}
  }

}

void TaskInstance::kill()
{
  XBT_INFO("Kill taskinstance td size: %d %d", reqs.size(), execMap_.size());

  keepGoing_ = false;
  // don't receive requests anymore
  //simgrid::s4u::Comm::wait_all(&commV);
  for(auto c : commV){
    c->cancel();
  }

  // I GUESS IT IS NOT THE WAY TO DO
  // IF WE KILL THE INSTANCE BUT SOME EXEC_ASYNC ARE STILL EXECUTING
  // WAIT FOR THE LAST ONE TO FINISH
  // IT SEEMS TO AVOID A SEGFAULT IN THE HOSTLOAD PLUGIN, BUT ITS A HORRIBLE WAY TO DO IT I GUESS
  if(pending_execs.size()>0)
    pending_execs.at(pending_execs.size()-1)->wait();

  poll->kill();
  pollEnd->kill();
  n_empty_->release();
  n_full_->release();
  n_bl_->release();
}
