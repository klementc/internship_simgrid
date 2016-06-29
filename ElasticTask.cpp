#include "ElasticTask.hpp"
#include "simgrid/s4u/engine.hpp"
#include "simgrid/s4u/comm.hpp"
#include "simgrid/s4u/forward.hpp"
#include "simgrid/kernel/future.hpp"
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>
#include "simgrid/msg.h"

bool operator<(const EvntQ& lhs, const EvntQ& rhs) {
  return lhs.date < rhs.date;
}

using namespace simgrid;
using namespace s4u;

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------

ElasticTaskManager::ElasticTaskManager() : keepGoing(true) {
  sleep_sem = MSG_sem_init(0);
}

//ElasticTaskManager::~ElasticTaskManager() {}

size_t ElasticTaskManager::addElasticTask(Host *host, double flopsTask, double interSpawnDelay) {
  tasks.push_back(TaskDescription(flopsTask, interSpawnDelay, host, Engine::instance()->getClock()));
  tasks.at(tasks.size() - 1).id = tasks.size() - 1;
  if (interSpawnDelay > 0.0) {
    TaskDescription *newTask = new TaskDescription(tasks.at(tasks.size() - 1));
    nextEvtQueue.push(newTask);
    MSG_sem_release(sleep_sem);
  }
  return tasks.size() - 1;
}

void ElasticTaskManager::addHost(size_t id, Host *host) {
  tasks.at(id).hosts.push_back(host);
}

void ElasticTaskManager::changeRatio(size_t id, double visitsPerSec) {
  removeTask(id);
  tasks.at(id).interSpawnDelay = visitsPerSec;
  tasks.at(id).date = Engine::instance()->getClock();
  if(visitsPerSec > 0.0) {
    nextEvtQueue.push(&(tasks.at(id)));
    MSG_sem_release(sleep_sem);
  }
}

void ElasticTaskManager::changeTask(size_t id, double flops) {
  removeTask(id);  // TODO, should it remove only regular task (not triggerOnce)
  tasks.at(id).flops = flops;
  tasks.at(id).date = Engine::instance()->getClock();
  if(tasks.at(id).interSpawnDelay > 0.0) {
    nextEvtQueue.push(&(tasks.at(id)));
    MSG_sem_release(sleep_sem);
  }
}

// TODO, All these change task method are too similar and messy
void ElasticTaskManager::simpleChangeTask(size_t id) {  // A change has been done and we want to update the repeating
  TaskDescription *newTask = new TaskDescription(tasks.at(id));
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, std::less<EvntQ*> > newNextEvtQueue;
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

void ElasticTaskManager::addRatioChange(size_t id, double date, double visitsPerSec) {
  RatioChange *rC = new RatioChange(id, date, visitsPerSec);
  nextEvtQueue.push(rC);
  if (date < nextEvtQueue.top()->date) {
    MSG_sem_release(sleep_sem);
  }
}

void ElasticTaskManager::removeTask(size_t id) {  // remove all even non repeat
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, std::less<EvntQ*> > newNextEvtQueue;
  while(!nextEvtQueue.empty()) {
    if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
      newNextEvtQueue.push(nextEvtQueue.top());
    } else if (const TaskDescription* t = dynamic_cast<const TaskDescription*>(nextEvtQueue.top())) {
      if (t->id != id) {
        newNextEvtQueue.push(nextEvtQueue.top());
      }
    }
    nextEvtQueue.pop();
  }
  nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::removeRatioChanges(size_t id) {
  std::priority_queue<EvntQ*, std::vector<EvntQ*>, std::less<EvntQ*> > newNextEvtQueue;
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
  MSG_sem_release(sleep_sem);
}

void ElasticTaskManager::triggerOneTimeTask(size_t id, double ratioLoad) {  // TODO, network load to add
  TaskDescription *newTask = new TaskDescription(tasks.at(id));
  newTask->repeat = false;
  newTask->flops = newTask->flops * ratioLoad;
  newTask->date = 0.0;
  nextEvtQueue.push(newTask);
  MSG_sem_release(sleep_sem);
}

void ElasticTaskManager::setOutputFunction(size_t id, std::function<void()> code) {
  tasks.at(id).outputFunction = code;
  simpleChangeTask(id);
}

void ElasticTaskManager::kill() {
  keepGoing = false;
}

void ElasticTaskManager::run() {
  unsigned long long task_count = 0;
  while(1) {
    while(!nextEvtQueue.empty() && nextEvtQueue.top()->date <= Engine::instance()->getClock()) {
      EvntQ *currentEvent = nextEvtQueue.top();
      if (RatioChange* t = dynamic_cast<RatioChange*>(currentEvent)) {
        changeRatio(t->id, t->visitsPerSec);
      } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(currentEvent)) {
        std::string host_name = t->hosts.at(t->nextHost)->name();
        Actor(nullptr, t->hosts.at(t->nextHost), [this, t, task_count, host_name] {
          std::cout << "TaskStart " << Engine::instance()->getClock() << " " << t->flops << " " << task_count
                    << " " << host_name << std::endl;
          this_actor::execute(t->flops);
          t->outputFunction();
        });
        ++task_count;
        // The shifting of hosts will occur before but should be negligible
        if(t->nextHost == t->hosts.size() - 1) {
          t->nextHost = 0;
        } else {
          t->nextHost++;  // TODO, use proper index stuff
        }
        if (t->repeat) {
          t->date = Engine::instance()->getClock() + (1 / t->interSpawnDelay);
          nextEvtQueue.push(t);
        }
      } else {
        std::cout << "wut";
        exit(0);  // Should'nt happen
      }
      nextEvtQueue.pop();  // TODO, is it deleted?
      //delete currentEvent;
    }
    if(!keepGoing) {
      break;
    }
    if(!nextEvtQueue.empty()) {
      MSG_sem_acquire_timeout(sleep_sem, nextEvtQueue.top()->date - Engine::instance()->getClock());
    } else {
      MSG_sem_acquire_timeout(sleep_sem, 999.0);
    }
  }
}

// ELASTICTASK ---------------------------------------------------------------------------------------------------------

ElasticTask::ElasticTask(Host *host, double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(host, flopsTask, interSpawnDelay);
}

ElasticTask::ElasticTask(Host *host, double flopsTask, ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(host, flopsTask, 0.0);
}

ElasticTask::ElasticTask(Host *host, double flopsTask, std::vector<RatioChange> fluctuations,
                         ElasticTaskManager *etm_) {
  etm = etm_;
  id = etm->addElasticTask(host, flopsTask, 0.0);
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

void ElasticTask::addHost(Host *host) {
  etm->addHost(id, host);
}

void ElasticTask::setOutputFunction(std::function<void()> code) {
  etm->setOutputFunction(id, code);
}
