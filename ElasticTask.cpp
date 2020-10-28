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

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------
ElasticTaskManager::ElasticTaskManager(std::string name)
  : rcvMailbox_(name), nextHost_(0), keepGoing(true), processRatio_(1e7){
  XBT_DEBUG("%s", rcvMailbox_.c_str());
  sg_host_load_plugin_init();
  sleep_sem = s4u::Semaphore::create(0);
}

void ElasticTaskManager::setProcessRatio(int64_t pr)
{
  processRatio_ = pr;
}

size_t ElasticTaskManager::addElasticTask(double flopsTask, double interSpawnDelay, double s) {
  tasks.push_back(TaskDescription(flopsTask, interSpawnDelay, Engine::get_instance()->get_clock(), s));
  tasks.at(tasks.size() - 1).id = tasks.size() - 1;
  if (interSpawnDelay > 0.0) {
    nextEvtQueue.push(&(tasks.at(tasks.size() -1 )));
    XBT_DEBUG("tasks: %d nextEvtQueue: %d", tasks.size(),nextEvtQueue.size());
    sleep_sem->release();
  }
  return tasks.size() - 1;
}

size_t ElasticTaskManager::addElasticTask(double flopsTask, double interSpawnDelay) {
  return addElasticTask(flopsTask, interSpawnDelay, -1);
}

void ElasticTaskManager::addHost(Host *host) {
  availableHostsList_.push_back(host);
}

void ElasticTaskManager::changeRatio(size_t id, double visitsPerSec) {
  removeTask(id);
  tasks.at(id).interSpawnDelay = visitsPerSec;
  tasks.at(id).date = Engine::get_instance()->get_clock();
  if(visitsPerSec > 0.0) {
    TaskDescription *newTask = new TaskDescription(tasks.at(id));
    nextEvtQueue.push(newTask);
    sleep_sem->release();
  }
}

void ElasticTaskManager::changeTask(size_t id, double flops) {
  removeTask(id);  // TODO, should it remove only regular task (not triggerOnce)
  tasks.at(id).flops = flops;
  tasks.at(id).date = Engine::get_instance()->get_clock();
  if(tasks.at(id).interSpawnDelay > 0.0) {
    TaskDescription *newTask = new TaskDescription(tasks.at(id));
    nextEvtQueue.push(newTask);
    sleep_sem->release(); //MSG_sem_release(sleep_sem);
  }
}

// TODO, All these change task method are too similar and messy
void ElasticTaskManager::simpleChangeTask(size_t id) {  // A change has been done and we want to update the repeating
  TaskDescription *newTask = new TaskDescription(tasks.at(id));
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> newNextEvtQueue;
  while(!nextEvtQueue.empty()) {
    if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
      newNextEvtQueue.push(nextEvtQueue.top());
    } else if (const TaskDescription* t = dynamic_cast<const TaskDescription*>(nextEvtQueue.top())) {
      if (t->id != id) {
        newNextEvtQueue.push(nextEvtQueue.top());
      } else if (t->repeat) {
        newTask->date = t->date;
        newNextEvtQueue.push(newTask);
      }
    }
    nextEvtQueue.pop();
  }
  nextEvtQueue = newNextEvtQueue;
}

/**
 * Modify the arrival rate of one of the tasks from date
 */
void ElasticTaskManager::addRatioChange(size_t id, double date, double visitsPerSec) {
  RatioChange *rC = new RatioChange(id, date, visitsPerSec);
  nextEvtQueue.push(rC);
  if (date < nextEvtQueue.top()->date) {
    sleep_sem->release();
  }
}

