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


XBT_LOG_NEW_DEFAULT_CATEGORY(elastic, "elastic tasks");

#ifdef USE_JAEGERTRACING
void setUpTracerGlob(const char* configFilePath, std::string name)
{
  XBT_INFO("set up jaeger tracer for %s", name.c_str());
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
        name, config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
        std::static_pointer_cast<opentracing::Tracer>(tracer));
}

std::shared_ptr<opentracing::v3::Tracer> setUpTracer(const char* configFilePath, std::string name)
{
  XBT_INFO("set up jaeger tracer for %s", name.c_str());
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
        name, config, jaegertracing::logging::consoleLogger());
    return tracer;
}
#endif

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------
ElasticTaskManager::ElasticTaskManager(std::string name, std::vector<std::string> incMailboxes)
  : serviceName_(name), incMailboxes_(incMailboxes), nextHost_(0),
    keepGoing(true), processRatio_(1e7), waitingReqAmount_(0),
    bootDuration_(0), executingReqAmount_(0), dataSizeRatio_(1),
    counterExecSlot_(0), parallelTasksPerInst_(100)
{
  XBT_DEBUG("Creating TaskManager %s", serviceName_.c_str());
  sg_host_load_plugin_init();
  sleep_sem = s4u::Semaphore::create(0);
  modif_sem_ = s4u::Semaphore::create(1);
  access_tmpdata_ = s4u::Semaphore::create(1);

#ifdef USE_JAEGERTRACING
  tracer_ = setUpTracer("config.yml", serviceName_.c_str());
#endif
}
ElasticTaskManager::ElasticTaskManager(std::string name)
  : ElasticTaskManager(name, std::vector<std::string>(1, name))
{}

void ElasticTaskManager::setProcessRatio(int64_t pr)
{
  XBT_DEBUG("new process ration: %d -> %d", processRatio_, pr);
  processRatio_ = pr;
}

void ElasticTaskManager::setParallelTasksPerInst(int s) {
  xbt_assert(s>0, "Instances cannot execute a negative amount of tasks in parallel");
  XBT_DEBUG("new max amount of parallel tasks per instance: %d -> %d", parallelTasksPerInst_, s);
  parallelTasksPerInst_ = s;
}

void ElasticTaskManager::addHost(Host *host) {
  availableHostsList_.push_back(host);
  XBT_DEBUG("created instance: %d", availableHostsList_.size());
  TaskInstance* ti = new TaskInstance(this,serviceName_+"_data", outputFunction, parallelTasksPerInst_);
  tiList.push_back(ti);
  Actor::create("TI"+boost::uuids::to_string(boost::uuids::random_generator()()), host, [&]{ti->run();});
}


void ElasticTaskManager::setBootDuration(double bd){
  xbt_assert(bd>=0, "Boot time has to be non negative");
  bootDuration_ = bd;
}

void ElasticTaskManager::trigger(TaskDescription* td) {
  xbt_assert(td!=nullptr, "Triggered a nullptr");
  td->date = Engine::get_instance()->get_clock();
  nextEvtQueue.push(td);
  // one more pending request
  modifWaitingReqAmount(1);
  sleep_sem->release();
}

void ElasticTaskManager::trigger(TaskDescription* td, double ratioLoad) {  // TODO, network load to add
  xbt_assert(td!=nullptr, "Triggered a nullptr");
  td->date = Engine::get_instance()->get_clock();
  nextEvtQueue.push(td);
  // one more pending request
  modifWaitingReqAmount(1);
  sleep_sem->release();
}

Host* ElasticTaskManager::removeHost(int i){
  xbt_assert(availableHostsList_.size()>1, "Cannot have 0 instances");
  Host* h;
  if(i<availableHostsList_.size()){
    h =  availableHostsList_.at(i);
    availableHostsList_.erase(availableHostsList_.begin()+i);
    simgrid::s4u::TaskInstance* ti = tiList.at(i);
    ti->kill();
    delete ti;
    tiList.erase(tiList.begin()+i);
  }else{
    XBT_INFO("Cannot remove element at position %d, overflow", i);
  }
  return h;
}

