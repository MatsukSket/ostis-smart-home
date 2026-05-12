#include "ScheduleTickerAgent.hpp"
#include <sc-memory/sc_agent_context.hpp>
#include <thread>
#include <chrono>

using namespace smart_home;

std::atomic<bool> ScheduleTickerAgent::m_running(false);
std::thread ScheduleTickerAgent::m_tickerThread;

ScAddr ScheduleTickerAgent::GetActionClass() const
{
  return Keynodes::action_check_schedule;
}

ScResult ScheduleTickerAgent::DoProgram(ScAction & action)
{
  return action.FinishSuccessfully();
}

void ScheduleTickerAgent::Start()
{
  if (m_running.load())
    return;

  SC_LOG_INFO("ScheduleTickerAgent: starting ticker thread.");
  m_running.store(true);
  m_tickerThread = std::thread(TickerLoop);
  m_tickerThread.detach();
}

void ScheduleTickerAgent::Stop()
{
  m_running.store(false);
  SC_LOG_INFO("ScheduleTickerAgent: stopped.");
}

void ScheduleTickerAgent::TickerLoop()
{
  while (m_running.load())
  {
    std::this_thread::sleep_for(std::chrono::seconds(60));

    if (!m_running.load())
      break;

    SC_LOG_INFO("ScheduleTickerAgent: tick.");

    try
    {
      ScAgentContext ctx;
      ctx.GenerateAction(Keynodes::action_check_schedule).Initiate();
    }
    catch (std::exception const & e)
    {
      SC_LOG_ERROR("ScheduleTickerAgent tick error: " << e.what());
    }
  }
}