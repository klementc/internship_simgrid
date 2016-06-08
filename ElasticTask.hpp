#ifndef ELASTICTASK_HPP
#define ELASTICTASK_HPP

#include <../simgrid/include/simgrid/s4u/actor.hpp>

typedef struct ratioChange {
    double time;
    double visitsPerSec;
} ratioChange;

ratioChange rC(double time, double visitsPerSec) {
    ratioChange res;
    res.time = time;
    res.visitsPerSec = visitsPerSec;
    return res;
}

using namespace simgrid;
using namespace s4u;

/** @brief Elastic task.
 */
XBT_PUBLIC_CLASS ElasticTask : public Actor {
    explicit ElasticTask(smx_process_t smx_proc);
    public:
        ElasticTask(const char* name, s4u::Host *host, std::function<void()> code);  // no killTime
        Elastictask(const char* name, s4u::Host *host, C code);
        void ~ElasticTask();

        /** Set the variation of visits/triggers per seconds over the time. */
        void setTriggerRatioVariation(vector<ratioChange> fluctuations);
        //void setTriggerTrace(string fileName);
        /** Add a link like in a workflow of tasks. */
        void addOutputStream(ElasticTask e2);
        /** Modify the normal task that is triggered.
         *
         * You can use it to reduce the computational size for example.
         */
        void modifyTask(task);
        /** Update on the fly the fluctuation of visit/trigger ratio */
        void updateTriggerRatioVariation(vector<date, ratio>);
        /** Update on the fly the current visit/trigger ratio and delete the old vector of fluctuation. */
        void setRatioVariation(date, ratio);
        /** Trigger the inherent task one time.
         *
         * Could be used for the output stream. */
        void triggerOneTime();
        /** Execute the elastic task until death.
         *
         * It will find itself the resources (e.g. flops) available.
         */
        void execute();
        //void sleep(double duration);  // We probably should just change the visits ratio to 0.
        //void recv(Mailbox &chan);  // Is it useful ? Maybe for workflow
        //void send(Mailbox &chan, void*payload, size_t simulatedSize);  // Is it useful ? Maybe for workflow
};

namespace this_task {

};

#endif

class Eve {
    public:
        void operator()() {
            ratioChange ratioFluctuation2[2] = {rC(0.0, 5), rC(40.0, 4)};
            std::vector<ratioChange> ratioFluctuation(2);
            ratioFluctuation.assign(&ratioFluctuation2[0], &ratioFluctuation2[0]+2);
            this_task.setRatioVariation(ratioFluctuation);
            sleep(99999999);
            this_task.kill();
        }
};

int main(int argc, char **argv) {
    Engine *e = new Engine(&argc, argv);
    e->loadPlatform("../../platforms/two_hosts.xml");
    new ElasticTask("eve", Host::by_name("Tremblay"), Eve());
    e->run();
    return 0;
}
