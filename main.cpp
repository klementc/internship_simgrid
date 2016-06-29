#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm, double loadIncrease) {
  XBT_INFO("Starting");
  simgrid::s4u::ElasticTask *e1 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("Tremblay"), 1.0, 5.0,
      etm.get());
  simgrid::s4u::ElasticTask *e2 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("Tremblay"), 3.0, 2.5,
      etm.get());
  e1->setOutputFunction([e2]() {
      e2->triggerOneTime(1.5);
  });
  simgrid::s4u::this_actor::sleep(15);
  etm->kill();
  XBT_INFO("Done.");
}

int main(int argc, char **argv) {
  //MSG_init(&argc, argv);
  int argcE = 1;
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argcE, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>();
  e->loadPlatform("dejavu_platform.xml");
  simgrid::s4u::Actor("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm] { etm->run(); });
  simgrid::s4u::Actor("main", simgrid::s4u::Host::by_name("cb1-1"), [etm, argv] { eve(etm, std::stod(argv[1])); });
  e->run();
  return 0;
}
