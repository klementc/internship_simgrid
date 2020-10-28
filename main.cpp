#include <xbt/sysdep.h>
#include "simgrid/s4u.hpp"
#include "simgrid/msg.h"

#include "ElasticTask.hpp"
#include "ElasticPolicy.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void return1(void){
  XBT_DEBUG("DONE FROM CALLBACK");
}

void returns2(void){
  XBT_DEBUG("returns2 sending to s2");
  s4u_Mailbox* m = s4u_Mailbox::by_name("s2");
  int v = 0;
  m->put(&v, 20000);
}

void returns3(void){
  XBT_DEBUG("returns3 sending to s3");
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3");
  int v=0;
  m->put(&v, 20000);
}

void returns4(void){
  XBT_DEBUG("s4 received message from S3, FINISH");
}

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm, int n) {
  XBT_INFO("Starting");
  simgrid::s4u::Actor::create("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm] { etm->run(); });
  // provision the policy with a list of usable hosts
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol = new simgrid::s4u::ElasticPolicyCPUThreshold(3,0.7,0.1);
  for(int i = 1; i < 200; i++) {
    cpuPol->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
  }
  cpuPol->addElasticTaskManager(etm.get());
  simgrid::s4u::Actor::create("POLICY", simgrid::s4u::Host::by_name("cb1-1"), [cpuPol] { cpuPol->run(); });

  // etm will have one host at start
  etm->addHost(simgrid::s4u::Host::by_name("cb1-1"));
  etm->setOutputFunction(return1);

  simgrid::s4u::this_actor::sleep_for(5);
  XBT_INFO("puishing to mailbox"  );
  s4u_Mailbox* mb = s4u_Mailbox::by_name("coucou");

  std::ifstream file;
  file.open("timestampsExpo2.txt");
  double a;
  while (file >> a)
  {
    simgrid::s4u::this_actor::sleep_until(a);
    int n = 50;
    mb->put(&n, n);
  }


  XBT_INFO("after mailbox");
  simgrid::s4u::this_actor::sleep_for(100);
  cpuPol->kill();
  etm->kill();

  XBT_INFO("Done.");

}


void test2()
{

  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("coucou");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm3 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s3");
  etm1->setOutputFunction(returns2);
  etm2->setOutputFunction(returns3);
  etm3->setOutputFunction(returns4);
  simgrid::s4u::Actor::create("ET1", simgrid::s4u::Host::by_name("cb1-1"), [etm1] { etm1->run(); });
  simgrid::s4u::Actor::create("ET2", simgrid::s4u::Host::by_name("cb1-2"), [etm2] { etm2->run(); });
  simgrid::s4u::Actor::create("ET3", simgrid::s4u::Host::by_name("cb1-3"), [etm3] { etm3->run(); });

  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol1 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol2 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  simgrid::s4u::ElasticPolicyCPUThreshold* cpuPol3 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.7,0.1);
  for(int i = 1; i < 100; i++) {
    cpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
    cpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(100+i)));
    cpuPol3->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(200+i)));
  }
  cpuPol1->addElasticTaskManager(etm1.get());
  cpuPol2->addElasticTaskManager(etm2.get());
  cpuPol3->addElasticTaskManager(etm3.get());

  simgrid::s4u::Actor::create("POLICY1", simgrid::s4u::Host::by_name("cb1-1"), [cpuPol1] { cpuPol1->run(); });
  simgrid::s4u::Actor::create("POLICY2", simgrid::s4u::Host::by_name("cb1-100"), [cpuPol2] { cpuPol2->run(); });
  simgrid::s4u::Actor::create("POLICY3", simgrid::s4u::Host::by_name("cb1-200"), [cpuPol3] { cpuPol3->run(); });

  etm1->addHost(simgrid::s4u::Host::by_name("cb1-1"));
  etm2->addHost(simgrid::s4u::Host::by_name("cb1-100"));
  etm3->addHost(simgrid::s4u::Host::by_name("cb1-200"));

  etm1->setProcessRatio(1e8);
  etm2->setProcessRatio(1e9);
  etm3->setProcessRatio(2e7);

  s4u_Mailbox* mb = s4u_Mailbox::by_name("coucou");
  std::ifstream file;
  file.open("default1TimeStamps.csv");
  double a;
  while (file >> a)
  {
    simgrid::s4u::this_actor::sleep_until(a);
    int n = 50;
    mb->put(&n, n);
  }

  XBT_INFO("Done.");

  simgrid::s4u::this_actor::sleep_for(100);
  cpuPol1->kill();
  cpuPol2->kill();
  cpuPol3->kill();
  etm1->kill();
  etm2->kill();
  etm3->kill();
}

int main(int argc, char **argv) {
  int argcE = 1;
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>("coucou");
  e->load_platform("dejavu_platform.xml");
  //simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [etm, argv] { eve(etm, std::stoi(argv[1])); });
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [etm, argv] { test2(); });
  e->run();
  return 0;
}
