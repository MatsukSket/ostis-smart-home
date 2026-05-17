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
  ScAddr FindLamp(ScAddr const & sensor);
  bool IsMotionDetected(ScAddr const & sensor);
  void ForceOff(ScAddr const & lamp);
  void ApplyLampState(ScAddr const & lamp, bool motionDetected);
  void SetState(ScAddr const & device, ScAddr const & targetState, ScAddr const & oppositeState);
};

}
