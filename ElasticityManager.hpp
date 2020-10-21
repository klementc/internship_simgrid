#ifndef ELASTICITY_MANAGER_HPP
#define ELASTICITY_MANAGER_HPP

#include <vector>

#include <simgrid/s4u/Actor.hpp>

namespace simgrid {
namespace s4u {

class ElasticityManager
{
  private:
    std::vector<MicroServiceGroupManager&> serviceGroups;

  public:
    /**
     * Returns the scaling value (how many nodes to add/remove)
     * for each service group
     */
    std::vector<int> policy();

    int addServiceGroup(MicroserviceGroupManager& mgm);

    /**
     * will regularly start and launch the policy to determine elasticity
     * requirements given the different servicegroup metrics
     */
    void run();



};


} /* namespace s4u */
} /* namespace simgrid */

#endif /* ELASTICITY_MANAGER_HPP */