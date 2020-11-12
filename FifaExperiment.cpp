#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>

#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"


XBT_LOG_NEW_DEFAULT_CATEGORY(FIFA_log, "logs for the FIFA experiment");

void returnf(TaskDescription* a){
  XBT_DEBUG("request served");
  delete a;
}

void runFIFA()
{
  XBT_INFO("starting FIFA experiment");
  XBT_INFO("Create ETM on cb1-1, Policy on cb1-1 and attach machines cb1-2 - ... - cb1-299 to policy");
  std::shared_ptr<simgrid::s4u::ElasticTaskManager> etm = std::make_shared<simgrid::s4u::ElasticTaskManager>("coucou");
  etm->setOutputFunction(returnf);

  simgrid::s4u::Actor::create("ETM", simgrid::s4u::Host::by_name("cb1-1"), [etm]{etm->run();});

  //simgrid::s4u::ElasticPolicyCPUThreshold* elasticPol = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.95,0.5);
  simgrid::s4u::ElasticPolicyReactive1* elasticPol = new simgrid::s4u::ElasticPolicyReactive1(120,5,120,120);
  for(int i=2 ; i<299 ; i++) {
    elasticPol->addHost(simgrid::s4u::Host::by_name("cb1-"+std::to_string(i)));
  }
  elasticPol->addElasticTaskManager(etm.get());

  simgrid::s4u::Actor::create("POLICY", simgrid::s4u::Host::by_name("cb1-1"), [elasticPol] { elasticPol->run(); });

  etm->addHost(simgrid::s4u::Host::by_name("cb1-2"));
  //etm->setProcessRatio(5e8);
  etm->setProcessRatio(1e7);
  etm->setBootDuration(60);


  XBT_INFO("starting to send messages");
  boost::uuids::random_generator generator;
  s4u_Mailbox* mb = s4u_Mailbox::by_name("coucou");
  std::ifstream file;
  file.open("analysis/FIFAtrace/t.csv");
  // remove first line (a string)
  std::string s;
  file>>s;
  double a;
  while (file >> a)
  {
    if(a > simgrid::s4u::Engine::get_clock())
      simgrid::s4u::this_actor::sleep_until(a);

    TaskDescription* t = new TaskDescription(generator(), -1, 0);
    t->dSize=1;
    mb->put(t, t->dSize);
    //std::map<std::string,double>* n = new std::map<std::string,double>();
    //n->insert(std::pair<std::string,double>("size",1));
    //mb->put(n, n->at("size"));
  }
  simgrid::s4u::this_actor::sleep_for(600);
  elasticPol->kill();
  etm->kill();
  XBT_INFO("Done.");

}


int main(int argc, char* argv[])
{
  simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);
  e->load_platform("dejavu_platform.xml");
  simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-1"), [&]{runFIFA();});

  e->run();
  return 0;
}