#include <xbt/sysdep.h>
#include "simgrid/s4u.hpp"
#include "simgrid/msg.h"

#include "ElasticTask.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm, int n) {
  XBT_INFO("Starting");

  simgrid::s4u::ElasticTask *e3 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-2"), 1000000.0, n,
      etm.get());
  e3->setOutputFunction([e3](){XBT_INFO("done");});
  for(int i = 3; i < 10; i++) {
    e3->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
  }

  etm->changeRatio(e3->getId(), 0.5);


  simgrid::s4u::ElasticTask *e4 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-4"), 100000.0, n,
      etm.get());
  //e3->setTimestampsFile("ts.txt");
  e4->setOutputFunction([e4](){XBT_INFO("done");});
  e4->triggerOneTime(1500);
  simgrid::s4u::this_actor::sleep_for(100);
  etm->kill();
  XBT_INFO("Done.");
}

int main(int argc, char **argv) {
  int argcE = 1;
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>();
  e->load_platform("dejavu_platform.xml");
  simgrid::s4u::Actor::create("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm] { etm->run(); });
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-1"), [etm, argv] { eve(etm, std::stoi(argv[1])); });
  e->run();
  return 0;
}
