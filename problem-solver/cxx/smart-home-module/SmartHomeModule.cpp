#include "SmartHomeModule.hpp"
#include "keynodes/Keynodes.hpp"
#include "agents/DeviceScheduleMonitorAgent.hpp"

using namespace smart_home;

SC_IMPLEMENT_MODULE(SmartHomeModule)

sc_result SmartHomeModule::InitializeImpl()
{
  if (!Keynodes::InitGlobal()) {
    return SC_RESULT_ERROR;
  }

  SC_AGENT_REGISTER(DeviceScheduleMonitorAgent)

  return SC_RESULT_OK;
}

sc_result SmartHomeModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(DeviceScheduleMonitorAgent)

  return SC_RESULT_OK;
}