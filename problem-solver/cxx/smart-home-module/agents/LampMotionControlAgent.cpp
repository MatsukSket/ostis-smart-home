#include "LampMotionControlAgent.hpp"

using namespace smart_home;

ScAddr LampMotionControlAgent::GetActionClass() const
{
  return Keynodes::action_check_motion_sensor;
}

ScResult LampMotionControlAgent::DoProgram(ScAction & action)
{
  SC_LOG_INFO("LampMotionControlAgent: начало обработки.");

  ScIterator3Ptr lampIt = m_context.CreateIterator3(
      Keynodes::concept_light_bulb,
      ScType::ConstPermPosArc,
      ScType::ConstNode);

  bool foundAnyLamp = false;

  while (lampIt->Next())
  {
    foundAnyLamp = true;
    ScAddr lampAddr = lampIt->Get(2);
    SC_LOG_INFO("LampMotionControlAgent: проверяю лампочку.");

    ScIterator5Ptr sensorIt = m_context.CreateIterator5(
        lampAddr,
        ScType::ConstCommonArc,
        ScType::ConstNode,
        ScType::ConstPermPosArc,
        Keynodes::nrel_sensor);

    if (!sensorIt->Next())
    {
      SC_LOG_INFO("LampMotionControlAgent: у лампочки нет nrel_sensor, пропускаю.");
      continue;
    }

    ScAddr sensorAddr = sensorIt->Get(2);

    ScIterator3Ptr motionCheckIt = m_context.CreateIterator3(
        Keynodes::concept_motion_sensor,
        ScType::ConstPermPosArc,
        sensorAddr);

    if (!motionCheckIt->Next())
    {
      SC_LOG_INFO("LampMotionControlAgent: привязанный датчик не является датчиком движения, пропускаю.");
      continue;
    }

    ScIterator3Ptr detectedIt = m_context.CreateIterator3(
        Keynodes::concept_action_detected,
        ScType::ConstPermPosArc,
        sensorAddr);

    if (detectedIt->Next())
    {
      SC_LOG_INFO("LampMotionControlAgent: движение обнаружено -> включаю лампочку.");
      SwitchDeviceState(lampAddr, Keynodes::concept_state_on, Keynodes::concept_state_off);
    }
    else
    {
      SC_LOG_INFO("LampMotionControlAgent: движения нет -> выключаю лампочку.");
      SwitchDeviceState(lampAddr, Keynodes::concept_state_off, Keynodes::concept_state_on);
    }
  }

  if (!foundAnyLamp)
    SC_LOG_INFO("LampMotionControlAgent: лампочки не найдены.");

  return action.FinishSuccessfully();
}

void LampMotionControlAgent::SwitchDeviceState(
    ScAddr const & deviceAddr,
    ScAddr const & targetState,
    ScAddr const & oppositeState)
{
  ScIterator3Ptr checkIt = m_context.CreateIterator3(
      targetState,
      ScType::ConstPermPosArc,
      deviceAddr);

  if (checkIt->Next())
    return;

  ScIterator3Ptr oldIt = m_context.CreateIterator3(
      oppositeState,
      ScType::ConstPermPosArc,
      deviceAddr);

  while (oldIt->Next())
    m_context.EraseElement(oldIt->Get(1));

  m_context.GenerateConnector(ScType::ConstPermPosArc, targetState, deviceAddr);
}
