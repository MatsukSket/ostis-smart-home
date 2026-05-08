#include "SmartHomeModule.hpp"
#include "keynodes/Keynodes.hpp"
#include "agents/DeviceScheduleMonitorAgent.hpp"
#include "agents/ScheduleTickerAgent.hpp"

using namespace smart_home;

SC_IMPLEMENT_MODULE(SmartHomeModule)

sc_result SmartHomeModule::InitializeImpl()
{
  if (!Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(DeviceScheduleMonitorAgent)
  SC_AGENT_REGISTER(ScheduleTickerAgent)

  ScMemoryContext ctx(sc_access_lvl_make_max, "ModuleInit");
  ScAddr starterAction = ctx.CreateNode(ScType::NodeConst);
  ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::action_start_schedule_ticker, starterAction);

  return SC_RESULT_OK;
}

sc_result SmartHomeModule::ShutdownImpl()
{
  ScheduleTickerAgent::StopTicker();

  SC_AGENT_UNREGISTER(ScheduleTickerAgent)
  SC_AGENT_UNREGISTER(DeviceScheduleMonitorAgent)

  return SC_RESULT_OK;
}