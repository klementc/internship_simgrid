#include "ElasticTask.hpp"
#include "simgrid/s4u/engine.hpp"
#include "simgrid/s4u/comm.hpp"
#include "simgrid/s4u/forward.hpp"
#include <vector>
#include "simgrid/msg.h"

using namespace simgrid;
using namespace s4u;

//ElasticTask::ElasticTask(smx_process_t smx_proc) : Actor::Actor(smx_proc) { }

ElasticTask::ElasticTask(const char *name, s4u::Host *host, std::function<void()> code, msg_task_t _task)
        : Actor::Actor(name, host, std::move(code)) {
    task = _task;
}
//ElasticTask::ElasticTask(const char *name, s4u::Host *host, C code)
//        : Actor::Actor(name, host, code) {}

ElasticTask::~ElasticTask() {
    Actor::~Actor();
}


//namespace this_task {
    void ElasticTask::setTriggerRatioVariation(std::vector<ratioChange> fluctuations) {
        ratioFluctuations = fluctuations;
        for (std::vector<ratioChange>::iterator it = fluctuations.begin(); it != fluctuations.end(); ++it) {
            if ((*it).time <= (Engine::instance())->getClock()) {
                currentRatio = (*it).visitsPerSec;
                currentIndex = it - fluctuations.begin();
                break;
            }
        }
    }

    void ElasticTask::addOutputStream(ElasticTask e2) {  // TODO, currently it's 1:1 ratio for executions
        outputStreams.push_back(e2);
    }

    void ElasticTask::modifyTask(msg_task_t _task) {
        task = _task;

    }
//void ElasticTask::modifyTask(C code) {}

//    void ElasticTask::modifyHost(s4u::Host *host, std::function<void()> code) {
//        this->set_pimpl(simcall_process_create(this->getName(),std::move(code),nullptr,host->name().c_str(),-1,nullptr,
//                                               0));
//    }

    void ElasticTask::updateTriggerRatioVariation(std::vector <ratioChange> fluctuations) {
        ratioFluctuations = fluctuations;
    }

    void ElasticTask::setTriggerRatio(double ratio) {
        currentRatio = ratio;
        ratioFluctuations.clear();
    }

    void ElasticTask::triggerOneTime() {
//        double flops = this->getHost().getAvailableFlops();  // TODO, manage available flops
        MSG_task_execute(task);
    }

    void ElasticTask::execute() {
//        double flops = this->getHost().getAvailableFlops();  // TODO, manage available flops
        while (1) {
            for (int i = 0; i < currentRatio; i++) {  // TODO, for now it acts like currentRation is an int
                MSG_task_execute(task);  // TODO, find a way to execute them on another thread or something
                for (std::vector<ElasticTask>::iterator it = outputStreams.begin(); it != outputStreams.end(); it++) {
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
    void ElasticTask::kill() {
        Actor::kill();
    }  // I guess we don't have to delete the output tasks
//}
