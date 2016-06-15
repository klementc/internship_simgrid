#include "ElasticTask.hpp"
#include "simgrid/s4u/engine.hpp"
#include "simgrid/s4u/comm.hpp"
#include "simgrid/s4u/forward.hpp"
#include <vector>
#include "simgrid/msg.h"

ratioChange rC(double time, double visitsPerSec) {
    ratioChange res;
    res.time = time;
    res.visitsPerSec = visitsPerSec;
    return res;
}

using namespace simgrid;
using namespace s4u;

//ElasticTaskManager::ElasticTaskManager(smx_process_t smx_proc) : Actor::Actor(smx_proc) { }

ElasticTaskManager::ElasticTaskManager(const char *name, s4u::Host *host, std::function<void()> code, double _flopsTask, double _bytesTask, void *_dataTask)
        : Actor::Actor(name, host, std::move(code)) {
    flopsTask = _flopsTask;
    bytesTask = _bytesTask;
    dataTask = _dataTask;
}
//ElasticTaskManager::ElasticTaskManager(const char *name, s4u::Host *host, C code)
//        : Actor::Actor(name, host, code) {}

ElasticTaskManager::~ElasticTaskManager() {
    Actor::~Actor();
}


//namespace this_task {
    void ElasticTaskManager::setTriggerRatioVariation(std::vector<ratioChange> fluctuations) {
        ratioFluctuations = fluctuations;
        for (std::vector<ratioChange>::iterator it = fluctuations.begin(); it != fluctuations.end(); ++it) {
            if ((*it).time <= (Engine::instance())->getClock()) {
                currentRatio = (*it).visitsPerSec;
                currentIndex = it - fluctuations.begin();
                break;
            }
        }
    }

    void ElasticTaskManager::addOutputStream(ElasticTaskManager e2) {  // TODO, currently it's 1:1 ratio for executions
        outputStreams.push_back(e2);
    }

    void ElasticTaskManager::modifyTask(double _flopsTask, double _bytesTask, void *_dataTask) {
        flopsTask = _flopsTask;
        bytesTask = _bytesTask;
        dataTask = _dataTask;
    }

//    void ElasticTaskManager::modifyHost(s4u::Host *host, std::function<void()> code) {
//        this->set_pimpl(simcall_process_create(this->getName(),std::move(code),nullptr,host->name().c_str(),-1,nullptr,
//                                               0));
//    }

    void ElasticTaskManager::updateTriggerRatioVariation(std::vector <ratioChange> fluctuations) {
        ratioFluctuations = fluctuations;
    }

    void ElasticTaskManager::setTriggerRatio(double ratio) {
        currentRatio = ratio;
        ratioFluctuations.clear();
    }

    void ElasticTaskManager::triggerOneTime() {
//        double flops = this->getHost().getAvailableFlops();  // TODO, manage available flops
        msg_task_t task = MSG_task_create(getName(), flopsTask, bytesTask, dataTask);
        MSG_task_execute(task);
    }

    void ElasticTaskManager::execute() {
//        double flops = this->getHost().getAvailableFlops();  // TODO, manage available flops
        while (1) {
            for (int i = 0; i < currentRatio; i++) {  // TODO, for now it acts like currentRation is an int
                msg_task_t task = MSG_task_create(getName(), flopsTask, bytesTask, dataTask);
                MSG_task_execute(task);  // TODO, find a way to execute them on another thread or something
                for (std::vector<ElasticTaskManager>::iterator it = outputStreams.begin(); it != outputStreams.end(); it++) {
                    it->triggerOneTime();
                }
            }
            sleep(1);
            if (!ratioFluctuations.empty()) {
                if (ratioFluctuations.end() - ratioFluctuations.begin() > currentIndex
                    && ratioFluctuations.at(currentIndex + 1).visitsPerSec <= (Engine::instance())->getClock()) {
                    currentRatio = ratioFluctuations.at(currentIndex + 1).visitsPerSec;
                    currentIndex = currentIndex + 1;
                }
                if (ratioFluctuations.end() - ratioFluctuations.begin() == currentIndex && currentRatio == 0.0) {
                    break;
                }
            }
        }
    }
//void sleep(double duration);  // We probably should just change the visits ratio to 0.
//void recv(Mailbox &chan);  // Is it useful ? Maybe for workflow
//void send(Mailbox &chan, void*payload, size_t simulatedSize);  // Is it useful ? Maybe for workflow
    void ElasticTaskManager::kill() {
        Actor::kill();
    }  // I guess we don't have to delete the output tasks
//}


//ElasticTask::ElasticTask(s4u::Host *host, double _flopsTask) {
//}
