#include "temperature_control_agent.hpp"

#include <vector>

#include <sc-memory/sc_structure.hpp>

#include "agents/common/automation_utils.hpp"
#include "keynodes/Keynodes.hpp"

namespace
{

enum class TemperatureLevel
{
  Unknown,
  Low,
  Normal,
  High
};

TemperatureLevel ResolveTemperatureLevel(ScMemoryContext & context, ScAddr const & value)
{
  if (!value.IsValid())
    return TemperatureLevel::Unknown;

  if (context.CheckConnector(SmartHomeKeynodes::concept_low_temperature, value, ScType::ConstPermPosArc))
    return TemperatureLevel::Low;

  if (context.CheckConnector(SmartHomeKeynodes::concept_normal_temperature, value, ScType::ConstPermPosArc))
    return TemperatureLevel::Normal;

  if (context.CheckConnector(SmartHomeKeynodes::concept_high_temperature, value, ScType::ConstPermPosArc))
    return TemperatureLevel::High;

  return TemperatureLevel::Unknown;
}

}  // namespace

ScAddr TemperatureControlAgent::GetActionClass() const
{
  return SmartHomeKeynodes::action_control_room_temperature;
}

ScResult TemperatureControlAgent::DoProgram(ScAction & action)
{
  auto const & [sensor] = action.GetArguments<1>();

  if (!sensor.IsValid())
    return action.FinishWithError();

  ScAddr const measurement = GetRelationTarget(sensor, SmartHomeKeynodes::nrel_current_measurement);
  ScAddr const temperatureValue = GetRelationTarget(measurement, SmartHomeKeynodes::rrel_value);

  if (!measurement.IsValid() || !temperatureValue.IsValid())
    return action.FinishWithError();

  ScAddr conditioner;
  ScAddr heater;
  for (ScAddr const & device : GetRelationTargets(sensor, SmartHomeKeynodes::nrel_controls))
  {
    if (!conditioner.IsValid() && HasClass(device, SmartHomeKeynodes::concept_air_conditioner))
      conditioner = device;
    else if (!heater.IsValid() && HasClass(device, SmartHomeKeynodes::concept_heater))
      heater = device;
  }

  if (!conditioner.IsValid() || !heater.IsValid())
    return action.FinishWithError();

  ScStructure result = m_context.GenerateStructure();
  result << sensor << measurement << temperatureValue << conditioner << heater;

  TemperatureLevel const level = ResolveTemperatureLevel(m_context, temperatureValue);
  if (level == TemperatureLevel::Unknown)
    return action.FinishWithError();

  auto applyState = [this, &result](ScAddr const & device, ScAddr const & targetState) -> bool
  {
  if (smart_home::automation::IsAutomationBlocked(m_context, device))
      return true;

    result << device << targetState;
    return RunDeviceStateAgent(device, targetState);
  };

  switch (level)
  {
  case TemperatureLevel::High:
    if (!applyState(conditioner, SmartHomeKeynodes::concept_on)
        || !applyState(heater, SmartHomeKeynodes::concept_off))
    {
      return action.FinishWithError();
    }
    break;
  case TemperatureLevel::Low:
    if (!applyState(conditioner, SmartHomeKeynodes::concept_off)
        || !applyState(heater, SmartHomeKeynodes::concept_on))
    {
      return action.FinishWithError();
    }
    break;
  case TemperatureLevel::Normal:
    if (!applyState(conditioner, SmartHomeKeynodes::concept_off)
        || !applyState(heater, SmartHomeKeynodes::concept_off))
    {
      return action.FinishWithError();
    }
    break;
  case TemperatureLevel::Unknown:
    return action.FinishWithError();
  }

  if (level != TemperatureLevel::Normal)
  {
    ScAddr const normalizedMeasurement = GetRelationTarget(sensor, SmartHomeKeynodes::nrel_normalized_measurement);
    if (normalizedMeasurement.IsValid())
    {
      ReplaceRelationTarget(sensor, SmartHomeKeynodes::nrel_current_measurement, normalizedMeasurement);
      result << normalizedMeasurement << SmartHomeKeynodes::nrel_normalized_measurement;

      if (level == TemperatureLevel::High)
      {
        if (!applyState(conditioner, SmartHomeKeynodes::concept_off))
          return action.FinishWithError();
      }
      else
      {
        if (!applyState(heater, SmartHomeKeynodes::concept_off))
          return action.FinishWithError();
      }
    }
  }

  action.SetResult(result);
  return action.FinishSuccessfully();
}

ScAddr TemperatureControlAgent::GetRelationTarget(ScAddr const & source, ScAddr const & relation) const
{
  return smart_home::automation::GetRelationTarget(m_context, source, relation);
}

std::vector<ScAddr> TemperatureControlAgent::GetRelationTargets(ScAddr const & source, ScAddr const & relation) const
{
  return smart_home::automation::GetRelationTargets(m_context, source, relation);
}

bool TemperatureControlAgent::HasClass(ScAddr const & element, ScAddr const & classAddr) const
{
  return smart_home::automation::HasClass(m_context, element, classAddr);
}

void TemperatureControlAgent::ReplaceRelationTarget(ScAddr const & source, ScAddr const & relation, ScAddr const & target)
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

bool TemperatureControlAgent::RunDeviceStateAgent(ScAddr const & device, ScAddr const & targetState)
{
  ScAction deviceStateAction = m_context.GenerateAction(SmartHomeKeynodes::action_change_device_state);
  deviceStateAction.SetArguments(device, targetState);

  if (!deviceStateAction.InitiateAndWait(5000))
    return false;

  return deviceStateAction.IsFinishedSuccessfully();
}
