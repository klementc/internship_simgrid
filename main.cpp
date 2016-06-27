#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm) {
  XBT_INFO("Creating first ET");
  simgrid::s4u::ElasticTask *e1 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("Tremblay"), 0.0, 10.0,
      etm.get());
  sleep(15);
  etm->kill();
  XBT_INFO("Done.");
  // TODO, kill the etm
}

int main(int argc, char **argv) {
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>();
  e->loadPlatform("../simgrid/examples/platforms/two_hosts.xml");
  simgrid::s4u::Actor("ETM", simgrid::s4u::Host::by_name("Jupiter"), [etm] { etm->run(); });
  simgrid::s4u::Actor("main", simgrid::s4u::Host::by_name("Tremblay"), [etm] { eve(etm); });
  e->run();
  return 0;
}



// REMINDER
//class ElasticSearchManager {
//  // stuff
//public:
//  void run();
//};

//std::shared_ptr<ElasticSearchManager> etm = std::make_shared<ElasticSearchManager>();
//Actor etmActor("etm", host, [etm] {
//                 etm->run();
//                 });
//Actor anotherActor("other", host, [etm] { doSomething(etm); } )
