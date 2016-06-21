#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

int main(int argc, char **argv) {
    simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
    e->loadPlatform("../../platforms/two_hosts.xml");
    simgrid::s4u::ElasticTaskManager *etm =
        new simgrid::s4u::ElasticTaskManager("etm", simgrid::s4u::Host::by_name("Jupiter"));
    e->run();
    return 0;
}
