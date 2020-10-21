#include "simgrid/s4u.hpp"

#include "Microservice.hpp"

using namespace simgrid;
using namespace s4u;

XBT_LOG_NEW_DEFAULT_CATEGORY(microservice, "microservice logs");

void Microservice::startService()
{
  _active = true;
  while(_active) {
    sleep(10);
    XBT_INFO("Hello");
  }
}

void Microservice::stopService()
{
  _active = false;
}

void Microservice::setInputOutputRatio(double ratio)
{
  if(ratio <= 0)
    _inputOutputRatio = 0;
  else
    _inputOutputRatio = ratio;
}

void Microservice::setInputFlopRatio(double ratio)
{
  if(ratio <= 0)
    _inputFlopRatio = 0;
  else
    _inputFlopRatio = ratio;
}
