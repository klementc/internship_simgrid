
#include <vector>
#include <iostream>
#include <numeric>
#include <simgrid/s4u/Actor.hpp>

#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"

using namespace simgrid;
using namespace s4u;

XBT_LOG_NEW_DEFAULT_CATEGORY(elasticPolicyCPU, "Elastic tasks policy manager");

ElasticPolicyCPUThreshold::ElasticPolicyCPUThreshold(double interval, double uCPUT, double lCPUT)
  : ElasticPolicy(interval), upperCPUThresh_(uCPUT), lowCPUThresh_(lCPUT)
{}


void ElasticPolicyCPUThreshold::run()
{
  // TODO something realistic
  int instanceToStartIndex = 0;

  XBT_INFO("ElasticPolicyCPUThreshold activated");
  while(isActive()){
    // wait until next scaling
    simgrid::s4u::this_actor::sleep_for(getUpdateInterval());

    // make your decision (here only one group should be in the taskgroup)
    xbt_assert(getTasks().size() == 1, "CPU threshold policy operates on single groups");

    ElasticTaskManager* etm = getTasks().at(0);

    std::vector<double> lv = etm->getCPULoads();
    double avgLoad = std::accumulate( lv.begin(), lv.end(), 0.0) / lv.size();

    XBT_INFO("%f %d %d %d %d %f stats", avgLoad, etm->getInstanceAmount(), etm->getAmountOfWaitingRequests(),
      etm->getAmountOfExecutingRequests(), etm->getCounterExecSlot(), etm->reqPerSec()*getUpdateInterval());

    etm->resetCounterExecSlot();

    if(avgLoad > upperCPUThresh_){
      // if available hosts, add one
      //XBT_DEBUG("add host %d", instanceToStartIndex);
      //etm->addHost(hostPool_.at(instanceToStartIndex));
      //instanceToStartIndex = (instanceToStartIndex+1) % hostPool_.size();
      XBT_DEBUG("add host");
      if(hostPool_.size()>0) {
        etm->addHost(hostPool_.at(0));
        hostPool_.erase(hostPool_.begin());
      }else{
        XBT_INFO("no more hosts to add to service");
      }
    }else if(avgLoad < lowCPUThresh_ && etm->getInstanceAmount()>1){
      // if more than one instance, remove one
      Host* h = etm->removeHost(0);
      if(h)
        hostPool_.push_back(h);
    }
  }
}