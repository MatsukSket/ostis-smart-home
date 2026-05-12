#include "LightBulbAgent.hpp"

using namespace smart_home;

ScAddr LightBulbAgent::GetActionClass() const
{
  return Keynodes::action_check_motion_sensor;
}

ScResult LightBulbAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("LightBulbAgent: DoProgram called.");

  ScAddr const sensor = Keynodes::sensor_motion_bedroom;

  bool const motionDetected =
      m_context.CheckConnector(Keynodes::concept_action_detected, sensor, ScType::ConstPermPosArc);

  SC_LOG_INFO(std::string("LightBulbAgent: motionDetected=") + (motionDetected ? "true" : "false"));

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
    SC_LOG_WARNING("LightBulbAgent: lamp not found.");
    return action.FinishSuccessfully();
  }

  SC_LOG_INFO("LightBulbAgent: lamp found, switching state.");

  ScAddr const targetState   = motionDetected ? Keynodes::concept_state_on  : Keynodes::concept_state_off;
  ScAddr const oppositeState = motionDetected ? Keynodes::concept_state_off : Keynodes::concept_state_on;

  {
    ScIterator3Ptr it = m_context.CreateIterator3(oppositeState, ScType::ConstPermPosArc, lamp);
    while (it->Next())
      m_context.EraseElement(it->Get(1));
  }
  {
    ScIterator3Ptr it = m_context.CreateIterator3(targetState, ScType::ConstPermPosArc, lamp);
    while (it->Next())
      m_context.EraseElement(it->Get(1));
  }

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, lamp);

  SC_LOG_INFO(std::string("LightBulbAgent: lamp_bedroom -> ") + (motionDetected ? "ON" : "OFF"));

  return action.FinishSuccessfully();
}
