#include "humidity_control_agent.hpp"

#include <vector>

#include <sc-memory/sc_structure.hpp>

#include "agents/common/automation_utils.hpp"
#include "keynodes/Keynodes.hpp"

ScAddr HumidityControlAgent::GetActionClass() const
{
  return SmartHomeKeynodes::action_control_room_humidity;
}

ScResult HumidityControlAgent::DoProgram(ScAction & action)
{
  auto const & [sensor] = action.GetArguments<1>();

  if (!sensor.IsValid())
    return action.FinishWithError();

  ScAddr const device = GetRelationTarget(sensor, SmartHomeKeynodes::nrel_controls);
  ScAddr const measurement = GetRelationTarget(sensor, SmartHomeKeynodes::nrel_current_measurement);
  ScAddr const humidityValue = GetRelationTarget(measurement, SmartHomeKeynodes::rrel_value);

  if (!device.IsValid() || !measurement.IsValid() || !humidityValue.IsValid())
    return action.FinishWithError();

  ScAddr const targetState = GetTargetState(device, humidityValue);
  if (!targetState.IsValid())
    return action.FinishWithError();

  if (smart_home::automation::IsAutomationBlocked(m_context, device))
  {
    ScStructure result = m_context.GenerateStructure();
    result << sensor << device << measurement << humidityValue;
    action.SetResult(result);
    return action.FinishSuccessfully();
  }

  if (!RunDeviceStateAgent(device, targetState))
    return action.FinishWithError();

  ScStructure result = m_context.GenerateStructure();
  result << sensor << device << measurement << humidityValue << targetState;

  if (targetState == SmartHomeKeynodes::concept_on)
  {
    ScAddr const normalizedMeasurement = GetRelationTarget(sensor, SmartHomeKeynodes::nrel_normalized_measurement);
    if (normalizedMeasurement.IsValid())
    {
      ReplaceRelationTarget(sensor, SmartHomeKeynodes::nrel_current_measurement, normalizedMeasurement);
      result << normalizedMeasurement << SmartHomeKeynodes::nrel_normalized_measurement;

      if (!RunDeviceStateAgent(device, SmartHomeKeynodes::concept_off))
        return action.FinishWithError();
    }
  }

  action.SetResult(result);
  return action.FinishSuccessfully();
}

ScAddr HumidityControlAgent::GetRelationTarget(ScAddr const & source, ScAddr const & relation) const
{
  if (!source.IsValid())
    return ScAddr::Empty;

  ScIterator5Ptr const iterator =
      m_context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  return iterator->Next() ? iterator->Get(2) : ScAddr::Empty;
}

bool HumidityControlAgent::HasClass(ScAddr const & element, ScAddr const & classAddr) const
{
  return m_context.CheckConnector(classAddr, element, ScType::ConstPermPosArc);
}

ScAddr HumidityControlAgent::GetTargetState(ScAddr const & device, ScAddr const & humidityValue) const
{
  if (HasClass(humidityValue, SmartHomeKeynodes::concept_normal_humidity))
    return SmartHomeKeynodes::concept_off;

  bool const deviceIsHumidifier = HasClass(device, SmartHomeKeynodes::concept_humidifier);
  bool const deviceIsDehumidifier = HasClass(device, SmartHomeKeynodes::concept_dehumidifier);
  bool const humidityIsLow = HasClass(humidityValue, SmartHomeKeynodes::concept_low_humidity);
  bool const humidityIsHigh = HasClass(humidityValue, SmartHomeKeynodes::concept_high_humidity);

  if ((humidityIsLow && deviceIsHumidifier) || (humidityIsHigh && deviceIsDehumidifier))
    return SmartHomeKeynodes::concept_on;

  if ((humidityIsLow && deviceIsDehumidifier) || (humidityIsHigh && deviceIsHumidifier))
    return SmartHomeKeynodes::concept_off;

  return ScAddr::Empty;
}

void HumidityControlAgent::ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target)
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

bool HumidityControlAgent::RunDeviceStateAgent(ScAddr const & device, ScAddr const & targetState)
{
  ScAction deviceStateAction = m_context.GenerateAction(SmartHomeKeynodes::action_change_device_state);
  deviceStateAction.SetArguments(device, targetState);

  if (!deviceStateAction.InitiateAndWait(5000))
    return false;

  return deviceStateAction.IsFinishedSuccessfully();
}
