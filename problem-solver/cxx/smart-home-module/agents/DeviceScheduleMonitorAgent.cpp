#include "DeviceScheduleMonitorAgent.hpp"
#include <sc-memory/sc_iterator.hpp>
#include <sc-memory/sc_link.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace smart_home;

SC_AGENT_IMPLEMENTATION(DeviceScheduleMonitorAgent)
{
  std::time_t t = std::time(nullptr);
  std::tm* now = std::localtime(&t);
  
  std::ostringstream timeStream;
  timeStream << now->tm_hour << ":" << std::setw(2) << std::setfill('0') << now->tm_min;
  std::string currentTime = timeStream.str();

  const char* days[] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};
  std::string currentDay = days[now->tm_wday];

  SC_LOG_INFO("SmartHome Monitor Tick. Time: " << currentTime << ", Day: " << currentDay);

  ScIterator3Ptr effectorIt = m_ctx->CreateIterator3(
      Keynodes::concept_effector,
      ScType::EdgeAccessConstPosPerm,
      ScType::NodeConst);

  while (effectorIt->Next())
  {
    ScAddr deviceAddr = effectorIt->Get(2);

    ScIterator5Ptr scheduleIt = m_ctx->CreateIterator5(
        deviceAddr,
        ScType::EdgeDCommonConst,
        ScType::NodeConst,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_schedule);

    if (scheduleIt->Next())
    {
      ScAddr scheduleNode = scheduleIt->Get(2);

      ScIterator5Ptr dayIt = m_ctx->CreateIterator5(
          scheduleNode,
          ScType::EdgeAccessConstPosPerm,
          ScType::LinkConst,
          ScType::EdgeAccessConstPosPerm,
          Keynodes::rrel_day);

      bool dayMatches = true;
      if (dayIt->Next()) 
      {
        std::string scheduledDay = GetTimeFromLink(m_ctx.get(), dayIt->Get(2));
        if (scheduledDay != currentDay) {
            dayMatches = false;
        }
      }

      if (!dayMatches) continue;

      ScIterator5Ptr onTimeIt = m_ctx->CreateIterator5(
          scheduleNode,
          ScType::EdgeAccessConstPosPerm,
          ScType::LinkConst,
          ScType::EdgeAccessConstPosPerm,
          Keynodes::rrel_on_time);

      if (onTimeIt->Next())
      {
        std::string onTime = GetTimeFromLink(m_ctx.get(), onTimeIt->Get(2));
        if (onTime == currentTime)
        {
          SwitchDeviceState(m_ctx.get(), deviceAddr, Keynodes::concept_state_on, Keynodes::concept_state_off);
        }
      }

      ScIterator5Ptr offTimeIt = m_ctx->CreateIterator5(
          scheduleNode,
          ScType::EdgeAccessConstPosPerm,
          ScType::LinkConst,
          ScType::EdgeAccessConstPosPerm,
          Keynodes::rrel_off_time);

      if (offTimeIt->Next())
      {
        std::string offTime = GetTimeFromLink(m_ctx.get(), offTimeIt->Get(2));
        if (offTime == currentTime)
        {
          SwitchDeviceState(m_ctx.get(), deviceAddr, Keynodes::concept_state_off, Keynodes::concept_state_on);
        }
      }
    }
  }

  return SC_RESULT_OK;
}


std::string DeviceScheduleMonitorAgent::GetTimeFromLink(ScMemoryContext * ctx, ScAddr linkAddr)
{
  std::string content;
  ScStreamPtr stream = ctx->GetLinkContent(linkAddr);
  if (stream) {
    ScStreamConverter::StreamToString(stream, content);
  }
  return content;
}

void DeviceScheduleMonitorAgent::SwitchDeviceState(ScMemoryContext * ctx, ScAddr deviceAddr, ScAddr targetState, ScAddr oppositeState)
{
     ScIterator3Ptr checkStateIt = ctx->CreateIterator3(
      targetState,
      ScType::EdgeAccessConstPosPerm,
      deviceAddr);
      
  if (checkStateIt->Next()) {
      return; 
  }

  SC_LOG_INFO("Switching device state...");

  ScIterator3Ptr oldStateIt = ctx->CreateIterator3(
      oppositeState,
      ScType::EdgeAccessConstPosPerm,
      deviceAddr);
      
  while (oldStateIt->Next()) {
      ctx->EraseElement(oldStateIt->Get(1));
  }

  ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, targetState, deviceAddr);
}