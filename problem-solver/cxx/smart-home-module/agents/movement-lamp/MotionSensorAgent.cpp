#include "MotionSensorAgent.hpp"
#include "agents/common/automation_utils.hpp"
#include <sc-memory/sc_agent_context.hpp>
#include <thread>
#include <chrono>

using namespace smart_home;

std::atomic<bool> MotionSensorAgent::m_running(false);
std::thread MotionSensorAgent::m_monitorThread;
bool MotionSensorAgent::m_lastMotionState = false;

ScAddr MotionSensorAgent::GetActionClass() const
{
  return Keynodes::action_toggle_motion_sensor;
}

ScResult MotionSensorAgent::DoProgram(ScAction & action)
{
  return action.FinishSuccessfully();
}

void MotionSensorAgent::Start()
{
  if (m_running.load())
    return;

  SC_LOG_INFO("MotionSensorAgent: starting monitor thread.");
  m_running.store(true);
  m_monitorThread = std::thread(MonitorLoop);
  m_monitorThread.detach();
}

void MotionSensorAgent::Stop()
{
  m_running.store(false);
  SC_LOG_INFO("MotionSensorAgent: stopped.");
}

// ── вспомогательные функции ──────────────────────────────────────────────────

bool MotionSensorAgent::ReadSensorState(ScAgentContext & ctx, ScAddr const & sensor)
{
  return ctx.CheckConnector(Keynodes::concept_action_detected, sensor, ScType::ConstPermPosArc);
}

void MotionSensorAgent::CleanupConflictingStates(ScAgentContext & ctx, ScAddr const & sensor)
{
  bool const detected    = ctx.CheckConnector(Keynodes::concept_action_detected,     sensor, ScType::ConstPermPosArc);
  bool const notDetected = ctx.CheckConnector(Keynodes::concept_action_not_detected, sensor, ScType::ConstPermPosArc);

  if (!detected && !notDetected)
    return;

  if (detected && notDetected)
  {
    ScIterator3Ptr it = ctx.CreateIterator3(Keynodes::concept_action_not_detected, ScType::ConstPermPosArc, sensor);
    while (it->Next())
      ctx.EraseElement(it->Get(1));
  }
}

void MotionSensorAgent::EnsureDefaultState(ScAgentContext & ctx, ScAddr const & sensor)
{
  bool const detected    = ctx.CheckConnector(Keynodes::concept_action_detected,     sensor, ScType::ConstPermPosArc);
  bool const notDetected = ctx.CheckConnector(Keynodes::concept_action_not_detected, sensor, ScType::ConstPermPosArc);

  if (!detected && !notDetected)
    ctx.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_action_not_detected, sensor);
}

ScAddr MotionSensorAgent::FindLamp(ScAgentContext & ctx, ScAddr const & sensor)
{
  ScIterator5Ptr it = ctx.CreateIterator5(
      ScType::ConstNode,
      ScType::ConstCommonArc,
      sensor,
      ScType::ConstPermPosArc,
      Keynodes::nrel_sensor);

  while (it->Next())
  {
    ScAddr const candidate = it->Get(0);
    if (ctx.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
      return candidate;
  }

  return ScAddr::Empty;
}

bool MotionSensorAgent::IsLampInSync(ScAgentContext & ctx, ScAddr const & lamp, bool motionDetected)
{
  ScAddr const expectedState = motionDetected ? Keynodes::concept_state_on : Keynodes::concept_state_off;
  return ctx.CheckConnector(expectedState, lamp, ScType::ConstPermPosArc);
}

// ── основной цикл ────────────────────────────────────────────────────────────

void MotionSensorAgent::MonitorLoop()
{
  bool initialized = false;

  while (m_running.load())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (!m_running.load())
      break;

    try
    {
      ScAgentContext ctx;
      ScAddr const sensor = Keynodes::sensor_motion_bedroom;

      if (!sensor.IsValid())
        continue;

      CleanupConflictingStates(ctx, sensor);
      EnsureDefaultState(ctx, sensor);

      bool const currentState = ReadSensorState(ctx, sensor);

      if (!initialized)
      {
        m_lastMotionState = currentState;
        initialized = true;
        SC_LOG_INFO(
            std::string("MotionSensorAgent: initialized, state=") +
            (currentState ? "detected" : "not detected"));
        ctx.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
        continue;
      }

      ScAddr const lamp = FindLamp(ctx, sensor);

      if (lamp.IsValid() && automation::IsAutomationBlocked(ctx, lamp))
        continue;

      bool const stateChanged = currentState != m_lastMotionState;
      bool const lampOutOfSync = lamp.IsValid() && !IsLampInSync(ctx, lamp, currentState);

      if (stateChanged)
      {
        m_lastMotionState = currentState;
        SC_LOG_INFO(
            std::string("MotionSensorAgent: state changed -> ") +
            (currentState ? "detected" : "not detected"));
      }
      else if (lampOutOfSync)
      {
        SC_LOG_INFO("MotionSensorAgent: lamp out of sync, resyncing.");
      }

      if (stateChanged || lampOutOfSync)
        ctx.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
    }
    catch (std::exception const & e)
    {
      SC_LOG_ERROR("MotionSensorAgent monitor error: " << e.what());
    }
  }
}
