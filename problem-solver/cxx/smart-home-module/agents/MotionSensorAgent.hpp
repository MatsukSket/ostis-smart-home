#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/keynodes.hpp"

namespace smart_home
{

class MotionSensorAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  void SwitchSensorState(
      ScAddr const & sensorAddr,
      ScAddr const & targetState,
      ScAddr const & oppositeState);
};

}