unsigned int ElasticTaskManager::getInstanceAmount(){
  return availableHostsList_.size();
}
std::vector<double> ElasticTaskManager::getCPULoads(){
  std::vector<double> v;
  for (int i=0; i<availableHostsList_.size();i++){
    v.push_back(sg_host_get_avg_load(availableHostsList_.at(i)));
    sg_host_load_reset(availableHostsList_.at(i));
  }
  return v;
}

void ElasticTaskManager::modifExecutingReqAmount(int n){
  modif_sem_->acquire();
  executingReqAmount_+=n;
  xbt_assert(executingReqAmount_>=0,"cannot have less than 0 executing requests");
  modif_sem_->release();
}

void ElasticTaskManager::modifWaitingReqAmount(int n){
  modif_sem_->acquire();
  waitingReqAmount_+=n;
  xbt_assert(waitingReqAmount_>=0,"cannot have less than 0 waiting requests");
  modif_sem_->release();
}

int64_t ElasticTaskManager::getAmountOfWaitingRequests(){
  return waitingReqAmount_;
}

int64_t ElasticTaskManager::getAmountOfExecutingRequests(){
  return executingReqAmount_;
}

void ElasticTaskManager::setOutputFunction(std::function<void(TaskDescription*)> f)
{
  outputFunction = f;
}

#ifdef USE_JAEGERTRACING
std::shared_ptr<opentracing::v3::Tracer> ElasticTaskManager::getTracer()
{
  return tracer_;
}
#endif

double ElasticTaskManager::reqPerSec() {
  double tot =0;
  for (auto i : tiList) {
    simgrid::s4u::Host* a = i->getRunningHost();
    if(a)
      tot+=(a->get_pstate_speed(a->get_pstate())*a->get_core_count())/processRatio_;
  }
  return tot;
}

void ElasticTaskManager::setDataSizeRatio(double r) {
  dataSizeRatio_ = r;
}

void ElasticTaskManager::setProcessRatioFunc(std::function<double(std::string)> costReqType){
  costReqType_ = costReqType;
}

double ElasticTaskManager::getProcessRatio(std::string reqType){
  // if the function is defined, use it, otherwise use the single value
  if(costReqType_){
    return costReqType_(reqType);
  }else{
    return processRatio_;
  }
}

double ElasticTaskManager::getDataSizeRatio() {
  return dataSizeRatio_;
}

/**
 * Will stop run()'s infinite loop
 */
void ElasticTaskManager::kill() {
  XBT_INFO("KILL ETM");
  keepGoing = false;
  for(int i=0;i<tiList.size();i++){
    tiList.at(i)->kill();
  }

  for(auto a : pollers_){
    a->kill();
  }
}

