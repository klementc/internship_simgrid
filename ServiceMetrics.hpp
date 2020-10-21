#ifndef SERVICE_METRICS_HPP
#define SERVICE_METRICS_HPP

#include <vector>

#include <xbt/base.h>
#include <xbt/signal.hpp>
#include <xbt/Extendable.hpp>


struct ServiceMetrics {

  double globalCPULoad;
  double networkUsage;

  /*add new metrics*/

};


#endif /* SERVICE_METRICS_HPP */