#include "MotionSensorAgent.hpp"
#include <sc-memory/sc_agent_context.hpp>

using namespace smart_home;

ScAddr MotionSensorAgent::GetActionClass() const
{
  return Keynodes::action_toggle_motion_sensor;
}

ScResult MotionSensorAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("MotionSensorAgent: начало обработки.");

  ScAddr const sensorAddr = Keynodes::sensor_motion_bedroom;

  bool isDetected = false;

  ScIterator3Ptr detectedIt = m_context.CreateIterator3(
      Keynodes::concept_action_detected,
      ScType::ConstPermPosArc,
      sensorAddr);

  if (detectedIt->Next())
    isDetected = true;

  if (isDetected)
  {
    SC_LOG_INFO("MotionSensorAgent: движение было обнаружено -> переключаю на НЕ обнаружено.");
    SwitchSensorState(
        sensorAddr,
        Keynodes::concept_action_not_detected,
        Keynodes::concept_action_detected);
  }
  else
  {
    SC_LOG_INFO("MotionSensorAgent: движение не было обнаружено -> переключаю на ОБНАРУЖЕНО.");
    SwitchSensorState(
        sensorAddr,
        Keynodes::concept_action_detected,
        Keynodes::concept_action_not_detected);
  }

  SC_LOG_INFO("MotionSensorAgent: инициирую action_check_motion_sensor.");
  m_context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();

  return action.FinishSuccessfully();
}

void MotionSensorAgent::SwitchSensorState(
    ScAddr const & sensorAddr,
    ScAddr const & targetState,
    ScAddr const & oppositeState)
{
  ScIterator3Ptr oldIt = m_context.CreateIterator3(
      oppositeState,
      ScType::ConstPermPosArc,
      sensorAddr);

  while (oldIt->Next())
    m_context.EraseElement(oldIt->Get(1));

  ScIterator3Ptr checkIt = m_context.CreateIterator3(
      targetState,
      ScType::ConstPermPosArc,
      sensorAddr);

  if (!checkIt->Next())
    m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, sensorAddr);
}
