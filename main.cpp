#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(s4u_test, "a sample log category");

simgrid::s4u::ElasticTaskManager *etm;

class Eve {
public:
    void operator()() {
        XBT_INFO("Creating first ET");
        simgrid::s4u::ElasticTask *e1 = new simgrid::s4u::ElasticTask(simgrid::s4u::Host::by_name("Tremblay"), 2.0, 2.0,
                etm);  // Doesn't work because using etm before it's initialized ?
        etm->execute();
        sleep(60);
        XBT_INFO("Done.");
    }
};

int main(int argc, char **argv) {
    simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
    e->loadPlatform("../simgrid/examples/platforms/two_hosts.xml");
    etm = new simgrid::s4u::ElasticTaskManager("etm", simgrid::s4u::Host::by_name("Jupiter"), Eve());
    e->run();
    return 0;
}
