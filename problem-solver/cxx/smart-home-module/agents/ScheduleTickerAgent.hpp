#pragma once

#include <sc-memory/sc_agent.hpp>
#include <atomic>
#include <thread>
#include "keynodes/Keynodes.hpp"
#include "ScheduleTickerAgent.generated.hpp"

namespace smart_home
{

class ScheduleTickerAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::action_start_schedule_ticker, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

public:
  static void StopTicker();

private:
  static std::atomic<bool> m_running;
  static std::thread m_tickerThread;

  static void TickerLoop(ScMemoryContext * ctx);
};

}