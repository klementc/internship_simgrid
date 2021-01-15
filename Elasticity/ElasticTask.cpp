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

#ifdef USE_JAEGERTRACING
  tracer_ = setUpTracer("config.yml", serviceName_.c_str());
#endif
}
ElasticTaskManager::ElasticTaskManager(std::string name)
  : ElasticTaskManager(name, std::vector<std::string>(1, name))
{}

void ElasticTaskManager::setProcessRatio(int64_t pr)
{
  processRatio_ = pr;
}

void ElasticTaskManager::setParallelTasksPerInst(int s) {
  xbt_assert(s>0, "Instances cannot execute a negative amount of tasks in parallel");
  parallelTasksPerInst_ = s;
}

int ElasticTaskManager::getParallelTasksPerInst() {
  return parallelTasksPerInst_;
}

/*test*/
boost::uuids::uuid ElasticTaskManager::addElasticTask(TaskDescription td){
  TaskDescription t(td);
  t.flops = processRatio_;
  tasks.insert({t.id_, t});

  return t.id_;
}
/*test*/

boost::uuids::uuid ElasticTaskManager::addElasticTask(boost::uuids::uuid id, double flopsTask, double interSpawnDelay, double s) {
  TaskDescription td(id, flopsTask, interSpawnDelay, Engine::get_instance()->get_clock(), s);
  tasks.insert({id, td});
  if (interSpawnDelay > 0.0) {
    // one more pending request
    modifWaitingReqAmount(1);
    nextEvtQueue.push(&(tasks.find(id)->second));
    XBT_DEBUG("tasks: %d nextEvtQueue: %d", tasks.size(),nextEvtQueue.size());
    sleep_sem->release();
  }
  return id;
}

boost::uuids::uuid ElasticTaskManager::addElasticTask(boost::uuids::uuid id, double flopsTask, double interSpawnDelay) {
  return addElasticTask(id, flopsTask, interSpawnDelay, -1);
}

void ElasticTaskManager::addHost(Host *host) {
  availableHostsList_.push_back(host);
  XBT_DEBUG("created instance: %d", availableHostsList_.size());
  TaskInstance* ti = new TaskInstance(this,serviceName_+"_data", outputFunction, parallelTasksPerInst_);
  tiList.push_back(ti);
  Actor::create("TI"+boost::uuids::to_string(boost::uuids::random_generator()()), host, [&]{ti->run();});
}

void ElasticTaskManager::changeRatio(boost::uuids::uuid id, double visitsPerSec) {
  removeTaskExecs(id);
  tasks.find(id)->second.interSpawnDelay = visitsPerSec;
  tasks.find(id)->second.date = Engine::get_instance()->get_clock();
  if(visitsPerSec > 0.0) {
    TaskDescription *newTask = new TaskDescription(tasks.find(id)->second);
    nextEvtQueue.push(newTask);
    sleep_sem->release();
  }
}

void ElasticTaskManager::setBootDuration(double bd){
  xbt_assert(bd>=0, "Boot time has to be non negative");
  bootDuration_ = bd;
}

/**
 * Modify the arrival rate of one of the tasks from date
 */
void ElasticTaskManager::addRatioChange(boost::uuids::uuid id, double date, double visitsPerSec) {
  RatioChange *rC = new RatioChange(id, date, visitsPerSec);
  nextEvtQueue.push(rC);
  if (date < nextEvtQueue.top()->date) {
    sleep_sem->release();
  }
}

void ElasticTaskManager::removeTaskExecs(boost::uuids::uuid id) {  // remove all even non repeat
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> newNextEvtQueue;
  while(!nextEvtQueue.empty()) {
    if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
      newNextEvtQueue.push(nextEvtQueue.top());
    } else if (const TaskDescription* t = dynamic_cast<const TaskDescription*>(nextEvtQueue.top())) {
      if (t->id_ != id) {
        newNextEvtQueue.push(nextEvtQueue.top());
      } else {
        delete nextEvtQueue.top();
      }
    }
    nextEvtQueue.pop();
  }
  nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::removeTask(boost::uuids::uuid id){
  tasks.erase(id);
}

