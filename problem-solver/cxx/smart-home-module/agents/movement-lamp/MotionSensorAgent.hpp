#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_memory.hpp>
#include <atomic>
#include <thread>
#include "keynodes/Keynodes.hpp"

namespace smart_home
{

class MotionSensorAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

  static void Start();
  static void Stop();

private:
  static std::atomic<bool> m_running;
  static std::thread m_monitorThread;
  static bool m_lastMotionState;

  static void MonitorLoop();

  static bool ReadSensorState(ScAgentContext & ctx, ScAddr const & sensor);
  static void CleanupConflictingStates(ScAgentContext & ctx, ScAddr const & sensor);
  static void EnsureDefaultState(ScAgentContext & ctx, ScAddr const & sensor);
  static ScAddr FindLamp(ScAgentContext & ctx, ScAddr const & sensor);
  static bool IsLampInSync(ScAgentContext & ctx, ScAddr const & lamp, bool motionDetected);
};

}
