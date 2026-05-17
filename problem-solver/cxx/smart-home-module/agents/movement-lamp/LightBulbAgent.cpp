#include "LightBulbAgent.hpp"
#include "agents/common/automation_utils.hpp"

using namespace smart_home;

ScAddr LightBulbAgent::GetActionClass() const
{
  return Keynodes::action_check_motion_sensor;
}

ScResult LightBulbAgent::DoProgram(ScAction & action)
{
  ScAddr const sensor = Keynodes::sensor_motion_bedroom;
  ScAddr const lamp = FindLamp(sensor);

  if (!lamp.IsValid())
  {
    SC_LOG_WARNING("LightBulbAgent: lamp not found for sensor.");
    return action.FinishSuccessfully();
  }

  if (automation::IsAutomationBlocked(m_context, lamp))
  {
    SC_LOG_INFO("LightBulbAgent: automation blocked (hard-off or schedule).");
    return action.FinishSuccessfully();
  }

  ApplyLampState(lamp, IsMotionDetected(sensor));
  return action.FinishSuccessfully();
}

ScAddr LightBulbAgent::FindLamp(ScAddr const & sensor)
{
  ScIterator5Ptr it = m_context.CreateIterator5(
      ScType::ConstNode,
      ScType::ConstCommonArc,
      sensor,
      ScType::ConstPermPosArc,
      Keynodes::nrel_sensor);

  while (it->Next())
  {
    ScAddr const candidate = it->Get(0);
    if (m_context.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
      return candidate;
  }

  return ScAddr::Empty;
}

bool LightBulbAgent::IsMotionDetected(ScAddr const & sensor)
{
  return m_context.CheckConnector(Keynodes::concept_action_detected, sensor, ScType::ConstPermPosArc);
}

void LightBulbAgent::ApplyLampState(ScAddr const & lamp, bool motionDetected)
{
  ScAddr const targetState   = motionDetected ? Keynodes::concept_state_on  : Keynodes::concept_state_off;
  ScAddr const oppositeState = motionDetected ? Keynodes::concept_state_off : Keynodes::concept_state_on;

  SetState(lamp, targetState, oppositeState);

  SC_LOG_INFO(std::string("LightBulbAgent: lamp_bedroom -> ") + (motionDetected ? "ON" : "OFF"));
}

void LightBulbAgent::SetState(ScAddr const & device, ScAddr const & targetState, ScAddr const & oppositeState)
{
  ScIterator3Ptr oldIt = m_context.CreateIterator3(oppositeState, ScType::ConstPermPosArc, device);
  while (oldIt->Next())
    m_context.EraseElement(oldIt->Get(1));

  ScIterator3Ptr dupIt = m_context.CreateIterator3(targetState, ScType::ConstPermPosArc, device);
  while (dupIt->Next())
    m_context.EraseElement(dupIt->Get(1));

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, device);
}
