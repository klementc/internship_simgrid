#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>
#include <simgrid/s4u/Comm.hpp>
#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"

XBT_LOG_NEW_DEFAULT_CATEGORY(run_log, "logs of the experiment");

void returnservice1(TaskDescription* td) {
	XBT_DEBUG("Return function of service service1");
	s4u_Mailbox* mservice2 = s4u_Mailbox::by_name("service2");
	mservice2->put(td, td->dSize);
}
void returnservice2(TaskDescription* td) {
	XBT_DEBUG("Return function of service service2");
#ifdef USE_JAEGERTRACING
XBT_DEBUG("CLOSE SPANS");
	for (auto it = td->parentSpans.rbegin(); it != td->parentSpans.rend(); ++it)
  {
    auto t2 = std::chrono::seconds(946684800)+std::chrono::milliseconds(int(simgrid::s4u::Engine::get_instance()->get_clock()*1000));
    (*it)->get()->Log({{"end",t2.count()}});
    (*it)->get()->Finish({opentracing::v3::FinishTimestamp(t2)});

  }
#endif /*USE_JAEGERTRACING*/
	//s4u_Mailbox* mservice3 = s4u_Mailbox::by_name("service3");
	//mservice3->put(td, td->dSize);
}
void run() {
XBT_INFO("Starting run()");
std::vector<std::string> vservice1 = std::vector<std::string>();
vservice1.push_back("service1");
std::shared_ptr<simgrid::s4u::ElasticTaskManager> etmservice1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("etmservice1",vservice1);

etmservice1->setOutputFunction(returnservice1);
simgrid::s4u::Actor::create("etmservice1_a", simgrid::s4u::Host::by_name("cb1-1"), [etmservice1] { etmservice1->run(); });
etmservice1->addHost(simgrid::s4u::Host::by_name("cb1-2"));
etmservice1->setProcessRatio(1e7);
etmservice1->setBootDuration(2);
etmservice1->setDataSizeRatio(1);

std::vector<std::string> vservice2 = std::vector<std::string>();
vservice2.push_back("service2");
std::shared_ptr<simgrid::s4u::ElasticTaskManager> etmservice2 = std::make_shared<simgrid::s4u::ElasticTaskManager>("etmservice2",vservice2);
etmservice2->setOutputFunction(returnservice2);
simgrid::s4u::Actor::create("etmservice2_a", simgrid::s4u::Host::by_name("cb1-100"), [etmservice2] { etmservice2->run(); });
etmservice2->addHost(simgrid::s4u::Host::by_name("cb1-100"));
etmservice2->setProcessRatio(1e7);
etmservice2->setBootDuration(2);
etmservice2->setDataSizeRatio(1);

	simgrid::s4u::ElasticPolicyCPUThreshold* polcpuPol1 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.2,0.95);
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-1"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-2"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-3"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-4"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-5"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-6"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-7"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-8"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-9"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-10"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-11"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-12"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-13"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-14"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-15"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-16"));

polcpuPol1->addElasticTaskManager(etmservice1.get());
simgrid::s4u::Actor::create("polcpuPol1_a", simgrid::s4u::Host::by_name("cb1-1"), [polcpuPol1] {polcpuPol1->run(); });
	simgrid::s4u::ElasticPolicyCPUThreshold* polcpuPol2 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.2,0.95);
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-101"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-102"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-103"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-104"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-105"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-106"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-107"));
polcpuPol2->addHost(simgrid::s4u::Host::by_name("cb1-108"));

polcpuPol2->addElasticTaskManager(etmservice2.get());
simgrid::s4u::Actor::create("polcpuPol2_a", simgrid::s4u::Host::by_name("cb1-100"), [polcpuPol2] {polcpuPol2->run(); });
s4u_Mailbox* mb = s4u_Mailbox::by_name("service1");
boost::uuids::random_generator generator;
std::ifstream file;
double a;
file.open("default1TimeStamps.csv");
std::string s; file>>s;
std::vector<simgrid::s4u::CommPtr> pending_comms;
while (file >> a) {
if(a > simgrid::s4u::Engine::get_clock())
	simgrid::s4u::this_actor::sleep_until(a);
TaskDescription* t = new TaskDescription(generator(), -1, 0);
//t->dSize=1.25e8;
t->dSize = 1e3;
simgrid::s4u::CommPtr comm = mb->put_async(t, t->dSize);
pending_comms.push_back(comm);
}
// kill policies and ETMs
simgrid::s4u::this_actor::sleep_for(30);
XBT_INFO("Done. Killing policies and etms");
polcpuPol1->kill();
polcpuPol2->kill();
etmservice1->kill();
etmservice2->kill();



}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);
	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-200"), [&]{run();});
	e->run();
	return 0;
}
