#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/Keynodes.hpp"

namespace smart_home
{

class LightBulbAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  void SwitchDeviceState(ScAddr const & device, ScAddr const & targetState, ScAddr const & oppositeState);
};

}
