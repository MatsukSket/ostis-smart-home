#include "ScheduleTickerAgent.hpp"
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_iterator.hpp>
#include <thread>
#include <chrono>

using namespace smart_home;

std::atomic<bool> ScheduleTickerAgent::m_running(false);
std::thread ScheduleTickerAgent::m_tickerThread;

SC_AGENT_IMPLEMENTATION(ScheduleTickerAgent)
{
  if (m_running.load())
  {
    SC_LOG_INFO("ScheduleTickerAgent: ticker already running, skipping.");
    return SC_RESULT_OK;
  }

  SC_LOG_INFO("ScheduleTickerAgent: starting ticker thread.");
  m_running.store(true);

  // Создаём отдельный контекст для потока
  ScMemoryContext * threadCtx = new ScMemoryContext(sc_access_lvl_make_max, "ScheduleTickerThread");

  m_tickerThread = std::thread(TickerLoop, threadCtx);
  m_tickerThread.detach();

  return SC_RESULT_OK;
}

void ScheduleTickerAgent::TickerLoop(ScMemoryContext * ctx)
{
  while (m_running.load())
  {
    // Ждём 60 секунд
    std::this_thread::sleep_for(std::chrono::seconds(60));

    if (!m_running.load())
      break;

    SC_LOG_INFO("ScheduleTickerAgent: tick — creating check_schedule action.");

    // Создаём экземпляр действия
    ScAddr actionInstance = ctx->CreateNode(ScType::NodeConst);
    ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::action_check_schedule, actionInstance);
  }

  delete ctx;
  SC_LOG_INFO("ScheduleTickerAgent: ticker thread stopped.");
}

void ScheduleTickerAgent::StopTicker()
{
  m_running.store(false);
}