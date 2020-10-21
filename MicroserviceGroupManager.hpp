#ifndef MICROSERVICE_GROUP_MANAGER_HPP
#define MICROSERVICE_GROUP_MANAGER_HPP

#include "Microservice.hpp"
#include "ServiceMetrics.hpp"
#include <vector>


namespace simgrid {
namespace s4u {

class MicroserviceGroupManager {
  private:
    /**
     * keep a list of active instances
     */
    std::vector<Microservice> _activeInstances;

    /**
     * name of the service
     */
    std::string _serviceName;


  public:
    MicroserviceGroupManager(std::string name)
      : _serviceName(name) {}

    /**
     * Add or remove 'amount' instances of the microservice
     * return: the amount of active instances after scaling
     **/
    int horizontal_scale(int amount);

    int add_instance(double inoutr, double inpflpr);

    /**
     * Return metrics required by the app manager to scale
     */
    struct ServiceMetrics getMetricsFromInstances();

    void run();

};

} /*namespace s4u*/
} /*namespace simgrid*/

#endif /* MICROSERVICE_GROUP_MANAGER_HPP */