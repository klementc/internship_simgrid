#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>

#include <memory>

#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"
#include "DataSource.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(Graph_log, "logs for the Graph experiment");

void return2a(TaskDescription* a){
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3a");


    XBT_DEBUG("send %p to s3a", a);
  m->put(a, a->dSize);
}

void return2b(TaskDescription* a){
  s4u_Mailbox* m = s4u_Mailbox::by_name("s3b");


    XBT_DEBUG("send %p to s3B", a);
  m->put(a, a->dSize);
}

void return1(TaskDescription* a){
  s4u_Mailbox* m1 = s4u_Mailbox::by_name("s2A");
  s4u_Mailbox* m2 = s4u_Mailbox::by_name("s2B");


  XBT_DEBUG("send %p to s2a", a);
XBT_DEBUG("send %p to s2b", a);
  m1->put(a, a->dSize);
  m2->put(a, a->dSize);
}

void returnf(TaskDescription* a){
  XBT_DEBUG("request served %p", a);
  a->finished = true;


  /*auto sp = a->parentSpans.back();
  auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000));
  sp->get()->Finish({opentracing::v3::FinishTimestamp(t2)});*/
  for (auto it = a->parentSpans.rbegin(); it != a->parentSpans.rend(); ++it)
  {
    auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});
    (*it)->reset();
  }

  if(a!=NULL){
    delete a;
    a = NULL;
  }
}

void run()
{
  XBT_INFO("start graph experiment");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s1");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2a = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2A");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm2b = std::make_shared<simgrid::s4u::ElasticTaskManager>("s2B");
  std::vector<std::string> v = std::vector<std::string>();
  v.push_back("s3a");
  v.push_back("s3b");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm3 = std::make_shared<simgrid::s4u::ElasticTaskManager>("s3", v);
  etm1->setOutputFunction(return1);
  etm2a->setOutputFunction(return2a);
  etm2b->setOutputFunction(return2b);
  etm3->setOutputFunction(returnf);

  etm1->setParallelTasksPerInst(10);
  etm2a->setParallelTasksPerInst(10);
  etm2b->setParallelTasksPerInst(10);
  etm3->setParallelTasksPerInst(10);
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

  etm1->setProcessRatio(/*1e8*/1e6);
  etm2a->setProcessRatio(/*1e9*/1e7);
  etm2b->setProcessRatio(/*1e9*/1e7);
  etm3->setProcessRatio(/*5e7*/5e5);

  // interval between adding a new instance and using it
  etm1->setBootDuration(1);
  etm2a->setBootDuration(1);
  etm2b->setBootDuration(1);
  etm3->setBootDuration(1);


  DataSourceTSFile* ds = new DataSourceTSFile("s1", "default1TimeStamps.csv", 1000);
	simgrid::s4u::ActorPtr dataS = simgrid::s4u::Actor::create("snd", simgrid::s4u::Host::by_name("cb1-1"), [&]{ds->run();});

  XBT_INFO("Done.");

  simgrid::s4u::this_actor::sleep_for(10000);
  ds->suspend();
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
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-2"), [argv] { run(); });
  e->run();
  return 0;
}