void ElasticTaskManager::removeRatioChanges(boost::uuids::uuid id) {
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> newNextEvtQueue;
  while(!nextEvtQueue.empty()) {
    if(strcmp(typeid(nextEvtQueue.top()).name(), "RatioChange")) {
      newNextEvtQueue.push(nextEvtQueue.top());
    } else if (const RatioChange* t = dynamic_cast<const RatioChange*>(nextEvtQueue.top())) {
      if (t->id != id) {
        newNextEvtQueue.push(nextEvtQueue.top());
      }
    }
    nextEvtQueue.pop();
  }
  nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::triggerOneTimeTask(boost::uuids::uuid id) {
  TaskDescription *newTask = new TaskDescription(tasks.find(id)->second);
  newTask->repeat = false;
  newTask->date = 0.0;
  nextEvtQueue.push(newTask);
  // one more pending request
  modifWaitingReqAmount(1);
  sleep_sem->release();
}

void ElasticTaskManager::triggerOneTimeTask(boost::uuids::uuid id, double ratioLoad) {  // TODO, network load to add
  TaskDescription *newTask = new TaskDescription(tasks.find(id)->second);
  newTask->repeat = false;
  newTask->flops = newTask->flops * ratioLoad;
  newTask->date = 0.0;
  nextEvtQueue.push(newTask);
    // one more pending request
  modifWaitingReqAmount(1);
  sleep_sem->release();
}

/**
 * Import timestamps (one timestamp per line) from a file
 */
void ElasticTaskManager::setTimestampsFile(boost::uuids::uuid id, std::string filename) {
  tasks.find(id)->second.repeat = false;
  tasks.find(id)->second.ts_file->open(filename);
  removeTaskExecs(id);
  std::string timestamp;
  if(tasks.find(id)->second.ts_file->is_open()) {
    if(!tasks.find(id)->second.ts_file->eof()) {
      std::getline(*(tasks.find(id)->second.ts_file), timestamp);
      tasks.find(id)->second.date = std::stod(timestamp);
      TaskDescription *newTask = new TaskDescription(tasks.find(id)->second);
      nextEvtQueue.push(newTask);
    } else {
      tasks.find(id)->second.ts_file->close();
    }
  }
  sleep_sem->release();
}

Host* ElasticTaskManager::removeHost(int i){
  xbt_assert(availableHostsList_.size()>1, "Cannot have 0 instances");
  Host* h;
  if(i<availableHostsList_.size()){
    h =  availableHostsList_.at(i);
    availableHostsList_.erase(availableHostsList_.begin()+i);
    tiList.at(i)->kill();
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
    //v.push_back(sg_host_get_current_load(availableHostsList_.at(i)));
    //XBT_INFO("load: %s %f", availableHostsList_.at(i)->get_name().c_str(),sg_host_get_current_load(availableHostsList_.at(i)));
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

double ElasticTaskManager::getDataSizeRatio() {
  return dataSizeRatio_;
}

/**
 * Will stop run()'s infinite loop
 */
void ElasticTaskManager::kill() {
  keepGoing = false;
  for(int i=0;i<tiList.size();i++){
    tiList.at(i)->kill();
  }
}

void ElasticTaskManager::pollnet(std::string mboxName){
  Mailbox* recvMB = simgrid::s4u::Mailbox::by_name(mboxName.c_str());
  while(keepGoing){
    try{
      TaskDescription* taskRequest = static_cast<TaskDescription*>(recvMB->get());

      //???????????????????????????????????????????????????????????????????????????????
      // TODO which size and compute to put when multiple inputs???????????????????????
      //???????????????????????????????????????????????????????????????????????????????
      boost::uuids::uuid i = addElasticTask(*taskRequest);//addElasticTask(taskRequest->id_, processRatio_, 0, taskRequest->dSize);
      XBT_DEBUG("POLLING RECEIVED size %f %s", taskRequest->dSize, boost::uuids::to_string(taskRequest->id_).c_str());
      if(incMailboxes_.size() == 1) {
        triggerOneTimeTask(i);
        sleep_sem->release();
      } else {
        std::vector<TaskDescription> v =
          (tempData.find(taskRequest->id_)!=tempData.end()) ?
            tempData.find(taskRequest->id_)->second : std::vector<TaskDescription>();

        v.push_back(*taskRequest);
        tempData.insert(std::pair<boost::uuids::uuid,std::vector<TaskDescription>>(taskRequest->id_, v));

        if(v.size() == incMailboxes_.size()) {
          triggerOneTimeTask(i);
          // remove data
          tempData.erase(taskRequest->id_);
          sleep_sem->release();
        }
      }
    }catch(simgrid::TimeoutException){continue;}
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
    simgrid::s4u::Actor::create(serviceName_+"_"+s+"polling",s4u::Host::current(),[&]{pollnet(s);});
    XBT_INFO("polling on mailbox %s", s.c_str());
  }

  while(1) {
    // execute events that need be executed now
    while(!nextEvtQueue.empty() && nextEvtQueue.top()->date <= Engine::get_instance()->get_clock()) {
      XBT_DEBUG("In run: %d, nextEvtQueue: %d, netxEvt date: %lf",
        tasks.size(), nextEvtQueue.size(), nextEvtQueue.top()->date);
      EvntQ *currentEvent = nextEvtQueue.top();
      nextEvtQueue.pop();

      if (RatioChange* t = dynamic_cast<RatioChange*>(currentEvent)) {
        changeRatio(t->id, t->visitsPerSec);
      } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(currentEvent)) {
        if (availableHostsList_.size() <= nextHost_)
          nextHost_ = 0;
        simgrid::s4u::Mailbox* mbp = simgrid::s4u::Mailbox::by_name(serviceName_+"_data");

        try{
          mbp->put(t, (t->dSize == -1) ? 1 : t->dSize);
        }catch(simgrid::TimeoutException e){XBT_INFO("WTFFF %s", e.what());}
	      ++task_count;

        if(! t->repeat)
          removeTask(t->id_);

        nextHost_++;
        nextHost_ = nextHost_ % availableHostsList_.size();

        // add next event to the queue (date defined through interspawndelay or file timestamps)
        if (t->repeat) {
          t->date = Engine::get_instance()->get_clock() + (1 / t->interSpawnDelay);
          nextEvtQueue.push(t);
        } else if (t->ts_file->is_open()) {
          if (t->ts_file->eof()) {
            t->ts_file->close();
          } else {
            std::string timestamp;
            std::getline(*(t->ts_file), timestamp);
            try {
              t->date = std::stod(timestamp);
            } catch(const std::invalid_argument& e) {
              //std::cout << "exception for stod(timestamp)" << std::endl;
            }
            nextEvtQueue.push(t);
          }
        }
      } else {
        std::cout << "wut";
        exit(0);  // Shouldn't happen
      }
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