void ElasticTaskManager::removeTask(size_t id) {  // remove all even non repeat
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> newNextEvtQueue;
  while(!nextEvtQueue.empty()) {
    if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
      newNextEvtQueue.push(nextEvtQueue.top());
    } else if (const TaskDescription* t = dynamic_cast<const TaskDescription*>(nextEvtQueue.top())) {
      if (t->id != id) {
        newNextEvtQueue.push(nextEvtQueue.top());
      } else {
        delete nextEvtQueue.top();
      }
    }
    nextEvtQueue.pop();
  }
  nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::removeRatioChanges(size_t id) {
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

void ElasticTaskManager::triggerOneTimeTask(size_t id) {
  TaskDescription *newTask = new TaskDescription(tasks.at(id));
  newTask->repeat = false;
  newTask->date = 0.0;
  nextEvtQueue.push(newTask);
  sleep_sem->release();
}

void ElasticTaskManager::triggerOneTimeTask(size_t id, double ratioLoad) {  // TODO, network load to add
  TaskDescription *newTask = new TaskDescription(tasks.at(id));
  newTask->repeat = false;
  newTask->flops = newTask->flops * ratioLoad;
  newTask->date = 0.0;
  nextEvtQueue.push(newTask);
  sleep_sem->release();
}

/**
 * Import timestamps (one timestamp per line) from a file
 */
void ElasticTaskManager::setTimestampsFile(size_t id, std::string filename) {
  tasks.at(id).repeat = false;
  tasks.at(id).ts_file->open(filename);
  removeTask(id);
  std::string timestamp;
  if(tasks.at(id).ts_file->is_open()) {
    if(!tasks.at(id).ts_file->eof()) {
      std::getline(*(tasks.at(id).ts_file), timestamp);
      tasks.at(id).date = std::stod(timestamp);
      TaskDescription *newTask = new TaskDescription(tasks.at(id));
      nextEvtQueue.push(newTask);
    } else {
      tasks.at(id).ts_file->close();
    }
  }
  sleep_sem->release();
}

void ElasticTaskManager::removeHost(int i){
  if(i<availableHostsList_.size()){
    availableHostsList_.erase(availableHostsList_.begin()+i);
  }else{
    XBT_INFO("Cannot remove element at position %d, overflow", i);
  }
}

unsigned int ElasticTaskManager::getInstanceAmount(){
  return availableHostsList_.size();
}
std::vector<double> ElasticTaskManager::getCPULoads(){
  std::vector<double> v;
  for (int i=0; i<availableHostsList_.size();i++){
    v.push_back(sg_host_get_current_load(availableHostsList_.at(i)));
  }
  return v;
}

/**
 * Will stop run()'s infinite loop
 */
void ElasticTaskManager::kill() {
  keepGoing = false;
}

void ElasticTaskManager::pollnet(){
  Mailbox* recvMB = simgrid::s4u::Mailbox::by_name(rcvMailbox_.c_str());
  while(keepGoing){
    try{
      int* taskRequest = static_cast<int*>(recvMB->get(999));
      int i = addElasticTask(processRatio_, 0, *taskRequest);
      triggerOneTimeTask(i);
      XBT_DEBUG("POLLING RECEIVED size %d", *taskRequest);
      sleep_sem->release();
    }catch(simgrid::TimeoutException){}
  }
}

/**
 * Supervisor of the elastictasks
 * Fetches events at their execution time and creates a new microtask
 * on one of the provided hosts
 */
void ElasticTaskManager::run() {
  // mailbox for incoming requests from other services
  Mailbox* recvMB = simgrid::s4u::Mailbox::by_name(rcvMailbox_.c_str());
  unsigned long long task_count = 0;
  simgrid::s4u::Actor::create(rcvMailbox_+"polling",s4u::Host::current(),[&]{pollnet();});
  while(1) {
    XBT_DEBUG("starting loop, event queue size: %d, %d tasks", nextEvtQueue.size(), tasks.size());


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

	      XBT_DEBUG("create actor");
        Actor::create("ET"+std::to_string(task_count), availableHostsList_.at(nextHost_), [this, t, task_count] {
          XBT_DEBUG("Taskstart: %f, flops: %f, taskcount: %d, avgload: %f\%, computer flops: %f",
          Engine::get_instance()->get_clock(), t->flops, task_count, sg_host_get_avg_load(s4u::Host::current())*100 ,sg_host_get_computed_flops(s4u::Host::current()));

          // receive data from mailbox
          simgrid::s4u::Mailbox* mb = simgrid::s4u::Mailbox::by_name(this_actor::get_host()->get_name()+"_data");
          void* a = mb->get();
          this_actor::execute(t->flops);
          XBT_DEBUG("Taskend: %f, flops: %f, taskcount: %d, avgload: %f\%, computer flops: %f",
          Engine::get_instance()->get_clock(), t->flops, task_count, sg_host_get_avg_load(s4u::Host::current())*100 ,sg_host_get_computed_flops(s4u::Host::current()));

          // task finished, call output function
          outputFunction();
        });
        // send data to process to the instance
        simgrid::s4u::Mailbox* mbp = simgrid::s4u::Mailbox::by_name(availableHostsList_.at(nextHost_)->get_name()+"_data");
        int a;
        mbp->put(&a, (t->dSize == -1) ? 1 : t->dSize);

	      ++task_count;

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

void ElasticTaskManager::setOutputFunction(std::function<void()> f)
{
  outputFunction = f;
}

// ELASTICTASK ---------------------------------------------------------------------------------------------------------

ElasticTask::ElasticTask(double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(flopsTask, interSpawnDelay);
}

ElasticTask::ElasticTask(double flopsTask, ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(flopsTask, 0.0);
}

ElasticTask::ElasticTask(double flopsTask, std::vector<RatioChange> fluctuations,
                         ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(flopsTask, 0.0);
  setTriggerRatioVariation(fluctuations);
}

void ElasticTask::setTriggerRatioVariation(std::vector<RatioChange> fluctuations) {
  etm->removeRatioChanges(id);
  for(std::vector<RatioChange>::iterator it = fluctuations.begin(); it != fluctuations.end(); ++it) {
    etm->addRatioChange(id, (*it).date, (*it).visitsPerSec);
  }
}

void ElasticTask::setRatioVariation(double interSpawnDelay) {
  etm->removeRatioChanges(id);
  etm->changeRatio(id, interSpawnDelay);
}

void ElasticTask::modifyTask(double flops) {
  etm->changeTask(id, flops);
}

void ElasticTask::triggerOneTime() {
  etm->triggerOneTimeTask(id);
}

void ElasticTask::triggerOneTime(double ratioLoad) {
  etm->triggerOneTimeTask(id, ratioLoad);
}


void ElasticTask::setTimestampsFile(std::string filename) {
  etm->setTimestampsFile(id, filename);
}
