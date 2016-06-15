#ifndef ELASTICTASK_HPP
#define ELASTICTASK_HPP

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

ratioChange rC(double time, double visitsPerSec);

namespace simgrid {
namespace s4u {

/** @brief Elastic task.
 */
XBT_PUBLIC_CLASS ElasticTaskManager : public Actor {
//explicit ElasticTaskManager(smx_process_t smx_proc);
private:
    std::vector<ratioChange> ratioFluctuations;
    double currentRatio;
    long currentIndex;
    double flopsTask;
    double bytesTask;
    void *dataTask;
    std::vector<ElasticTaskManager> outputStreams;
public:
    ElasticTaskManager(const char *name, s4u::Host *host, std::function<void()> code, double _flopsTask,
            double _bytesTask, void *_dataTask);  // no killTime
    template<class C>
    ElasticTaskManager(const char *name, s4u::Host *host, C code, double _flopsTask, double _bytesTask, void *_dataTask)
        : ElasticTaskManager(name, host, std::function<void()>(std::move(code)), _flopsTask, _bytesTask, _dataTask) { }

    ~ElasticTaskManager();
//        };

//    namespace this_task {
/** Set the variation of visits/triggers per seconds over the time. */
    void setTriggerRatioVariation(std::vector<ratioChange> fluctuations);
//void setTriggerTrace(string fileName);
/** Add a link like in a workflow of tasks. */
    void addOutputStream(ElasticTaskManager e2);

/** Modify the normal task that is triggered.
*
* You can use it to reduce the computational size for example.
*/
    void modifyTask(double _flopsTask, double _bytesTask, void *_dataTask);

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

//XBT_PUBLIC_CLASS ElasticTask {
//private:
//    ElasticTaskManager etm;
//public:
//    ElasticTask(s4u::Host *host, double _flopsTask);

//    void execute();
//};
}}

#endif //ELASTICTASK_HPP
