#include "DeviceScheduleMonitorAgent.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace smart_home;

ScAddr DeviceScheduleMonitorAgent::GetActionClass() const
{
  return Keynodes::action_check_schedule;
}

ScResult DeviceScheduleMonitorAgent::DoProgram(ScAction & action)
{
  CurrentDateTime now = GetCurrentDateTime();
  SC_LOG_INFO("Tick. Time: " + now.time + ", Day: " + now.day);

  ProcessAllEffectors(now);

  return action.FinishSuccessfully();
}


// getting time
CurrentDateTime DeviceScheduleMonitorAgent::GetCurrentDateTime() const
{
  std::time_t t = std::time(nullptr);
  std::tm * now = std::localtime(&t);

  std::ostringstream timeStream;
  timeStream << std::setw(2) << std::setfill('0') << now->tm_hour 
             << ":" << std::setw(2) << std::setfill('0') << now->tm_min;

  const char * days[] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};
  
  return {timeStream.str(), days[now->tm_wday]};
}


// device searching
void DeviceScheduleMonitorAgent::ProcessAllEffectors(CurrentDateTime const & now)
{
  ScIterator5Ptr subclassIt = m_context.CreateIterator5(
      Keynodes::concept_effector, ScType::ConstCommonArc, ScType::ConstNode,
      ScType::ConstPermPosArc, Keynodes::nrel_inclusion);

  bool foundAnySubclass = false;
  while (subclassIt->Next())
  {
    foundAnySubclass = true;
    ScAddr subclass = subclassIt->Get(2);

    ScIterator3Ptr effectorIt = m_context.CreateIterator3(
        subclass, ScType::ConstPermPosArc, ScType::ConstNode);

    while (effectorIt->Next())
    {
      ProcessDeviceSchedule(effectorIt->Get(2), now);
    }
  }

  if (!foundAnySubclass)
    SC_LOG_INFO("No effector subclasses found.");
}

// schedule searcching
void DeviceScheduleMonitorAgent::ProcessDeviceSchedule(ScAddr const & deviceAddr, CurrentDateTime const & now)
{
  ScIterator5Ptr scheduleSetIt = m_context.CreateIterator5(
      deviceAddr, ScType::ConstCommonArc, ScType::ConstNode,
      ScType::ConstPermPosArc, Keynodes::nrel_schedule);

  if (!scheduleSetIt->Next()) 
    return; 

  ScAddr scheduleSet = scheduleSetIt->Get(2);
  ScIterator3Ptr scheduleIt = m_context.CreateIterator3(
      scheduleSet, ScType::ConstPermPosArc, ScType::ConstNode);

  while (scheduleIt->Next())
  {
    ProcessScheduleTuple(scheduleIt->Get(2), deviceAddr, now);
  }
}


// schedule processing
void DeviceScheduleMonitorAgent::ProcessScheduleTuple(ScAddr const & scheduleTuple, ScAddr const & deviceAddr, CurrentDateTime const & now)
{
  if (!IsDayMatching(scheduleTuple, now.day))
    return;

  CheckAndApplyTimeAction(scheduleTuple, Keynodes::rrel_on_time, 
                          Keynodes::concept_state_on, Keynodes::concept_state_off, 
                          deviceAddr, now.time);

  CheckAndApplyTimeAction(scheduleTuple, Keynodes::rrel_off_time, 
                          Keynodes::concept_state_off, Keynodes::concept_state_on, 
                          deviceAddr, now.time);
}

bool DeviceScheduleMonitorAgent::IsDayMatching(ScAddr const & scheduleTuple, std::string const & currentDay)
{
  ScIterator5Ptr dayIt = m_context.CreateIterator5(
      scheduleTuple, ScType::ConstPermPosArc, ScType::ConstNodeLink,
      ScType::ConstPermPosArc, Keynodes::rrel_day);

  bool hasConstraint = false;
  while (dayIt->Next())
  {
    hasConstraint = true;
    if (GetLinkContent(dayIt->Get(2)) == currentDay)
      return true;
  }
  
  return !hasConstraint; 
}

void DeviceScheduleMonitorAgent::CheckAndApplyTimeAction(
    ScAddr const & scheduleTuple, ScAddr const & timeRelation, 
    ScAddr const & targetState, ScAddr const & oppositeState, 
    ScAddr const & deviceAddr, std::string const & currentTime)
{
  ScIterator5Ptr timeIt = m_context.CreateIterator5(
      scheduleTuple, ScType::ConstPermPosArc, ScType::ConstNodeLink,
      ScType::ConstPermPosArc, timeRelation);

  if (timeIt->Next())
  {
    std::string scheduledTime = GetLinkContent(timeIt->Get(2));
    if (scheduledTime == currentTime)
    {
      SC_LOG_INFO("Time matched: " + scheduledTime + ". Switching state.");
      SwitchDeviceState(deviceAddr, targetState, oppositeState);
    }
  }
}



std::string DeviceScheduleMonitorAgent::GetLinkContent(ScAddr const & linkAddr)
{
  std::string content;
  m_context.GetLinkContent(linkAddr, content);
  return content;
}

void DeviceScheduleMonitorAgent::SwitchDeviceState(
    ScAddr const & deviceAddr,
    ScAddr const & targetState,
    ScAddr const & oppositeState)
{
  ScIterator3Ptr checkIt = m_context.CreateIterator3(
      targetState, ScType::ConstPermPosArc, deviceAddr);

  if (checkIt->Next())
    return;

  ScIterator3Ptr oldStateIt = m_context.CreateIterator3(
      oppositeState, ScType::ConstPermPosArc, deviceAddr);

  while (oldStateIt->Next())
    m_context.EraseElement(oldStateIt->Get(1));

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, deviceAddr);
}