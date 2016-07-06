#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm, int n) {
  XBT_INFO("Starting");
  simgrid::s4u::ElasticTask *ets[n];
  for(int i = 0; i < n; i++) {
    ets[i] = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i+1)), 5.0, 1.0,
                                           etm.get());
  }
  //simgrid::s4u::ElasticTask *e2 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-3"), 5.0, 0.0,
  //    etm.get());
  //e1->setOutputFunction([e2]() {
  //    e2->triggerOneTime(1.5);
  //});
  //simgrid::s4u::ElasticTask *e3 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-4"), 5.0, 0.0,
  //    etm.get());
  //for(int i = 5; i < 2000; i++) {
  //  e3->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
  //}
  //e3->setTimestampsFile("d20_timestamp_wc.txt");
  simgrid::s4u::this_actor::sleep(100);
  etm->kill();
  XBT_INFO("Done.");
}

int main(int argc, char **argv) {
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>();
  e->loadPlatform("dejavu_platform.xml");
  simgrid::s4u::Actor::createActor("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm] { etm->run(); });
  simgrid::s4u::Actor::createActor("main", simgrid::s4u::Host::by_name("cb1-1"), [etm] { eve(etm, 1); });
  e->run();
  return 0;
}
