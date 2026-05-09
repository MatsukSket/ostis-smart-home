#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/keynodes.hpp"

namespace smart_home
{

class LampMotionControlAgent : public ScElementaryEventAgent
{
public:
  ScAddr GetEventSubscriptionElement() const override;
  ScEventType GetEventType() const override;
  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;

private:
  void SwitchDeviceState(
      ScAddr const & deviceAddr,
      ScAddr const & targetState,
      ScAddr const & oppositeState);
};

class LampMotionControlAgentOff : public ScElementaryEventAgent
{
public:
  ScAddr GetEventSubscriptionElement() const override;
  ScEventType GetEventType() const override;
  ScResult DoProgram(ScElementaryEvent const & event, ScAction & action) override;

private:
  void SwitchDeviceState(
      ScAddr const & deviceAddr,
      ScAddr const & targetState,
      ScAddr const & oppositeState);
};

}
