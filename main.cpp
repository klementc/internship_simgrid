#include <xbt/sysdep.h>
#include "simgrid/s4u.hpp"
#include "simgrid/msg.h"

#include "ElasticTask.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

void return1(void){
  XBT_INFO("DONE FROM CALLBACK");
}

void returns2(void){
  XBT_INFO("returns2 sending to s2");
  s4u_Mailbox* m = s4u_Mailbox::by_name("s2");
  int v = 0;
  m->put(&v, 20000);
}

void returns3(void){
  XBT_INFO("returns3 sending to s3");
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3");
  int v=0;
  m->put(&v, 20000);
}

void returns4(void){
  XBT_INFO("s4 received message from S3, FINISH");
}

void eve(std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm, int n) {
  XBT_INFO("Starting");

  //simgrid::s4u::ElasticTask *e3 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("cb1-2"), 1000000.0, n,
  //    etm.get());
  //e3->setOutputFunction([e3](){XBT_INFO("done");});
  /*for(int i = 1; i < 200; i++) {
    etm->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
  }

  //etm->changeRatio(e3->getId(), 0.5);


  //simgrid::s4u::ElasticTask *e4 = new simgrid::s4u::ElasticTask(100000.0, n, etm.get());
  //e3->setTimestampsFile("ts.txt");
  etm->setOutputFunction(return1);
  //e4->triggerOneTime(1500);

  simgrid::s4u::this_actor::sleep_for(5);
  XBT_INFO("puishing to mailbox"  );
  s4u_Mailbox* mb = s4u_Mailbox::by_name("coucou");
  for(int i = 0;i<500000;i++){
    void* pl;
    mb->put(pl, 50);
  }

  XBT_INFO("after mailbox");
  simgrid::s4u::this_actor::sleep_for(100);
  etm->kill();
  */

  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("coucou");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm3 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s3");
  etm1->setOutputFunction(returns2);
  etm2->setOutputFunction(returns3);
  etm3->setOutputFunction(returns4);
  simgrid::s4u::Actor::create("ET1", simgrid::s4u::Host::by_name("cb1-1"), [etm1] { etm1->run(); });
  simgrid::s4u::Actor::create("ET2", simgrid::s4u::Host::by_name("cb1-2"), [etm2] { etm2->run(); });
  simgrid::s4u::Actor::create("ET3", simgrid::s4u::Host::by_name("cb1-3"), [etm3] { etm3->run(); });
  for(int i = 1; i < 50; i++){
    etm1->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(i)));
    etm2->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(50+i)));
    etm3->addHost(simgrid::s4u::Host::by_name("cb1-" + std::to_string(100+i)));
  }

  simgrid::s4u::this_actor::sleep_for(5);
  XBT_INFO("puishing to mailbox"  );
  s4u_Mailbox* mb = s4u_Mailbox::by_name("coucou");
  for(int i = 0;i<500000;i++){
    void* pl;
    mb->put(pl, 5000);
    simgrid::s4u::this_actor::sleep_for(0.5);
  }

  simgrid::s4u::this_actor::sleep_for(1000);
  etm1->kill();
  etm2->kill();
  etm3->kill();
  XBT_INFO("Done.");

}

int main(int argc, char **argv) {
  int argcE = 1;
  simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>("coucou");
  e->load_platform("dejavu_platform.xml");
  //simgrid::s4u::Actor::create("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm] { etm->run(); });
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [etm, argv] { eve(etm, std::stoi(argv[1])); });
  e->run();
  return 0;
}
