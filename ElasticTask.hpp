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
    boost::uuids::uuid id;
    double visitsPerSec;

    RatioChange(boost::uuids::uuid id_, double date_, double visitsPerSec_)
      : EvntQ(date_), id(id_), visitsPerSec(visitsPerSec_) {}
    RatioChange(boost::uuids::uuid id_, double visitsPerSec_)  // for the event queue as the date is already known
      : EvntQ(0.0), id(id_), visitsPerSec(visitsPerSec_) {}
    RatioChange(double date_, double visitsPerSec_)  // for the user that doesn't know the ID of the ET
      : EvntQ(date_), visitsPerSec(visitsPerSec_) {}
};

class TaskDescription : public EvntQ {
  public:
    boost::uuids::uuid id;
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

class TaskInstance;

/** @brief */
class ElasticTaskManager {
  private:
    std::function<void(std::map<std::string,double>*)> outputFunction = [](std::map<std::string,double>*) {};
    std::vector<simgrid::s4u::Host*> availableHostsList_;
    std::string rcvMailbox_;
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

  public:
    ElasticTaskManager(std::string name);

    boost::uuids::uuid addElasticTask(double flopsTask, double interSpawnDelay);
    boost::uuids::uuid addElasticTask(double flopsTask, double interSpawnDelay, double s);
    void pollnet();
    void addRatioChange(boost::uuids::uuid id, double date, double visitsPerSec);
    void addHost(Host *host);
    void removeHost(int i);
    void changeRatio(boost::uuids::uuid id, double visitsPerSec);
    void removeTaskExecs(boost::uuids::uuid id);
    void removeTask(boost::uuids::uuid id);
    void removeRatioChanges(boost::uuids::uuid id);
    void setProcessRatio(int64_t pr);
    void setBootDuration(double bd);
    int64_t getAmountOfWaitingRequests();
    int64_t getAmountOfExecutingRequests();
    void modifExecutingReqAmount(int n);
    void modifWaitingReqAmount(int n);

    void triggerOneTimeTask(boost::uuids::uuid id);
    void triggerOneTimeTask(boost::uuids::uuid id, double ratioLoad);
    void setOutputFunction(std::function<void(std::map<std::string,double>*)> code);
    void setTimestampsFile(boost::uuids::uuid id, std::string filename);

    void kill();
    void run();


    // basic metrics
    std::vector<double> getCPULoads();
    unsigned int getInstanceAmount();
};


class TaskInstance {
  private:
    bool keepGoing_;
    std::function<void(std::map<std::string,double>*)> outputFunction_ = [](std::map<std::string,double>*) {};
    ElasticTaskManager* etm_;
    std::string mbName_;

    simgrid::s4u::SemaphorePtr n_empty_;
    simgrid::s4u::SemaphorePtr n_full_;
    double bootTime_;

    int maxReqInInst_;
    std::vector<std::map<std::string,double>*> reqs;

    void pollTasks();
  public:
    TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(std::map<std::string,double>*)> outputFunction,
      int maxReqInst, double bootTime);
    TaskInstance(ElasticTaskManager* etm, std::string mbName,
      std::function<void(std::map<std::string,double>*)> outputFunction);


    void run();
    void kill();
};

class ElasticTask {
  private:
    boost::uuids::uuid id;
    ElasticTaskManager *etm;
  public:
    ElasticTask(double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_);
    ElasticTask(double flopsTask, ElasticTaskManager *etm_);
    ElasticTask(double flopsTask, std::vector<RatioChange> fluctuations, ElasticTaskManager *etm_);
    ~ElasticTask();

    void setTriggerRatioVariation(std::vector<RatioChange> fluctuations);
    void setRatioVariation(double interSpawnDelay);
    void triggerOneTime();
    void triggerOneTime(double ratioLoad);
    void setTimestampsFile(std::string filename);
    inline boost::uuids::uuid getId(){ return id; }
};

}}

#endif //ELASTICTASK_HPP
