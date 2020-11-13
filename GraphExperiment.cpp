#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>

#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"


XBT_LOG_NEW_DEFAULT_CATEGORY(Graph_log, "logs for the Graph experiment");

void return2a(TaskDescription* a){
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3");
  m->put(a, a->dSize);
}

void return2b(TaskDescription* a){
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3");
  m->put(a, a->dSize);
}

void return1(TaskDescription* a){
  s4u_Mailbox* m1 = s4u_Mailbox::by_name("s2A");
  s4u_Mailbox* m2 = s4u_Mailbox::by_name("s2B");

  m1->put(a, a->dSize);
  m2->put(a, a->dSize);
}

void returnf(TaskDescription* a){
  XBT_DEBUG("request served");
  delete a;
}

void run()
{
  XBT_INFO("start graph experiment");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s1");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2a = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2A");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2b = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2B");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm3 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s3");
  etm1->setOutputFunction(return1);
  etm2a->setOutputFunction(return2a);
  etm2b->setOutputFunction(return2b);
  etm3->setOutputFunction(returnf);
  simgrid::s4u::Actor::create("ET1", simgrid::s4u::Host::by_name("cb1-1"), [etm1] { etm1->run(); });
  simgrid::s4u::Actor::create("ET2A", simgrid::s4u::Host::by_name("cb1-2"), [etm2a] { etm2a->run(); });
  simgrid::s4u::Actor::create("ET2B", simgrid::s4u::Host::by_name("cb1-3"), [etm2b] { etm2b->run(); });
  simgrid::s4u::Actor::create("ET3", simgrid::s4u::Host::by_name("cb1-4"), [etm3] { etm3->run(); });

  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol1 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol2a = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol2b = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol3 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);

  for(int i = 2; i < 100; i++) {
    cpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
    cpuPol2a->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(100+i)));
    cpuPol2b->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(200+i)));
    cpuPol3->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(300+i)));
  }
  cpuPol1->addElasticTaskManager(etm1.get());
  cpuPol2a->addElasticTaskManager(etm2a.get());
  cpuPol2b->addElasticTaskManager(etm2b.get());
  cpuPol3->addElasticTaskManager(etm3.get());

  simgrid::s4u::Actor::create("POLICY1", simgrid::s4u::Host::by_name("cb1-1"), [cpuPol1] { cpuPol1->run(); });
  simgrid::s4u::Actor::create("POLICY2a", simgrid::s4u::Host::by_name("cb1-100"), [cpuPol2a] { cpuPol2a->run(); });
  simgrid::s4u::Actor::create("POLICY2b", simgrid::s4u::Host::by_name("cb1-200"), [cpuPol2b] { cpuPol2b->run(); });
  simgrid::s4u::Actor::create("POLICY3", simgrid::s4u::Host::by_name("cb1-300"), [cpuPol3] { cpuPol3->run(); });

  etm1->addHost(simgrid::s4u::Host::by_name("cb1-1"));
  etm2a->addHost(simgrid::s4u::Host::by_name("cb1-100"));
  etm2b->addHost(simgrid::s4u::Host::by_name("cb1-200"));
  etm3->addHost(simgrid::s4u::Host::by_name("cb1-300"));

  etm1->setProcessRatio(1e8);
  etm2a->setProcessRatio(1e9);
  etm2b->setProcessRatio(2e9);
  etm3->setProcessRatio(5e7);

  // interval between adding a new instance and using it
  etm1->setBootDuration(1);
  etm2a->setBootDuration(1);
  etm2b->setBootDuration(1);
  etm3->setBootDuration(1);

  s4u_Mailbox* mb = s4u_Mailbox::by_name("s1");
  boost::uuids::random_generator generator;
  std::ifstream file;
  file.open("default1TimeStamps.csv");
  double a;
  while (file >> a)
  {
    simgrid::s4u::this_actor::sleep_until(a);
    TaskDescription* t = new TaskDescription(generator(), -1, 0);
    t->dSize=5000;
    mb->put(t, t->dSize);
  }

  XBT_INFO("Done.");

  simgrid::s4u::this_actor::sleep_for(100);
  cpuPol1->kill();
  cpuPol2a->kill();
  cpuPol2b->kill();
  cpuPol3->kill();
  etm1->kill();
  etm2a->kill();
  etm2b->kill();
  etm3->kill();
}

int main(int argc, char **argv) {
  int argcE = 1;
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  e->load_platform("dejavu_platform.xml");
  //simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [etm, argv] { eve(etm, 0.5); });
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [argv] { run(); });
  e->run();
  return 0;
}