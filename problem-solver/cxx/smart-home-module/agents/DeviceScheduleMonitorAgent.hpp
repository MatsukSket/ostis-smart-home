#pragma once

#include <sc-memory/sc_agent.hpp>
#include "keynodes/Keynodes.hpp"

#include "DeviceScheduleMonitorAgent.generated.hpp"

namespace smart_home
{
class DeviceScheduleMonitorAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::action_check_schedule, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  std::string GetTimeFromLink(ScMemoryContext * ctx, ScAddr linkAddr);
  
  void SwitchDeviceState(ScMemoryContext * ctx, ScAddr deviceAddr, ScAddr newState);
};
}