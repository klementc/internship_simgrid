#ifndef ELASTICTASK_HPP
#define ELASTICTASK_HPP

#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <xbt/base.h>
#include <xbt/string.hpp>
#include <xbt/signal.hpp>
#include <xbt/Extendable.hpp>
#include "simgrid/msg.h"
#include <simgrid/simix.h>
#include <simgrid/s4u/Actor.hpp>

#include "ElasticConfig.hpp"

class EvntQ {
  public:
    double date;

    EvntQ(double date_) : date(date_) {}
    virtual ~EvntQ() {}
};

/**
 * Compare EvntQ's event timestamps
 */
struct Comparator {
  bool operator()(const EvntQ* lhs, const EvntQ* rhs) {
    return lhs->date > rhs->date;
  }
};

class TaskDescription : public EvntQ {
  public:
    boost::uuids::uuid id_;
    double flops;
    double interSpawnDelay;
    bool repeat = true;
    std::function<void()> outputFunction = []() {};
    bool hasTimestamps = false;
    double dSize;
    double startExec;
    double queueArrival;
    double instArrival;
    double endExec;
    // to enable multi routes (the important part will be in the output function, responible
    //for sending to output mailboxes depending ont the request type)
    std::string requestType;
    // if ack needed (ex: waiting for data from a DB before sending a request to the next service)
    // then in the output function: push a mb name on the stack, send requests to the services you want an ack for
    // those services in their return function will pop the mb from the stack and send an ack
    // then the original service, after receiving the ACK, can send a req to the next services
    // THUS, an important thing to note is that the routing part is dependent on the implenentation of the output services functions
    std::stack<std::string> ackStack;
    std::vector<double> flopsPerServ;

#ifdef USE_JAEGERTRACING
    std::vector<std::unique_ptr<opentracing::v3::Span>*> parentSpans;
#endif

    TaskDescription(boost::uuids::uuid id, double flops_, double interSpawnDelay_, double date_, double dSize_, std::string requestType_)
        : EvntQ(date_), id_(id), flops(flops_), interSpawnDelay(interSpawnDelay_), dSize(dSize_), requestType(requestType_) {
    }

    TaskDescription(boost::uuids::uuid id, double flops_, double interSpawnDelay_, double date_)
        : TaskDescription(id, flops_, interSpawnDelay_, date_, -1, "default") {
    }
    TaskDescription(boost::uuids::uuid id, double flops_, double interSpawnDelay_)
      : TaskDescription(id, flops_, interSpawnDelay_, 0.0) {}
};

namespace simgrid {
namespace s4u {

class TaskInstance;

/** @brief */
class ElasticTaskManager {
  private:
    std::function<void(TaskDescription*)> outputFunction = [](TaskDescription*) {};
    std::vector<simgrid::s4u::Host*> availableHostsList_;
    std::string serviceName_;
    std::vector<std::string> incMailboxes_;
    // used for requests to wait for all dependencies to be received within a graph node
    // (respect precedence constraints and don't execute a task before all it's data arrived)
    std::map<boost::uuids::uuid, std::vector<TaskDescription>> tempData;
    std::map<boost::uuids::uuid, TaskDescription> tasks;
    std::vector<TaskInstance*> tiList;
    std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> nextEvtQueue;
    simgrid::s4u::SemaphorePtr sleep_sem;
    bool keepGoing;
    int nextHost_;
    int64_t processRatio_;
    int64_t waitingReqAmount_;
    int64_t executingReqAmount_;
    double bootDuration_;
    simgrid::s4u::SemaphorePtr modif_sem_;
    boost::uuids::random_generator uuidGen_;
    double dataSizeRatio_;
    int counterExecSlot_;
    int parallelTasksPerInst_;
#ifdef USE_JAEGERTRACING
    std::shared_ptr<opentracing::v3::Tracer> tracer_;
#endif
    // for async send to taskInstances
    std::vector<simgrid::s4u::CommPtr> pending_comms;

  public:
    std::string parentName;
    ElasticTaskManager(std::string name, std::vector<std::string> incMailboxes);
    ElasticTaskManager(std::string name);

    boost::uuids::uuid addElasticTask(TaskDescription td);
    boost::uuids::uuid addElasticTask(boost::uuids::uuid id, double flopsTask, double interSpawnDelay);
    boost::uuids::uuid addElasticTask(boost::uuids::uuid id, double flopsTask, double interSpawnDelay, double s);
    void pollnet(std::string mbName);
    void addHost(Host *host);
    Host* removeHost(int i);
    void setProcessRatio(int64_t pr);
    void setBootDuration(double bd);
    int64_t getAmountOfWaitingRequests();
    int64_t getAmountOfExecutingRequests();
    void modifExecutingReqAmount(int n);
    void modifWaitingReqAmount(int n);
    void setDataSizeRatio(double r);
    double getDataSizeRatio();
    void setParallelTasksPerInst(int s);

    void trigger(TaskDescription* td);
    void trigger(TaskDescription* td, double ratioLoad);
    void setOutputFunction(std::function<void(TaskDescription*)> code);

    void kill();
    void run();

    // basic metrics
    std::vector<double> getCPULoads();
    unsigned int getInstanceAmount();
    inline int getCounterExecSlot(){return counterExecSlot_;}
    inline void setCounterExecSlot(int v){counterExecSlot_=v;}
    inline void resetCounterExecSlot(){counterExecSlot_=0;}
    inline std::string getServiceName(){return serviceName_;}
    double reqPerSec();

#ifdef USE_JAEGERTRACING
    /**
     * Obtain the tracer creater for jaeger interception
     * Only used if the jaegertracing option is ON
     */
    std::shared_ptr<opentracing::v3::Tracer> getTracer();
#endif
};


class TaskInstance {
  private:
    bool keepGoing_;
    std::function<void(TaskDescription*)> outputFunction_ = [](TaskDescription*) {};
    ElasticTaskManager* etm_;
    std::string mbName_;

    simgrid::s4u::SemaphorePtr n_empty_;
    simgrid::s4u::SemaphorePtr n_full_;
    simgrid::s4u::SemaphorePtr n_bl_;
    double bootTime_;
    boost::uuids::random_generator uuidGen_;

    int maxReqInInst_;
    std::vector<TaskDescription*> reqs;
    simgrid::s4u::Host* host_;
    std::vector<simgrid::s4u::CommPtr> commV;

    std::vector<simgrid::s4u::ExecPtr> pending_execs;
    std::map<simgrid::s4u::ExecPtr, TaskDescription*> execMap_;

    void pollTasks();
    void pollEndOfTasks();

  public:
    TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(TaskDescription*)> outputFunction,
      int maxReqInst, double bootTime);
    TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(TaskDescription*)> outputFunction,
      int maxReqInst);


    void run();
    void kill();
    inline simgrid::s4u::Host* getRunningHost(){return host_;};
};

}}

#endif //ELASTICTASK_HPP
