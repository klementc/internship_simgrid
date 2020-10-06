#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <iostream>

#include "simgrid/s4u/Engine.hpp"
#include "simgrid/s4u/Comm.hpp"
#include "simgrid/s4u/Semaphore.hpp"
#include "simgrid/kernel/future.hpp"
#include "simgrid/msg.h"

#include "ElasticTask.hpp"

using namespace simgrid;
using namespace s4u;


XBT_LOG_NEW_DEFAULT_CATEGORY(elastic, "elastic tasks");

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------
ElasticTaskManager::ElasticTaskManager() : keepGoing(true) {
  sleep_sem = s4u::Semaphore::create(0);
}

size_t ElasticTaskManager::addElasticTask(Host *host, double flopsTask, double interSpawnDelay) {
  tasks.push_back(TaskDescription(flopsTask, interSpawnDelay, host, Engine::get_instance()->get_clock()));
  tasks.at(tasks.size() - 1).id = tasks.size() - 1;
  if (interSpawnDelay > 0.0) {
    TaskDescription *newTask = new TaskDescription(tasks.at(tasks.size() - 1));
    nextEvtQueue.push(newTask);
    XBT_DEBUG("tasks: %d nextEvtQueue: %d", tasks.size(),nextEvtQueue.size());
    sleep_sem->release();
  }
  return tasks.size() - 1;
}

void ElasticTaskManager::addHost(size_t id, Host *host) {
  tasks.at(id).hosts.push_back(host);
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
 *  Sets the function to be called at the end of each microtask
 */
void ElasticTaskManager::setOutputFunction(size_t id, std::function<void()> code) {
  tasks.at(id).outputFunction = code;
  simpleChangeTask(id);
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

void ElasticTaskManager::kill() {
  keepGoing = false;
}

void ElasticTaskManager::run() {
  unsigned long long task_count = 0;
  while(1) {
    while(!nextEvtQueue.empty() && nextEvtQueue.top()->date <= Engine::get_instance()->get_clock()) {
      XBT_DEBUG("In run: %d, nextEvtQueue: %d, netxEvt date: %lf", 
        tasks.size(), nextEvtQueue.size(), nextEvtQueue.top()->date);
      EvntQ *currentEvent = nextEvtQueue.top();
      nextEvtQueue.pop();  // TODO, is it deleted?
      //simcall_mutex_unlock(queue_mutex);
      //if(!nextEvtQueue.empty())
      XBT_DEBUG("In run2: %d, nextEvtQueue: %d, netxEvt date: %lf", 
        tasks.size(), nextEvtQueue.size(), nextEvtQueue.top()->date);
      if (RatioChange* t = dynamic_cast<RatioChange*>(currentEvent)) {
        changeRatio(t->id, t->visitsPerSec);
      } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(currentEvent)) {
        if (t->hosts.size() <= t->nextHost) {
          t->nextHost = 0;
        }
	      XBT_INFO("create actor");
        Actor::create("ET"+std::to_string(task_count), t->hosts.at(t->nextHost), [t, task_count] {
          XBT_DEBUG("Taskstart: %f, flops: %f, taskcount: %d", 

          Engine::get_instance()->get_clock(), t->flops, task_count);
          this_actor::execute(t->flops);
          
          XBT_DEBUG("Taskend: %f, flops: %f, taskcount: %d", 
            Engine::get_instance()->get_clock(), t->flops, task_count);

          // task finished, call output function
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
    if(!nextEvtQueue.empty()) {
      sleep_sem->acquire_timeout(nextEvtQueue.top()->date - Engine::get_instance()->get_clock()); 
    } else {
      sleep_sem->acquire_timeout(999.0);
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

void ElasticTask::setTimestampsFile(std::string filename) {
  etm->setTimestampsFile(id, filename);
}
