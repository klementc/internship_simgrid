#ifndef MICROSERVICE_HPP
#define MICROSERVICE_HPP

#include <string>
#include <vector>

#include <xbt/base.h>
#include <xbt/signal.hpp>
#include <xbt/Extendable.hpp>
#include <simgrid/s4u/Actor.hpp>

namespace simgrid {
namespace s4u {

class Microservice {
  private:
    std::string _name;
    /**
     * Mailbox used to received data (provided by the servicemanager)
     **/
    std::string _mailbox_name;


    /**
     * Ratios used to compute the amount of output communications
     * and computations depending on the input data size
     * should maybe set this to a function later to match different
     * complexities and not only linear (especially for compute)
     */
    double _inputOutputRatio;
    double _inputFlopRatio;

    /**
     * Mailbox(es) for output data
     */
    std::vector<std::string> outputMailboxes;

    /**
     * If the service is currently running?
     */
    bool _active;

    /**
     *  Time interval between activation and execution of the service
     */
    double _boot_duration;

  public:
    Microservice(std::string name, std::string mailboxName,
                double inoutr, double inpflpr)
      :_name(name), _mailbox_name(mailboxName), _inputOutputRatio(inoutr),
      _inputFlopRatio(inpflpr), _active(false) {}

    void stopService();
    void startService();

    void setInputOutputRatio(double ratio);
    void setInputFlopRatio(double ratio);

    inline double getInputOutputRatio() {return _inputOutputRatio;}
    inline double getInputFlopRatio()   {return _inputFlopRatio;}

};

} /* namespace s4u */
} /* namespace simgrid */

#endif /* MICROSERVICE_HPP */