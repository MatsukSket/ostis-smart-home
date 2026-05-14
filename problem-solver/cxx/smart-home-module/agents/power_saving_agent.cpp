#include "power_saving_agent.hpp"

#include <iostream>

#include <sc-memory/sc_structure.hpp>

#include "keynodes/keynodes.hpp"

ScAddr PowerSavingAgent::GetActionClass() const
{
  return SmartHomeKeynodes::action_control_power_saving;
}

ScResult PowerSavingAgent::DoProgram(ScAction & action)
{
  auto const & [home, trigger] = action.GetArguments<2>();

  if (!home.IsValid() || !trigger.IsValid())
  {
    std::clog << "[PowerSavingAgent] Invalid action arguments\n";
    return action.FinishWithError();
  }

  ScStructure result = m_context.GenerateStructure();
  result << home << trigger;

  bool const shouldEnable =
      HasClass(home, SmartHomeKeynodes::away_mode_active) || HasClass(home, SmartHomeKeynodes::home_empty);

  if (shouldEnable)
    EnablePowerSavingMode(home, result);
  else
    DisablePowerSavingMode(home, result);

  action.SetResult(result);
  return action.FinishSuccessfully();
}

bool PowerSavingAgent::HasClass(ScAddr const & element, ScAddr const & classAddr) const
{
  return element.IsValid() && m_context.CheckConnector(classAddr, element, ScType::ConstPermPosArc);
}

bool PowerSavingAgent::HasRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation) const
{
  ScIterator5Ptr const iterator = m_context.CreateIterator5(source, ScType::Unknown, target, ScType::Unknown, relation);
  return iterator->Next();
}

void PowerSavingAgent::AddClass(ScAddr const & element, ScAddr const & classAddr)
{
  if (!HasClass(element, classAddr))
    m_context.GenerateConnector(ScType::ConstPermPosArc, classAddr, element);
}

void PowerSavingAgent::RemoveClass(ScAddr const & element, ScAddr const & classAddr)
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

void PowerSavingAgent::SetRelation(ScAddr const & source, ScAddr const & target, ScAddr const & relation)
{
  ScAddr const relationPair = m_context.GenerateConnector(ScType::ConstCommonArc, source, target);
  m_context.GenerateConnector(ScType::ConstPermPosArc, relation, relationPair);
}

ScAddr PowerSavingAgent::GetRelationTarget(ScAddr const & source, ScAddr const & relation) const
{
  if (!source.IsValid())
    return ScAddr::Empty;

  ScIterator5Ptr const iterator =
      m_context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  return iterator->Next() ? iterator->Get(2) : ScAddr::Empty;
}

void PowerSavingAgent::ReplaceRelationTarget(
    ScAddr const & source,
    ScAddr const & relation,
    ScAddr const & target)
{
  std::vector<ScAddr> elementsToErase;
  ScIterator5Ptr const iterator =
      m_context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  while (iterator->Next())
  {
    elementsToErase.push_back(iterator->Get(3));
    elementsToErase.push_back(iterator->Get(1));
  }

  for (ScAddr const & element : elementsToErase)
  {
    if (m_context.IsElement(element))
      m_context.EraseElement(element);
  }

  SetRelation(source, target, relation);
}

ScAddr PowerSavingAgent::CreateEvent(
    ScAddr const & eventClass,
    ScAddr const & home,
    ScAddr const & device,
    ScAddr const & state)
{
  ScAddr const event = m_context.GenerateNode(ScType::ConstNode);
  AddClass(event, eventClass);
  SetRelation(event, home, SmartHomeKeynodes::nrel_event_home);

  if (device.IsValid())
    SetRelation(event, device, SmartHomeKeynodes::nrel_event_device);

  if (state.IsValid())
    SetRelation(event, state, SmartHomeKeynodes::nrel_event_state);

  return event;
}

bool PowerSavingAgent::ShouldDisableDevice(ScAddr const & device) const
{
  bool const deviceCanBeDisabled =
      HasRelation(device, SmartHomeKeynodes::concept_on, SmartHomeKeynodes::device_can_be_disabled);
  bool const deviceIsCritical =
      HasClass(device, SmartHomeKeynodes::critical_device)
      || HasRelation(device, SmartHomeKeynodes::concept_on, SmartHomeKeynodes::device_is_critical);

  return HasClass(device, SmartHomeKeynodes::smart_device) && deviceCanBeDisabled && !deviceIsCritical;
}

std::vector<ScAddr> PowerSavingAgent::CollectClassInstances(ScAddr const & classAddr) const
{
  std::vector<ScAddr> result;
  ScIterator3Ptr const iterator = m_context.CreateIterator3(classAddr, ScType::ConstPermPosArc, ScType::Unknown);
  while (iterator->Next())
    result.push_back(iterator->Get(2));

  return result;
}

void PowerSavingAgent::EnablePowerSavingMode(ScAddr const & home, ScStructure & result)
{
  std::clog << "[PowerSavingAgent] Enabling power saving mode\n";

  AddClass(home, SmartHomeKeynodes::power_saving_mode);
  SetRelation(
      SmartHomeKeynodes::power_saving_mode,
      SmartHomeKeynodes::climate_control_agent_node,
      SmartHomeKeynodes::mode_affects_agent);
  CreateEvent(SmartHomeKeynodes::power_saving_started, home);

  for (ScAddr const & device : CollectClassInstances(SmartHomeKeynodes::smart_device))
  {
    if (!ShouldDisableDevice(device))
      continue;

    ScAddr const currentState = GetRelationTarget(device, SmartHomeKeynodes::nrel_current_state);
    if (currentState.IsValid())
      ReplaceRelationTarget(device, SmartHomeKeynodes::nrel_previous_state, currentState);

    ReplaceRelationTarget(device, SmartHomeKeynodes::nrel_current_state, SmartHomeKeynodes::concept_off);
    AddClass(device, SmartHomeKeynodes::disabled_device);
    SetRelation(SmartHomeKeynodes::power_saving_agent_node, device, SmartHomeKeynodes::agent_controls_device);
    CreateEvent(SmartHomeKeynodes::concept_device_disabled_event, home, device, SmartHomeKeynodes::concept_off);

    result << device << SmartHomeKeynodes::concept_off << SmartHomeKeynodes::disabled_device;
  }

  result << SmartHomeKeynodes::power_saving_mode << SmartHomeKeynodes::power_saving_started;
}

void PowerSavingAgent::DisablePowerSavingMode(ScAddr const & home, ScStructure & result)
{
  std::clog << "[PowerSavingAgent] Disabling power saving mode\n";

  RemoveClass(home, SmartHomeKeynodes::power_saving_mode);
  CreateEvent(SmartHomeKeynodes::power_saving_finished, home);

  for (ScAddr const & device : CollectClassInstances(SmartHomeKeynodes::disabled_device))
  {
    ScAddr const previousState = GetRelationTarget(device, SmartHomeKeynodes::nrel_previous_state);
    if (previousState.IsValid())
    {
      ReplaceRelationTarget(device, SmartHomeKeynodes::nrel_current_state, previousState);

      ScAddr const eventClass = previousState == SmartHomeKeynodes::concept_on
          ? SmartHomeKeynodes::concept_device_enabled_event
          : SmartHomeKeynodes::concept_device_disabled_event;
      CreateEvent(eventClass, home, device, previousState);
    }

    RemoveClass(device, SmartHomeKeynodes::disabled_device);
    CreateEvent(SmartHomeKeynodes::power_saving_finished, home, device);
    result << device;
  }

  result << SmartHomeKeynodes::power_saving_finished;
}
