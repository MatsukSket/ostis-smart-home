#include "SmartHomeModule.hpp"
#include "agents/DeviceScheduleMonitorAgent.hpp"
#include "agents/ScheduleTickerAgent.hpp"
#include "agents/LampMotionControlAgent.hpp"
#include "agents/MotionSensorAgent.hpp"

using namespace smart_home;

SC_MODULE_REGISTER(SmartHomeModule)
    ->Agent<DeviceScheduleMonitorAgent>()
    ->Agent<LampMotionControlAgent>()
    ->Agent<MotionSensorAgent>();

SmartHomeModule::SmartHomeModule()
{
  ScheduleTickerAgent::Start();
}

SmartHomeModule::~SmartHomeModule()
{
  ScheduleTickerAgent::Stop();
}
