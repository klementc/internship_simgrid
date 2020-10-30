#ifndef ELASTICTASK_HPP
#define ELASTICTASK_HPP

#include <vector>
#include <queue>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

#include <xbt/base.h>
#include <xbt/string.hpp>
#include <xbt/signal.hpp>
#include <xbt/Extendable.hpp>
#include "simgrid/msg.h"
#include <simgrid/simix.h>
#include <simgrid/s4u/Actor.hpp>

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

class BootInstance : public EvntQ {
  public:
    simgrid::s4u::Host* host_;
    BootInstance(simgrid::s4u::Host* host, double date):EvntQ(date), host_(host){}
};

class RatioChange : public EvntQ {
  public:
    size_t id;
    double visitsPerSec;

    RatioChange(size_t id_, double date_, double visitsPerSec_)
      : EvntQ(date_), id(id_), visitsPerSec(visitsPerSec_) {}
    RatioChange(size_t id_, double visitsPerSec_)  // for the event queue as the date is already known
      : EvntQ(0.0), id(id_), visitsPerSec(visitsPerSec_) {}
    RatioChange(double date_, double visitsPerSec_)  // for the user that doesn't know the ID of the ET
      : EvntQ(date_), visitsPerSec(visitsPerSec_) {}
};

class TaskDescription : public EvntQ {
  public:
    size_t id;
    double flops;
    double interSpawnDelay;
    bool repeat = true;
    std::function<void()> outputFunction = []() {};
    bool hasTimestamps = false;
    std::ifstream *ts_file;
    double dSize;
    double startTime;

    TaskDescription(double flops_, double interSpawnDelay_, double date_, double dSize_)
        : EvntQ(date_), flops(flops_), interSpawnDelay(interSpawnDelay_), dSize(dSize_) {
      ts_file = new std::ifstream;
    }

    TaskDescription(double flops_, double interSpawnDelay_, double date_)
        : TaskDescription(flops_, interSpawnDelay_, date_, -1) {
    }
    TaskDescription(double flops_, double interSpawnDelay_)
      : TaskDescription(flops_, interSpawnDelay_, 0.0) {}
};

namespace simgrid {
namespace s4u {

/** @brief */
class ElasticTaskManager {
  private:
    std::function<void(std::map<std::string,double>*)> outputFunction = [](std::map<std::string,double>*) {};
    std::vector<simgrid::s4u::Host*> availableHostsList_;
    std::string rcvMailbox_;
    std::vector<TaskDescription> tasks;
    std::priority_queue<EvntQ*, std::vector<EvntQ*>, Comparator> nextEvtQueue;
    simgrid::s4u::SemaphorePtr sleep_sem;
    bool keepGoing;
    int nextHost_;
    int64_t processRatio_;
    int64_t waitingReqAmount_;
    double bootDuration_;

  public:
    ElasticTaskManager(std::string name);

    size_t addElasticTask(double flopsTask, double interSpawnDelay);
    size_t addElasticTask(double flopsTask, double interSpawnDelay, double s);
    void pollnet();
    void addRatioChange(size_t id, double date, double visitsPerSec);
    void addHost(Host *host);
    void removeHost(int i);
    void changeRatio(size_t id, double visitsPerSec);
    void changeTask(size_t id, double flops);
    void simpleChangeTask(size_t id);
    void removeTask(size_t id);
    void removeRatioChanges(size_t id);
    void setProcessRatio(int64_t pr);
    void setBootDuration(double bd);
    int64_t getAmountOfWaitingRequests();

    void triggerOneTimeTask(size_t id);
    void triggerOneTimeTask(size_t id, double ratioLoad);
    void setOutputFunction(std::function<void(std::map<std::string,double>*)> code);
    void setTimestampsFile(size_t id, std::string filename);

    void kill();
    void run();


    // basic metrics
    std::vector<double> getCPULoads();
    unsigned int getInstanceAmount();
};

class ElasticTask {
  private:
    size_t id;
    ElasticTaskManager *etm;
  public:
    ElasticTask(double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_);
    ElasticTask(double flopsTask, ElasticTaskManager *etm_);
    ElasticTask(double flopsTask, std::vector<RatioChange> fluctuations, ElasticTaskManager *etm_);
    ~ElasticTask();

    void setTriggerRatioVariation(std::vector<RatioChange> fluctuations);
    void setRatioVariation(double interSpawnDelay);
    void modifyTask(double flops);
    void triggerOneTime();
    void triggerOneTime(double ratioLoad);
    void setTimestampsFile(std::string filename);
    inline size_t getId(){ return id; }
};

}}

#endif //ELASTICTASK_HPP
