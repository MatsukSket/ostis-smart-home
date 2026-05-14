#pragma once

#include <sc-memory/sc_agent.hpp>

class DeviceStateAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;

private:
  void ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target);

  ScAddr CreateDeviceStateEvent(ScAddr const & device, ScAddr const & targetState);
};
