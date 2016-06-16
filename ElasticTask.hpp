#ifndef ELASTICTASK_HPP
#define ELASTICTASK_HPP

#include <vector>
#include <queue>
#include <string>

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
    size_t id;
    double date;
    double visitsPerSec;
} ratioChange;

ratioChange rC(size_t id, double visitsPerSec);

typedef struct taskDescription {
    size_t id;
    double flops;
    double interSpawnDelay;
    simgrid::s4u::Host *host;
} taskDescription;

typedef struct evntQ {
    double date;
    enum { ratioChange_type, taskDescription_type } eventEnum;
    union {
        ratioChange params_rC;
        taskDescription params_tD;
    } instruction;
} evntQ;

bool operator<(const evntQ& lhs, const evntQ& rhs);

namespace simgrid {
namespace s4u {

XBT_PUBLIC_CLASS ElasticTaskManager : public Actor {
private:
    std::vector<taskDescription> tasks;
    std::priority_queue<evntQ, std::vector<evntQ>, std::less<evntQ> > nextEvtQueue;
public:
    ElasticTaskManager(const char *name, s4u::Host *host, std::function<void()> code);
    template<class C>
    ElasticTaskManager(const char *name, s4u::Host *host, C code) :
        ElasticTaskManager(name, host, std::function<void()>(std::move(code))) {}
    ~ElasticTaskManager();

    size_t addElasticTask(s4u::Host *host, double flopsTask, double interSpawnDelay);

    void addRatioChange(size_t id, double flops, double visitsPerSec);
    void changeRatio(size_t id, double visitsPerSec);
    void removeTask(size_t id);
    void removeRatioChanges(size_t id);

    void execute();
};

XBT_PUBLIC_CLASS ElasticTask {
private:
    size_t id;
    ElasticTaskManager *etm;
public:
    ElasticTask(s4u::Host *host, double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_);
    ElasticTask(Host *host, double flopsTask, ElasticTaskManager *etm_);
    ElasticTask(Host *host, double flopsTask, std::vector<ratioChange> fluctuations, ElasticTaskManager *etm_);
    ~ElasticTask();

    void setTriggerRatioVariation(std::vector<ratioChange> fluctuations);
    void setRatioVariation(double interSpawnDelay);
};

}}

#endif //ELASTICTASK_HPP
