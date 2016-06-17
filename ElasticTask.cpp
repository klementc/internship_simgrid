#include "ElasticTask.hpp"
#include "simgrid/s4u/engine.hpp"
#include "simgrid/s4u/comm.hpp"
#include "simgrid/s4u/forward.hpp"
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include "simgrid/msg.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

ratioChange rC(size_t id, double visitsPerSec) {
    ratioChange res;
    res.id = id;
    res.visitsPerSec = visitsPerSec;
    return res;
}

ratioChange rC(size_t id, double date, double visitsPerSec) {
    ratioChange res;
    res.id = id;
    res.date = date;
    res.visitsPerSec = visitsPerSec;
    return res;
}

taskDescription tD(double flops, double interSpawnDelay, simgrid::s4u::Host *host) {
    taskDescription res;
    res.flops = flops;
    res.interSpawnDelay = interSpawnDelay;
    res.host = host;
    return res;
}

evntQ eQ(double date, ratioChange params) {
    evntQ res;
    res.date = date;
    res.eventEnum = evntQ::ratioChange_type;
    res.instruction.params_rC = params;
    return res;
}

evntQ eQ(double date, taskDescription params) {
    evntQ res;
    res.date = date;
    res.eventEnum = evntQ::taskDescription_type;
    res.instruction.params_tD = params;
    return res;
}

bool operator<(const evntQ& lhs, const evntQ& rhs) {
  return lhs.date < rhs.date;
}

streamET sET(size_t idOutput, double ratioLoad) {
    streamET res;
    res.destET = idOutput;
    res.ratioLoad = ratioLoad;
    return res;
}

using namespace simgrid;
using namespace s4u;

// ELASTICTASKMANAGER --------------------------------------------------------------------------------------------------

ElasticTaskManager::ElasticTaskManager(const char *name, Host *host, std::function<void()> code) :
    Actor(name, host, code) {}

size_t ElasticTaskManager::addElasticTask(Host *host, double flopsTask, double interSpawnDelay) {
    tasks.push_back(tD(flopsTask, interSpawnDelay, host));
    tasks.at(tasks.size() - 1).id = tasks.size() - 1;
    if (interSpawnDelay > 0.0) {
        nextEvtQueue.push(eQ(Engine::instance()->getClock(), tasks.at(tasks.size() - 1)));
    }
    return tasks.size() - 1;
}

void ElasticTaskManager::changeRatio(size_t id, double visitsPerSec) {
    removeTask(id);
    tasks.at(id).interSpawnDelay = visitsPerSec;
    if(visitsPerSec > 0.0) {
        nextEvtQueue.push(eQ(Engine::instance()->getClock(), tasks.at(id)));
    }
}

void ElasticTaskManager::changeTask(size_t id, double flops) {
    removeTask(id);
    tasks.at(id).flops = flops;
    if(tasks.at(id).interSpawnDelay > 0.0) {
        nextEvtQueue.push(eQ(Engine::instance()->getClock(), tasks.at(id)));
    }
}

void ElasticTaskManager::simpleChangeTask(size_t id) {
    taskDescription newTask = tasks.at(id);
    std::priority_queue<evntQ, std::vector<evntQ>, std::less<evntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(nextEvtQueue.top().eventEnum != evntQ::taskDescription_type ||
                nextEvtQueue.top().instruction.params_tD.id != id) {
            newNextEvtQueue.push(nextEvtQueue.top());
        } else {
            if(newNextEvtQueue.top().instruction.params_tD.repeat) {
                newNextEvtQueue.push(eQ(newNextEvtQueue.top().date, newTask));
            }
        }
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::addRatioChange(size_t id, double date, double visitsPerSec) {
    nextEvtQueue.push(eQ(date, rC(id, visitsPerSec)));
}

void ElasticTaskManager::removeTask(size_t id) {
    std::priority_queue<evntQ, std::vector<evntQ>, std::less<evntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(nextEvtQueue.top().eventEnum != evntQ::taskDescription_type ||
                nextEvtQueue.top().instruction.params_tD.id != id) {
            newNextEvtQueue.push(nextEvtQueue.top());
        }  // maybe do a break once you found the task and copy without worry on another while(1)
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::removeRatioChanges(size_t id) {
    std::priority_queue<evntQ, std::vector<evntQ>, std::less<evntQ> > newNextEvtQueue;
    while(!nextEvtQueue.empty()) {
        if(nextEvtQueue.top().eventEnum != evntQ::ratioChange_type ||
                nextEvtQueue.top().instruction.params_rC.id != id) {
            newNextEvtQueue.push(nextEvtQueue.top());
        }
        nextEvtQueue.pop();
    }
    nextEvtQueue = newNextEvtQueue;
}

void ElasticTaskManager::triggerOneTimeTask(size_t id) {
    taskDescription newTask = tasks.at(id);
    newTask.repeat = false;
    nextEvtQueue.push(eQ(0.0, newTask));
}

void ElasticTaskManager::triggerOneTimeTask(size_t id, double ratioLoad) {
    taskDescription newTask = tasks.at(id);
    newTask.repeat = false;
    newTask.flops = newTask.flops * ratioLoad;
    nextEvtQueue.push(eQ(0.0, newTask));
}

void ElasticTaskManager::addOutputStream(size_t sourceET, size_t destET, double ratioLoad) {
    tasks.at(sourceET).outputStreams.push_back(sET(destET, ratioLoad));
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
        //std::string evnt = nextEvtQueue.top().instruction;
        //std::vector<std::string> evntSplit = split(evnt, ' ');
        //if(evntSplit.at(0) == "changeRatio") {
        //    tasks.at(std::stoi(evntSplit.at(1))).interSpawnDelay = std::stod(evntSplit.at(2));
        //}
        evntQ currentEvent = nextEvtQueue.top();
        switch(currentEvent.eventEnum) {
            case evntQ::ratioChange_type:
                changeRatio(currentEvent.instruction.params_rC.id, currentEvent.instruction.params_rC.visitsPerSec);
                break;
            case evntQ::taskDescription_type:
                this_actor::execute(currentEvent.instruction.params_tD.flops);
                if (currentEvent.instruction.params_tD.repeat) {
                    nextEvtQueue.push(eQ(Engine::instance()->getClock() + (1 /
                                          currentEvent.instruction.params_tD.interSpawnDelay),
                                         currentEvent.instruction.params_tD));
                }
                for(std::vector<streamET>::iterator it = currentEvent.instruction.params_tD.outputStreams.begin();
                        it != currentEvent.instruction.params_tD.outputStreams.end(); ++it) {
                    triggerOneTimeTask((*it).destET, (*it).ratioLoad);
                }
                break;
        }
        nextEvtQueue.pop();
    }
    //for(std::vector<taskDescription>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
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

ElasticTask::ElasticTask(Host *host, double flopsTask, std::vector<ratioChange> fluctuations, ElasticTaskManager *etm_) {
    etm = etm_;
    id = etm->addElasticTask(host, flopsTask, 0.0);
    setTriggerRatioVariation(fluctuations);
}

void ElasticTask::setTriggerRatioVariation(std::vector<ratioChange> fluctuations) {
    etm->removeRatioChanges(id);
    for(std::vector<ratioChange>::iterator it = fluctuations.begin(); it != fluctuations.end(); ++it) {
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
