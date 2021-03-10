#include <simgrid/s4u/Actor.hpp>
#include <simgrid/s4u/Host.hpp>
#include <simgrid/s4u/Mailbox.hpp>
#include <simgrid/s4u/Engine.hpp>
#include "ElasticPolicy.hpp"
#include "ElasticTask.hpp"
#include <memory>

XBT_LOG_NEW_DEFAULT_CATEGORY(run_log, "logs of the experiment");

void returnservice1(TaskDescription* td) {
	XBT_DEBUG("Return function of service service1");
	s4u_Mailbox* mservice2 = s4u_Mailbox::by_name("service2");
	//mservice2->put(td, td->dSize);
}

void run() {
XBT_INFO("Starting run()");
std::vector<std::string> vservice1 = std::vector<std::string>();
vservice1.push_back("service1");
std::shared_ptr<simgrid::s4u::ElasticTaskManager> etmservice1 = std::make_shared<simgrid::s4u::ElasticTaskManager>("etmservice1",vservice1);
simgrid::s4u::Actor::create("etmservice1_a", simgrid::s4u::Host::by_name("cb1-1"), [etmservice1] { etmservice1->run(); });
etmservice1->addHost(simgrid::s4u::Host::by_name("cb1-1"));
etmservice1->setProcessRatio(1e8);
etmservice1->setOutputFunction(returnservice1);
etmservice1->setBootDuration(2);
etmservice1->setDataSizeRatio(1);

	simgrid::s4u::ElasticPolicyCPUThreshold* polcpuPol1 = new simgrid::s4u::ElasticPolicyCPUThreshold(10,0.2,0.95);
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-1"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-2"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-3"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-4"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-5"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-6"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-7"));
polcpuPol1->addHost(simgrid::s4u::Host::by_name("cb1-8"));
polcpuPol1->addElasticTaskManager(etmservice1.get());
simgrid::s4u::Actor::create("POLICY1", simgrid::s4u::Host::by_name("cb1-1"), [polcpuPol1] {polcpuPol1->run(); });
s4u_Mailbox* mb = s4u_Mailbox::by_name("service1");
boost::uuids::random_generator generator;
std::ifstream file;
double a;
//file.open("default1TimeStamps.csv");
file.open("default4TimeStamps.csv");
std::string s; file>>s;
while (file >> a) {
if(a > simgrid::s4u::Engine::get_clock())
	simgrid::s4u::this_actor::sleep_until(a);
TaskDescription* t = new TaskDescription(generator(), -1, 0);
t->dSize=1;
mb->put(t, t->dSize);
}
// kill policies and ETMs
XBT_INFO("Done. Killing policies and etms");
simgrid::s4u::this_actor::sleep_for(600);polcpuPol1->kill();
etmservice1->kill();

}
int main(int argc, char* argv[]) {
	simgrid::s4u::Engine* e = new simgrid::s4u::Engine(&argc, argv);
	e->load_platform(argv[1]);
	simgrid::s4u::Actor::create("main", simgrid::s4u::Host::by_name("cb1-1"), [&]{run();});
	e->run();
	return 0;
}
