#include "LightBulbAgent.hpp"

using namespace smart_home;

ScAddr LightBulbAgent::GetActionClass() const
{
  return Keynodes::action_check_motion_sensor;
}

ScResult LightBulbAgent::DoProgram(ScAction & action)
{
  ScAddr const sensor = Keynodes::sensor_motion_bedroom;

  bool const motionDetected =
      m_context.CheckConnector(Keynodes::concept_action_detected, sensor, ScType::ConstPermPosArc);

  ScAddr lamp;
  ScIterator5Ptr lampIt = m_context.CreateIterator5(
      ScType::ConstNode,
      ScType::ConstCommonArc,
      sensor,
      ScType::ConstPermPosArc,
      Keynodes::nrel_sensor);

  while (lampIt->Next())
  {
    ScAddr const candidate = lampIt->Get(0);
    if (m_context.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
    {
      lamp = candidate;
      break;
    }
  }

  if (!lamp.IsValid())
  {
    SC_LOG_WARNING("LightBulbAgent: lamp not found for sensor.");
    return action.FinishSuccessfully();
  }

  ScAddr const targetState = motionDetected ? Keynodes::concept_state_on : Keynodes::concept_state_off;
  ScAddr const oppositeState = motionDetected ? Keynodes::concept_state_off : Keynodes::concept_state_on;

  SwitchDeviceState(lamp, targetState, oppositeState);

  SC_LOG_INFO(std::string("LightBulbAgent: lamp_bedroom -> ") + (motionDetected ? "ON" : "OFF"));

  return action.FinishSuccessfully();
}

void LightBulbAgent::SwitchDeviceState(
    ScAddr const & device,
    ScAddr const & targetState,
    ScAddr const & oppositeState)
{
  ScIterator3Ptr checkIt = m_context.CreateIterator3(targetState, ScType::ConstPermPosArc, device);
  if (checkIt->Next())
    return;

  ScIterator3Ptr oldIt = m_context.CreateIterator3(oppositeState, ScType::ConstPermPosArc, device);
  while (oldIt->Next())
    m_context.EraseElement(oldIt->Get(1));

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, device);
}
