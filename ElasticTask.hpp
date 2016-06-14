#ifndef SIMGRID_ELASTIC_TASK_H
#define SIMGRID_ELASTIC_TASK_H

#include <vector>

#include <xbt/base.h>
#include <xbt/string.hpp>
#include <xbt/signal.hpp>
#include <xbt/Extendable.hpp>

#include "simgrid/msg.h"

#include <simgrid/simix.h>
#include <simgrid/datatypes.h>
#include <simgrid/s4u/forward.hpp>

#include <simgrid/s4u/actor.hpp>

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

namespace simgrid {
namespace s4u {

/** @brief Elastic task.
 */
    XBT_PUBLIC_CLASS ElasticTask : public Actor {
//    explicit ElasticTask(smx_process_t smx_proc);
    private:
        std::vector<ratioChange> ratioFluctuations;
        double currentRatio;
        long currentIndex;
//        double flopsTask;
        std::vector<ElasticTask> outputStreams;
        msg_task_t task;
    public:
        ElasticTask(const char *name, s4u::Host *host, std::function<void()> code, msg_task_t _task);  // no killTime
        template<class C>
        ElasticTask(const char *name, s4u::Host *host, C code, msg_task_t _task)
            : ElasticTask(name, host, std::function<void()>(std::move(code)), _task) { }

        ~ElasticTask();
//        };

//    namespace this_task {
/** Set the variation of visits/triggers per seconds over the time. */
        void setTriggerRatioVariation(std::vector<ratioChange> fluctuations);
//void setTriggerTrace(string fileName);
/** Add a link like in a workflow of tasks. */
        void addOutputStream(ElasticTask e2);

/** Modify the normal task that is triggered.
 *
 * You can use it to reduce the computational size for example.
 */
        void modifyTask(msg_task_t _task);

        /** Change the host
         */
//        void modifyHost(s4u::Host *host, std::function<void()> code);
/** Update on the fly the fluctuation of visit/trigger ratio
 *
 * Keep the same time
 */
        void updateTriggerRatioVariation(std::vector<ratioChange> fluctuations);

/** Update on the fly the current visit/trigger ratio and delete the old vector of fluctuation. */
        void setTriggerRatio(double ratio);

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
        void kill();
    };
}}

#endif //SIMGRID_ELASTIC_TASK_H
