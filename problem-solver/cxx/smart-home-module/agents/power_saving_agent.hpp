#pragma once

#include <vector>

#include <sc-memory/sc_agent.hpp>

class PowerSavingAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;

private:
  bool HasClass(ScAddr const & element, ScAddr const & classAddr) const;

  bool HasRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation) const;

  void AddClass(ScAddr const & element, ScAddr const & classAddr);

  void RemoveClass(ScAddr const & element, ScAddr const & classAddr);

  void SetRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation);

  ScAddr GetRelationTarget(ScAddr const & source, ScAddr const & relation) const;

  void ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target);

  ScAddr CreateEvent(
      ScAddr const & eventClass,
      ScAddr const & home,
      ScAddr const & device = ScAddr::Empty,
      ScAddr const & state = ScAddr::Empty);

  bool ShouldDisableDevice(ScAddr const & device) const;

  std::vector<ScAddr> CollectClassInstances(ScAddr const & classAddr) const;

  void EnablePowerSavingMode(ScAddr const & home, ScStructure & result);

  void DisablePowerSavingMode(ScAddr const & home, ScStructure & result);
};
