#pragma once

#include <sc-memory/sc_agent.hpp>

class HumidityControlAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;

private:
  ScAddr GetRelationTarget(ScAddr const & source, ScAddr const & relation) const;

  bool HasClass(ScAddr const & element, ScAddr const & classAddr) const;

  ScAddr GetTargetState(ScAddr const & device, ScAddr const & humidityValue) const;

  void ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target);

  bool RunDeviceStateAgent(ScAddr const & device, ScAddr const & targetState);
};
