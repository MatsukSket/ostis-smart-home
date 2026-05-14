#include "away_mode_agent.hpp"

#include <iostream>
#include <vector>

#include <sc-memory/sc_structure.hpp>

#include "keynodes/keynodes.hpp"

ScAddr AwayModeAgent::GetActionClass() const
{
  return SmartHomeKeynodes::action_control_away_mode;
}

ScResult AwayModeAgent::DoProgram(ScAction & action)
{
  auto const & [home, sourceEvent] = action.GetArguments<2>();

  if (!home.IsValid() || !sourceEvent.IsValid())
  {
    std::clog << "[AwayModeAgent] Invalid action arguments\n";
    return action.FinishWithError();
  }

  if (HasClass(sourceEvent, SmartHomeKeynodes::away_mode_requested))
    return ActivateAwayMode(action, home, sourceEvent);

  if (HasClass(sourceEvent, SmartHomeKeynodes::motion_detected)
      || HasClass(sourceEvent, SmartHomeKeynodes::door_opened))
  {
    return ProcessPresenceEvent(action, home, sourceEvent);
  }

  std::clog << "[AwayModeAgent] Unsupported event type\n";
  return action.FinishWithError();
}

bool AwayModeAgent::HasClass(ScAddr const & element, ScAddr const & classAddr) const
{
  return element.IsValid() && m_context.CheckConnector(classAddr, element, ScType::ConstPermPosArc);
}

void AwayModeAgent::AddClass(ScAddr const & element, ScAddr const & classAddr)
{
  if (!HasClass(element, classAddr))
    m_context.GenerateConnector(ScType::ConstPermPosArc, classAddr, element);
}

void AwayModeAgent::RemoveClass(ScAddr const & element, ScAddr const & classAddr)
{
  std::vector<ScAddr> arcsToErase;
  ScIterator3Ptr const iterator = m_context.CreateIterator3(classAddr, ScType::ConstPermPosArc, element);
  while (iterator->Next())
    arcsToErase.push_back(iterator->Get(1));

  for (ScAddr const & arc : arcsToErase)
  {
    if (m_context.IsElement(arc))
      m_context.EraseElement(arc);
  }
}

void AwayModeAgent::SetRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation)
{
  ScAddr const relationPair = m_context.GenerateConnector(ScType::ConstCommonArc, source, target);
  m_context.GenerateConnector(ScType::ConstPermPosArc, relation, relationPair);
}

ScAddr AwayModeAgent::CreateHomeEvent(
    ScAddr const & eventClass,
    ScAddr const & home,
    ScAddr const & sourceEvent)
{
  ScAddr const event = m_context.GenerateNode(ScType::ConstNode);
  AddClass(event, eventClass);
  SetRelation(event, home, SmartHomeKeynodes::nrel_event_home);

  if (sourceEvent.IsValid())
    SetRelation(event, sourceEvent, SmartHomeKeynodes::nrel_event_source);

  return event;
}

bool AwayModeAgent::RunPowerSavingAgent(ScAddr const & home, ScAddr const & trigger)
{
  ScAction powerSavingAction = m_context.GenerateAction(SmartHomeKeynodes::action_control_power_saving);
  powerSavingAction.SetArguments(home, trigger);

  if (!powerSavingAction.InitiateAndWait(5000))
  {
    std::clog << "[AwayModeAgent] PowerSavingAgent timeout\n";
    return false;
  }

  return powerSavingAction.IsFinishedSuccessfully();
}

ScResult AwayModeAgent::ActivateAwayMode(ScAction & action, ScAddr const & home, ScAddr const & sourceEvent)
{
  std::clog << "[AwayModeAgent] Activating away mode\n";

  AddClass(home, SmartHomeKeynodes::home_empty);
  AddClass(home, SmartHomeKeynodes::away_mode_active);
  AddClass(home, SmartHomeKeynodes::security_mode_active);
  RemoveClass(home, SmartHomeKeynodes::home_present);

  ScAddr const event = CreateHomeEvent(SmartHomeKeynodes::away_mode_activated, home, sourceEvent);
  SetRelation(
      SmartHomeKeynodes::away_mode_active,
      SmartHomeKeynodes::power_saving_agent_node,
      SmartHomeKeynodes::mode_affects_agent);
  SetRelation(
      SmartHomeKeynodes::away_mode_active,
      SmartHomeKeynodes::climate_control_agent_node,
      SmartHomeKeynodes::mode_affects_agent);
  SetRelation(
      sourceEvent,
      SmartHomeKeynodes::action_control_power_saving,
      SmartHomeKeynodes::event_triggers_action);

  if (!RunPowerSavingAgent(home, SmartHomeKeynodes::away_mode_active))
    return action.FinishWithError();

  ScStructure result = m_context.GenerateStructure();
  result << home << sourceEvent << event << SmartHomeKeynodes::home_empty << SmartHomeKeynodes::away_mode_active
         << SmartHomeKeynodes::security_mode_active << SmartHomeKeynodes::power_saving_agent_node
         << SmartHomeKeynodes::climate_control_agent_node;
  action.SetResult(result);

  return action.FinishSuccessfully();
}

ScResult AwayModeAgent::ProcessPresenceEvent(ScAction & action, ScAddr const & home, ScAddr const & sourceEvent)
{
  if (!HasClass(home, SmartHomeKeynodes::away_mode_active))
  {
    std::clog << "[AwayModeAgent] Presence event ignored because away mode is inactive\n";
    return action.FinishSuccessfully();
  }

  if (!HasClass(sourceEvent, SmartHomeKeynodes::confirmed_event))
  {
    std::clog << "[AwayModeAgent] Presence event is not confirmed\n";
    ScAddr const event = CreateHomeEvent(SmartHomeKeynodes::unverified_presence_event, home, sourceEvent);

    ScStructure result = m_context.GenerateStructure();
    result << home << sourceEvent << event << SmartHomeKeynodes::unverified_presence_event;
    action.SetResult(result);

    return action.FinishSuccessfully();
  }

  std::clog << "[AwayModeAgent] Confirmed return home, disabling away mode\n";

  RemoveClass(home, SmartHomeKeynodes::away_mode_active);
  RemoveClass(home, SmartHomeKeynodes::home_empty);
  RemoveClass(home, SmartHomeKeynodes::security_mode_active);
  AddClass(home, SmartHomeKeynodes::home_present);

  ScAddr const event = CreateHomeEvent(SmartHomeKeynodes::person_returned_home, home, sourceEvent);
  SetRelation(event, SmartHomeKeynodes::action_control_power_saving, SmartHomeKeynodes::event_triggers_action);

  if (!RunPowerSavingAgent(home, SmartHomeKeynodes::person_returned_home))
    return action.FinishWithError();

  ScStructure result = m_context.GenerateStructure();
  result << home << sourceEvent << event << SmartHomeKeynodes::home_present << SmartHomeKeynodes::person_returned_home;
  action.SetResult(result);

  return action.FinishSuccessfully();
}
