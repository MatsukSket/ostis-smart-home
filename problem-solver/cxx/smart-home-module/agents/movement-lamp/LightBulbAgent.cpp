#include "LightBulbAgent.hpp"
#include "agents/common/automation_utils.hpp"

using namespace smart_home;

static const std::string RESET      = "\033[0m";
static const std::string GREEN      = "\033[32m";
static const std::string YELLOW     = "\033[33m";
static const std::string BOLD_RED   = "\033[1;31m";
static const std::string BOLD_GREEN = "\033[1;32m";

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
    SC_LOG_INFO(BOLD_RED + "LightBulbAgent: lamp not found." + RESET);
    return action.FinishSuccessfully();
  }

  if (automation::IsDeviceHardOff(m_context, lamp))
  {
    SC_LOG_INFO(BOLD_RED + "LightBulbAgent: hard-off active, forcing OFF." + RESET);
    ForceOff(lamp);
    return action.FinishSuccessfully();
  }

  if (automation::HasScheduleControl(m_context, lamp))
  {
    SC_LOG_INFO(YELLOW + "LightBulbAgent: schedule active, skipping." + RESET);
    return action.FinishSuccessfully();
  }

  bool const motionDetected = IsMotionDetected(sensor);
  ApplyLampState(lamp, motionDetected);
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

void LightBulbAgent::ForceOff(ScAddr const & lamp)
{
  SetState(lamp, Keynodes::concept_state_off, Keynodes::concept_state_on);
}

void LightBulbAgent::ApplyLampState(ScAddr const & lamp, bool motionDetected)
{
  ScAddr const targetState   = motionDetected ? Keynodes::concept_state_on  : Keynodes::concept_state_off;
  ScAddr const oppositeState = motionDetected ? Keynodes::concept_state_off : Keynodes::concept_state_on;

  SetState(lamp, targetState, oppositeState);
  SC_LOG_INFO(BOLD_GREEN + "LightBulbAgent: lamp_bedroom -> " + (motionDetected ? "ON" : "OFF") + RESET);
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
