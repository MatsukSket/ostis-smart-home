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
  std::time_t t = std::time(nullptr);
  std::tm * now = std::localtime(&t);

  std::ostringstream timeStream;
  timeStream << std::setw(2) << std::setfill('0') << now->tm_hour 
             << ":" << std::setw(2) << std::setfill('0') << now->tm_min;
  std::string currentTime = timeStream.str();

  const char * days[] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};
  std::string currentDay = days[now->tm_wday];

  SC_LOG_INFO("Tick. Time: " + currentTime + ", Day: " + currentDay);

  // Итерируемся по подклассам concept_effector
  ScIterator5Ptr subclassIt = m_context.CreateIterator5(
      Keynodes::concept_effector,
      ScType::ConstCommonArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      Keynodes::nrel_inclusion);

  bool foundAnySubclass = false;
  while (subclassIt->Next())
  {
    foundAnySubclass = true;
    ScAddr subclass = subclassIt->Get(2);
    SC_LOG_INFO("Found effector subclass.");

    ScIterator3Ptr effectorIt = m_context.CreateIterator3(
        subclass,
        ScType::ConstPermPosArc,
        ScType::ConstNode);

    bool foundAnyDevice = false;
    while (effectorIt->Next())
    {
      foundAnyDevice = true;
      ScAddr deviceAddr = effectorIt->Get(2);
      SC_LOG_INFO("Found device, checking schedule.");

      ScIterator5Ptr scheduleSetIt = m_context.CreateIterator5(
          deviceAddr,
          ScType::ConstCommonArc,
          ScType::ConstNode,
          ScType::ConstPermPosArc,
          Keynodes::nrel_schedule);

      if (!scheduleSetIt->Next())
      {
        SC_LOG_INFO("No schedule for device, skipping.");
        continue;
      }

      ScAddr scheduleSet = scheduleSetIt->Get(2);

      ScIterator3Ptr scheduleIt = m_context.CreateIterator3(
          scheduleSet,
          ScType::ConstPermPosArc,
          ScType::ConstNode);

      while (scheduleIt->Next())
      {
        ScAddr scheduleTuple = scheduleIt->Get(2);

        bool hasDayConstraint = false;
        bool dayMatches = false;

        ScIterator5Ptr dayIt = m_context.CreateIterator5(
            scheduleTuple,
            ScType::ConstPermPosArc,
            ScType::ConstNodeLink,
            ScType::ConstPermPosArc,
            Keynodes::rrel_day);

        while (dayIt->Next())
        {
          hasDayConstraint = true;
          std::string scheduledDay = GetLinkContent(dayIt->Get(2));
          SC_LOG_INFO("Schedule day: " + scheduledDay + ", current: " + currentDay);
          if (scheduledDay == currentDay)
          {
            dayMatches = true;
            break;
          }
        }

        if (hasDayConstraint && !dayMatches)
          continue;

        ScIterator5Ptr onTimeIt = m_context.CreateIterator5(
            scheduleTuple,
            ScType::ConstPermPosArc,
            ScType::ConstNodeLink,
            ScType::ConstPermPosArc,
            Keynodes::rrel_on_time);

        if (onTimeIt->Next())
        {
          std::string onTime = GetLinkContent(onTimeIt->Get(2));
          SC_LOG_INFO("On time: " + onTime + ", current: " + currentTime);
          if (onTime == currentTime)
          {
            SC_LOG_INFO("Turning ON device.");
            SwitchDeviceState(deviceAddr, Keynodes::concept_state_on, Keynodes::concept_state_off);
          }
        }

        ScIterator5Ptr offTimeIt = m_context.CreateIterator5(
            scheduleTuple,
            ScType::ConstPermPosArc,
            ScType::ConstNodeLink,
            ScType::ConstPermPosArc,
            Keynodes::rrel_off_time);

        if (offTimeIt->Next())
        {
          std::string offTime = GetLinkContent(offTimeIt->Get(2));
          SC_LOG_INFO("Off time: " + offTime + ", current: " + currentTime);
          if (offTime == currentTime)
          {
            SC_LOG_INFO("Turning OFF device.");
            SwitchDeviceState(deviceAddr, Keynodes::concept_state_off, Keynodes::concept_state_on);
          }
        }
      }
    }

    if (!foundAnyDevice)
      SC_LOG_INFO("No devices found in subclass.");
  }

  if (!foundAnySubclass)
    SC_LOG_INFO("No effector subclasses found.");

  return action.FinishSuccessfully();
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
      targetState,
      ScType::ConstPermPosArc,
      deviceAddr);

  if (checkIt->Next())
    return;

  ScIterator3Ptr oldStateIt = m_context.CreateIterator3(
      oppositeState,
      ScType::ConstPermPosArc,
      deviceAddr);

  while (oldStateIt->Next())
    m_context.EraseElement(oldStateIt->Get(1));

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, deviceAddr);
}