void ElasticTaskManager::pollnet(std::string mboxName){
  Mailbox* recvMB = simgrid::s4u::Mailbox::by_name(mboxName.c_str());

  int parComSize = 1;
  std::vector<simgrid::s4u::CommPtr> commV;
  void* taskV;
  for(int i=0;i<parComSize;i++){
    commV.push_back(recvMB->get_async(&taskV));
  }


  while(keepGoing){
          access_tmpdata_->acquire();

    int newMsgPos = simgrid::s4u::Comm::wait_any(&commV);
    TaskDescription* taskRequest = static_cast<TaskDescription*>(taskV);
    //set amount of computation for the instance
    taskRequest->flops = processRatio_;
    taskRequest->queueArrival = simgrid::s4u::Engine::get_clock();
    commV.erase(commV.begin() + newMsgPos);
    commV.push_back(recvMB->get_async(&taskV));
    taskRequest->flopsPerServ.push_back(this->processRatio_);

    if(incMailboxes_.size() == 1) {

#ifdef USE_JAEGERTRACING
        /* NOT SURE WHERE I SHOULD PUT IT
        auto t1 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(Engine::get_instance()->get_clock()*1000));
        auto span = taskRequest->parentSpans.size()==0?
          getTracer().get()->StartSpan( getServiceName(),
            {opentracing::v3::StartTimestamp(t1)}) :
          getTracer().get()->StartSpan( getServiceName(),
            {opentracing::v3::StartTimestamp(t1), opentracing::v3::ChildOf(&taskRequest->parentSpans.at(taskRequest->parentSpans.size()-1)->get()->context())});
        span->Log({ {"nbInst:", getInstanceAmount()},{"waitingReqAtRx:", getAmountOfWaitingRequests()},{"alreadyExecutingAtRx:", getAmountOfExecutingRequests()}});
        std::unique_ptr<opentracing::v3::Span>* t = new std::unique_ptr<opentracing::v3::Span>();
        *t = std::move(span);
        taskRequest->parentSpans.push_back(t);*/
#endif
      trigger(taskRequest);
      sleep_sem->release();
    } else {
      std::vector<TaskDescription*> v =
        (tempData.find(taskRequest->id_)!=tempData.end()) ?
          tempData.find(taskRequest->id_)->second : std::vector<TaskDescription*>();

      v.push_back(taskRequest);
      tempData.insert(std::pair<boost::uuids::uuid,std::vector<TaskDescription*>>(taskRequest->id_, v));

      if(v.size() == incMailboxes_.size()) {
        XBT_DEBUG("Received %d for %p, trigger now", v.size(), taskRequest);
        trigger(taskRequest);
        // remove data
        tempData.erase(taskRequest->id_);
        sleep_sem->release();
      }
    }

    access_tmpdata_->release();

  }
}

/**
 * Supervisor of the elastictasks
 * Fetches events at their execution time and creates a new microtask
 * on one of the provided hosts
 */
void ElasticTaskManager::run() {
  unsigned long long task_count = 0;

  for(auto s : incMailboxes_) {
    Mailbox* rec = simgrid::s4u::Mailbox::by_name(s.c_str());
    pollers_.push_back(simgrid::s4u::Actor::create(serviceName_+"_"+s+"polling",s4u::Host::current(),[&]{pollnet(s);}));
    XBT_INFO("polling on mailbox %s", s.c_str());
  }

  while(1) {
    for(std::vector<simgrid::s4u::CommPtr>::iterator it = pending_comms.begin(); it != pending_comms.end();) {
      if((*it)->test())
        it = pending_comms.erase(it);
      else
        it++;
    }
    // execute events that need be executed now
    while(!nextEvtQueue.empty() && nextEvtQueue.top()->date <= Engine::get_instance()->get_clock()) {
      EvntQ *currentEvent = nextEvtQueue.top();
      nextEvtQueue.pop();
      XBT_DEBUG("add: %p", currentEvent);
      TaskDescription* t = dynamic_cast<TaskDescription*>(currentEvent);
      if (availableHostsList_.size() <= nextHost_)
        nextHost_ = 0;
      simgrid::s4u::Mailbox* mbp = simgrid::s4u::Mailbox::by_name(serviceName_+"_data");

      /* Async version TODO: clean pending vector */
      simgrid::s4u::CommPtr comm = mbp->put_async(t, (t->dSize == -1) ? 1 : t->dSize);
      pending_comms.push_back(comm);

      ++task_count;

      nextHost_++;
      nextHost_ = nextHost_ % availableHostsList_.size();

      }
    if(!keepGoing) {
      break;
    }

    XBT_DEBUG("RUN: going to sleep on semaphore");
    if(!nextEvtQueue.empty()) {
      sleep_sem->acquire_timeout(nextEvtQueue.top()->date - Engine::get_instance()->get_clock());
    } else {
      sleep_sem->acquire_timeout(999.0);
    }
  }
}

ElasticTaskManager::~ElasticTaskManager(){

  // cancel active communications
  for(auto c: pending_comms){
    c->cancel();
  }

  // delete instances
  tiList.clear();

}