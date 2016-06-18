#include "ElasticTask.hpp"
#include "simgrid/s4u/engine.hpp"
#include "simgrid/s4u/comm.hpp"
#include "simgrid/s4u/forward.hpp"
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include "simgrid/msg.h"

bool operator<(const EvntQ& lhs, const EvntQ& rhs) {
  return lhs.date < rhs.date;
}

using namespace simgrid;
using namespace s4u;

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------

ElasticTaskManager::ElasticTaskManager(const char *name, Host *host, std::function<void()> code) :
    Actor(name, host, code) {}

size_t ElasticTaskManager::addElasticTask(Host *host, double flopsTask, double interSpawnDelay) {
    tasks.push_back(TaskDescription(flopsTask, interSpawnDelay, host, Engine::instance()->getClock()));
    tasks.at(tasks.size() - 1).id = tasks.size() - 1;
    if (interSpawnDelay > 0.0) {
        nextEvtQueue.push(tasks.at(tasks.size() - 1));
    }
    return tasks.size() - 1;
}

void ElasticTaskManager::changeRatio(size_t id, double visitsPerSec) {
    removeTask(id);
    tasks.at(id).interSpawnDelay = visitsPerSec;
    tasks.at(id).date = Engine::instance()->getClock();
    if(visitsPerSec > 0.0) {
        nextEvtQueue.push(tasks.at(id));
    }
}

void ElasticTaskManager::changeTask(size_t id, double flops) {
    removeTask(id);  // TODO, should it remove only regular task (not triggerOnce)
    tasks.at(id).flops = flops;
    tasks.at(id).date = Engine::instance()->getClock();
    if(tasks.at(id).interSpawnDelay > 0.0) {
        nextEvtQueue.push(tasks.at(id));
    }
}

// TODO, All these change task method are too similar and messy
void ElasticTaskManager::simpleChangeTask(size_t id) {  // A change has been done and we want to update the repeating
    TaskDescription newTask = tasks.at(id);
    std::priority_queue<EvntQ, std::vector<EvntQ>, std::less<EvntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
            newNextEvtQueue.push(nextEvtQueue.top());
        } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(&nextEvtQueue.top())) {
            if (t->id != id) {
                newNextEvtQueue.push(nextEvtQueue.top());
            } else if (t->repeat) {
                newTask.date = t->date;
                newNextEvtQueue.push(newTask);
            }
        }
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::addRatioChange(size_t id, double date, double visitsPerSec) {
    nextEvtQueue.push(RatioChange(id, date, visitsPerSec));
}

void ElasticTaskManager::removeTask(size_t id) {  // remove all even non repeat
    std::priority_queue<EvntQ, std::vector<EvntQ>, std::less<EvntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(strcmp(typeid(nextEvtQueue.top()).name(), "TaskDescription")) {
            newNextEvtQueue.push(nextEvtQueue.top());
        } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(&nextEvtQueue.top())) {
            if (t->id != id) {
                newNextEvtQueue.push(nextEvtQueue.top());
            }
        }
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::removeRatioChanges(size_t id) {
    std::priority_queue<EvntQ, std::vector<EvntQ>, std::less<EvntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(strcmp(typeid(nextEvtQueue.top()).name(), "RatioChange")) {
            newNextEvtQueue.push(nextEvtQueue.top());
        } else if (RatioChange* t = dynamic_cast<RatioChange*>(&nextEvtQueue.top())) {
            if (t->id != id) {
                newNextEvtQueue.push(nextEvtQueue.top());
            }
        }
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::triggerOneTimeTask(size_t id) {
    TaskDescription newTask = tasks.at(id);
    newTask.repeat = false;
    newTask.date = 0.0;
    nextEvtQueue.push(newTask);
}

void ElasticTaskManager::triggerOneTimeTask(size_t id, double ratioLoad) {
    TaskDescription newTask = tasks.at(id);
    newTask.repeat = false;
    newTask.flops = newTask.flops * ratioLoad;
    newTask.date = 0.0;
    nextEvtQueue.push(newTask);
}

void ElasticTaskManager::addOutputStream(size_t sourceET, size_t destET, double ratioLoad) {
    tasks.at(sourceET).outputStreams.push_back(streamET(destET, ratioLoad));
    simpleChangeTask(sourceET);
}

void ElasticTaskManager::removeOutputStream(size_t sourceET, size_t destET) {
    for(std::vector<streamET>::iterator it = tasks.at(sourceET).outputStreams.begin();
            it != tasks.at(sourceET).outputStreams.end(); ++it) {
        if((*it).destET == destET) {
            tasks.at(sourceET).outputStreams.erase(it - tasks.at(sourceET).outputStreams.begin());
        }
    }
    simpleChangeTask(sourceET);
}

void ElasticTaskManager::execute() {
    while(nextEvtQueue.top().date <= Engine::instance()->getClock()) {
        EvntQ currentEvent = nextEvtQueue.top();
        if (RatioChange* t = dynamic_cast<RatioChange*>(&currentEvent)) {
                changeRatio(t->id, t->visitsPerSec);
        } else if (TaskDescription* t = dynamic_cast<TaskDescription*>(&currentEvent)) {
            this_actor::execute(t->flops);
            if (t->repeat) {
                t->date = Engine::instance()->getClock() + (1 / t->interSpawnDelay);
                nextEvtQueue.push(*t);
            }
            for(std::vector<streamET>::iterator it = t->outputStreams.begin(); it != t->outputStreams.end(); ++it) {
                triggerOneTimeTask((*it).destET, (*it).ratioLoad);
            }
        }
        nextEvtQueue.pop();
    }
    //for(std::vector<TaskDescription>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
    //    for(int i = 0; i < (*it).interSpawnDelay ; i++) {
    //        msg_task_t task = MSG_task_create(std::to_string(i).c_str(), (*it).flops, 0.0, NULL);
    //        MSG_task_execute(task);
    //    }
    //}
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

ElasticTask::ElasticTask(Host *host, double flopsTask, std::vector<RatioChange> fluctuations, ElasticTaskManager *etm_) {
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

void ElasticTask::addOutputStream(size_t idOutput, double ratioLoad) {
    etm->addOutputStream(id, idOutput, ratioLoad);
}

void ElasticTask::removeOutputStream(size_t idOutput) {
    etm->removeOutputStream(id, idOutput);
}

//void ElasticTask::addOutputStreams(std::vector<size_t> streams) {

//}
