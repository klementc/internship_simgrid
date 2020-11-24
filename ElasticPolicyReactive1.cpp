#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <simgrid/s4u/Actor.hpp>

#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"

using namespace simgrid;
using namespace s4u;

XBT_LOG_NEW_DEFAULT_CATEGORY(elasticPolicyReactive1, "Elastic tasks policy manager");

ElasticPolicyReactive1::ElasticPolicyReactive1(double interval, double k, double r, double mAvg)
  : ElasticPolicy(interval), k_(k), r_(r), mAvg_(mAvg)
{

}

void ElasticPolicyReactive1::run()
{
  int instanceToStartIndex = 0;

  XBT_INFO("ElasticPolicyreactive1 activated");
  while(isActive()){
    // wait until next scaling
    simgrid::s4u::this_actor::sleep_for(getUpdateInterval());
    // make your decision (here only one group should be in the taskgroup)
    xbt_assert(getTasks().size() == 1, "Reactive1 policy operates on single groups");
    ElasticTaskManager* etm = getTasks().at(0);

    // Update R(t), A(t), B(t), and E(t) from monitoring data
    // TODO separate A from B (not sure its correct right now)
    double B = etm->getAmountOfWaitingRequests();
    double E = etm->getAmountOfExecutingRequests();

    double L = E + (B/r_);
    double R = etm->getInstanceAmount()*mAvg_*getUpdateInterval();

    // TODO compute mAvg

    double D = L - R;

    double nReactive = D/(mAvg_*getUpdateInterval());

    std::vector<double> lv = etm->getCPULoads();
    double avgLoad = std::accumulate( lv.begin(), lv.end(), 0.0) / lv.size();
    XBT_INFO("%f %d %d %d %d %f stats", avgLoad, etm->getInstanceAmount(), etm->getAmountOfWaitingRequests(),
      etm->getAmountOfExecutingRequests(), etm->getCounterExecSlot(), etm->reqPerSec()*getUpdateInterval());
    etm->resetCounterExecSlot();

    if(nReactive > 0 && D > k_){
      int newI = std::min(nReactive, (double)(hostPool_.size()-etm->getInstanceAmount()));
      for(int i=0;i<newI;i++){

        etm->addHost(hostPool_.at(instanceToStartIndex));
        instanceToStartIndex = (instanceToStartIndex+1) % hostPool_.size();
      }
    }else if(nReactive < -2){
      for(int i=0;i<-nReactive && etm->getInstanceAmount()>1;i++){
        etm->removeHost(0);
      }
    }

    //std::vector<double> lv = etm->getCPULoads();
    //double avgLoad = std::accumulate( lv.begin(), lv.end(), 0.0) / lv.size();
    //XBT_INFO("%f %d %d %d stats", avgLoad, etm->getInstanceAmount(), etm->getAmountOfWaitingRequests(), etm->getAmountOfExecutingRequests());

  }
}