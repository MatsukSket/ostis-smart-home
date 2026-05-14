#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/keynodes.hpp"
#include <string>

namespace smart_home
{

struct CurrentDateTime {
  std::string time;
  std::string day;
};

class DeviceScheduleMonitorAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

private:
  CurrentDateTime GetCurrentDateTime() const;
  
  void ProcessAllEffectors(CurrentDateTime const & now);
  void ProcessDeviceSchedule(ScAddr const & deviceAddr, CurrentDateTime const & now);
  void ProcessScheduleTuple(ScAddr const & scheduleTuple, ScAddr const & deviceAddr, CurrentDateTime const & now);
  
  bool IsDayMatching(ScAddr const & scheduleTuple, std::string const & currentDay);
  void CheckAndApplyTimeAction(ScAddr const & scheduleTuple, ScAddr const & timeRelation, 
                               ScAddr const & targetState, ScAddr const & oppositeState, 
                               ScAddr const & deviceAddr, std::string const & currentTime);

  void SwitchDeviceState(ScAddr const & deviceAddr, ScAddr const & targetState, ScAddr const & oppositeState);

  bool IsDeviceHardOff(ScAddr const & deviceAddr);
  ScAddr GetDeviceScheduleSet(ScAddr const & deviceAddr);
  void ProcessScheduleSet(ScAddr const & scheduleSet, ScAddr const & deviceAddr, CurrentDateTime const & now);
};

}