#include <xbt/sysdep.h>
#include "simgrid/s4u.h"
#include "ElasticTask.hpp"
#include "simgrid/msg.h"

class Eve {
public:
    void operator()() {
//        ratioChange ratioFluctuation2[2] = {rC(0.0, 5), rC(40.0, 4)};
//        std::vector <ratioChange> ratioFluctuation(2);
//        ratioFluctuation.assign(&ratioFluctuation2[0], &ratioFluctuation2[0] + 2);
//        simgrid::s4u::this_task::setTriggerRatioVariation(ratioFluctuation);
        sleep(60);
//        simgrid::s4u::this_task::kill();
    }
};

int main(int argc, char **argv) {
    simgrid::s4u::Engine *e = new simgrid::s4u::Engine(&argc, argv);
    e->loadPlatform("../simgrid/examples/platforms/two_hosts.xml");
    msg_task_t task = MSG_task_create("Task", 2.0, 0.0, NULL);
    simgrid::s4u::ElasticTaskManager *eT = new simgrid::s4u::ElasticTaskManager("eve", simgrid::s4u::Host::by_name("Tremblay"),
                                                                 Eve(), task);
    ratioChange ratioFluctuation2[3] = {rC(0.0, 5), rC(40.0, 4), rC(50, 0)};
    std::vector <ratioChange> ratioFluctuation(3);
    ratioFluctuation.assign(&ratioFluctuation2[0], &ratioFluctuation2[0] + 3*(sizeof(ratioChange)));
    eT->setTriggerRatioVariation(ratioFluctuation);
    e->run();
    return 0;
}
