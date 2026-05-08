#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_memory.hpp>
#include <atomic>
#include <thread>
#include "keynodes/keynodes.hpp"

namespace smart_home
{

class ScheduleTickerAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;

  static void Start();
  static void Stop();

private:
  static std::atomic<bool> m_running;
  static std::thread m_tickerThread;

  static void TickerLoop();
};

}