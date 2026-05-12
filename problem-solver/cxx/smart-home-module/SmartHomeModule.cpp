#include "SmartHomeModule.hpp"
#include "agents/schedule-humanizer/DeviceScheduleMonitorAgent.hpp"
#include "agents/schedule-humanizer/ScheduleTickerAgent.hpp"
#include "agents/movement-lamp/MotionSensorAgent.hpp"
#include "agents/movement-lamp/LightBulbAgent.hpp"

using namespace smart_home;

SC_MODULE_REGISTER(SmartHomeModule)
    ->Agent<DeviceScheduleMonitorAgent>()
    ->Agent<LightBulbAgent>();

SmartHomeModule::SmartHomeModule()
{
  ScheduleTickerAgent::Start();
  MotionSensorAgent::Start();
}

SmartHomeModule::~SmartHomeModule()
{
  ScheduleTickerAgent::Stop();
  MotionSensorAgent::Stop();
}
