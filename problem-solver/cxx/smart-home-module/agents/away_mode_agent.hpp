#pragma once

#include <sc-memory/sc_agent.hpp>

class AwayModeAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;

private:
  bool HasClass(ScAddr const & element, ScAddr const & classAddr) const;

  void AddClass(ScAddr const & element, ScAddr const & classAddr);

  void RemoveClass(ScAddr const & element, ScAddr const & classAddr);

  void SetRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation);

  ScAddr CreateHomeEvent(ScAddr const & eventClass, ScAddr const & home, ScAddr const & sourceEvent = ScAddr::Empty);

  bool RunPowerSavingAgent(ScAddr const & home, ScAddr const & trigger);

  ScResult ActivateAwayMode(ScAction & action, ScAddr const & home, ScAddr const & sourceEvent);

  ScResult ProcessPresenceEvent(ScAction & action, ScAddr const & home, ScAddr const & sourceEvent);
};
