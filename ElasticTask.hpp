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
#include <simgrid/datatypes.h>
#include <simgrid/s4u/forward.hpp>

#include <simgrid/s4u/actor.hpp>

class EvntQ {
  public:
    double date;

    EvntQ(double date_) : date(date_) {}
    virtual ~EvntQ() {}
};

typedef struct streamET {
  size_t destET;
  double ratioLoad;

  //streamET() = default;
  streamET(size_t destET_, double ratioLoad_)
    : destET(destET_), ratioLoad(ratioLoad_) {}
} streamET;

class RatioChange : public EvntQ {
  public:
    size_t id;
    double visitsPerSec;

    //RatioChange() = default;
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
    size_t nextHost;
    std::vector<simgrid::s4u::Host*> hosts;  // TODO, use hosts
    bool repeat = true;
    std::function<void()> outputFunction = []() {};
    bool hasTimestamps = false;
    std::ifstream *myfile;

    //TaskDescription() = default;
    TaskDescription(double flops_, double interSpawnDelay_, simgrid::s4u::Host *host_, double date_)
        : EvntQ(date_), flops(flops_), interSpawnDelay(interSpawnDelay_) {
      hosts.push_back(host_);
      nextHost = 0;
      myfile = new std::ifstream;
    }
    TaskDescription(double flops_, double interSpawnDelay_, simgrid::s4u::Host *host_)
      : TaskDescription(flops_, interSpawnDelay_, host_, 0.0) {}
    //~TaskDescription() {
    //  delete myfile;
    //}
};

bool operator<(const EvntQ& lhs, const EvntQ& rhs);

namespace simgrid {
namespace s4u {

/** @brief */
XBT_PUBLIC_CLASS ElasticTaskManager {
  private:
    std::vector<TaskDescription> tasks;
    std::priority_queue<EvntQ*, std::vector<EvntQ*>, std::less<EvntQ*> > nextEvtQueue;
    msg_sem_t sleep_sem;
    bool keepGoing;
  public:
    ElasticTaskManager();
    //~ElasticTaskManager();

    size_t addElasticTask(s4u::Host *host, double flopsTask, double interSpawnDelay);

    void addRatioChange(size_t id, double date, double visitsPerSec);
    void addHost(size_t id, Host *host);
    void changeRatio(size_t id, double visitsPerSec);
    void changeTask(size_t id, double flops);
    void simpleChangeTask(size_t id);
    void removeTask(size_t id);
    void removeRatioChanges(size_t id);

    void triggerOneTimeTask(size_t id);
    void triggerOneTimeTask(size_t id, double ratioLoad);

    void setOutputFunction(size_t id, std::function<void()> code);
    void setTimestampsFile(size_t id, std::string filename);

    void kill();
    void run();
};

XBT_PUBLIC_CLASS ElasticTask {
  private:
    size_t id;
    ElasticTaskManager *etm;
  public:
    ElasticTask(Host *host, double flopsTask, double interSpawnDelay, ElasticTaskManager *etm_);
    ElasticTask(Host *host, double flopsTask, ElasticTaskManager *etm_);
    ElasticTask(Host *host, double flopsTask, std::vector<RatioChange> fluctuations, ElasticTaskManager *etm_);
    ~ElasticTask();

    void setTriggerRatioVariation(std::vector<RatioChange> fluctuations);
    void setRatioVariation(double interSpawnDelay);
    void modifyTask(double flops);
    void triggerOneTime();
    void triggerOneTime(double ratioLoad);
    void setOutputFunction(std::function<void()> code);
    void addHost(Host *host);
    void setTimestampsFile(std::string filename);
};

}}

#endif //ELASTICTASK_HPP
