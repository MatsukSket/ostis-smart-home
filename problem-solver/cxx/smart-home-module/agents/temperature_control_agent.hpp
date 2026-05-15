#pragma once

#include <vector>

#include <sc-memory/sc_agent.hpp>

class TemperatureControlAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;

private:
  ScAddr GetRelationTarget(ScAddr const & source, ScAddr const & relation) const;

  std::vector<ScAddr> GetRelationTargets(ScAddr const & source, ScAddr const & relation) const;

  bool HasClass(ScAddr const & element, ScAddr const & classAddr) const;

  void ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target);

  bool RunDeviceStateAgent(ScAddr const & device, ScAddr const & targetState);
};
