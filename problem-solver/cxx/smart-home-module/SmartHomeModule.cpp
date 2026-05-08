#include "SmartHomeModule.hpp"
#include "agents/DeviceScheduleMonitorAgent.hpp"
#include "agents/ScheduleTickerAgent.hpp"

using namespace smart_home;

SC_MODULE_REGISTER(SmartHomeModule)
    ->Agent<DeviceScheduleMonitorAgent>();

SmartHomeModule::SmartHomeModule()
{
  ScheduleTickerAgent::Start();
}

SmartHomeModule::~SmartHomeModule()
{
  ScheduleTickerAgent::Stop();
}