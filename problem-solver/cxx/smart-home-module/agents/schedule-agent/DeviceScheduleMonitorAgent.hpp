#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/Keynodes.hpp"
#include <string>

namespace smart_home
{

const std::string LOG_COLOR_RESET  = "\033[0m";
const std::string LOG_COLOR_RED    = "\033[31m";
const std::string LOG_COLOR_GREEN  = "\033[32m";
const std::string LOG_COLOR_YELLOW = "\033[33m";
const std::string LOG_COLOR_BLUE   = "\033[34m";
const std::string LOG_COLOR_PURPLE = "\033[35m";
const std::string LOG_COLOR_CYAN   = "\033[36m";

const std::string LOG_BOLD_RED     = "\033[1;31m";
const std::string LOG_BOLD_GREEN   = "\033[1;32m";

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
                              ScAddr const & deviceAddr, std::string const & currentTime,
                              std::string const & logPrefix);

  void SwitchDeviceState(ScAddr const & deviceAddr, ScAddr const & targetState, ScAddr const & oppositeState);

  bool IsDeviceHardOff(ScAddr const & deviceAddr);
  ScAddr GetDeviceScheduleSet(ScAddr const & deviceAddr);
  void ProcessScheduleSet(ScAddr const & scheduleSet, ScAddr const & deviceAddr, CurrentDateTime const & now);
};

}