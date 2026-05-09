#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/keynodes.hpp"

namespace smart_home
{

class LampMotionControlAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  void SwitchDeviceState(
      ScAddr const & deviceAddr,
      ScAddr const & targetState,
      ScAddr const & oppositeState);
};

}
