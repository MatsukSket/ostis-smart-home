#include "device_state_agent.hpp"

#include <vector>

#include <sc-memory/sc_structure.hpp>

#include "keynodes/keynodes.hpp"

ScAddr DeviceStateAgent::GetActionClass() const
{
  return SmartHomeKeynodes::action_change_device_state;
}

ScResult DeviceStateAgent::DoProgram(ScAction & action)
{
  auto const & [device, targetState] = action.GetArguments<2>();

  if (!device.IsValid())
    return action.FinishWithError();

  bool const stateIsValid =
      targetState == SmartHomeKeynodes::concept_on || targetState == SmartHomeKeynodes::concept_off;
  if (!targetState.IsValid() || !stateIsValid)
    return action.FinishWithError();

  ReplaceRelationTarget(device, SmartHomeKeynodes::nrel_current_state, targetState);
  ScAddr const event = CreateDeviceStateEvent(device, targetState);

  ScStructure result = m_context.GenerateStructure();
  result << device << targetState << event << SmartHomeKeynodes::nrel_current_state
         << SmartHomeKeynodes::nrel_event_device << SmartHomeKeynodes::nrel_event_state;
  action.SetResult(result);

  return action.FinishSuccessfully();
}

void DeviceStateAgent::ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target)
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

  ScAddr const relationPair = m_context.GenerateConnector(ScType::ConstCommonArc, source, target);
  m_context.GenerateConnector(ScType::ConstPermPosArc, relation, relationPair);
}

ScAddr DeviceStateAgent::CreateDeviceStateEvent(ScAddr const & device, ScAddr const & targetState)
{
  ScAddr const event = m_context.GenerateNode(ScType::ConstNode);
  m_context.GenerateConnector(ScType::ConstPermPosArc, SmartHomeKeynodes::concept_device_state_event, event);

  ScAddr const eventClass = targetState == SmartHomeKeynodes::concept_on
      ? SmartHomeKeynodes::concept_device_enabled_event
      : SmartHomeKeynodes::concept_device_disabled_event;
  m_context.GenerateConnector(ScType::ConstPermPosArc, eventClass, event);

  ScAddr const eventDevicePair = m_context.GenerateConnector(ScType::ConstCommonArc, event, device);
  m_context.GenerateConnector(ScType::ConstPermPosArc, SmartHomeKeynodes::nrel_event_device, eventDevicePair);

  ScAddr const eventStatePair = m_context.GenerateConnector(ScType::ConstCommonArc, event, targetState);
  m_context.GenerateConnector(ScType::ConstPermPosArc, SmartHomeKeynodes::nrel_event_state, eventStatePair);

  return event;
}
