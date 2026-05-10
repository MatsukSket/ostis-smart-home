#include "SmartHomeModule.hpp"
#include "agents/DeviceScheduleMonitorAgent.hpp"
#include "agents/ScheduleTickerAgent.hpp"
#include "agents/MotionSensorAgent.hpp"
#include "agents/LightBulbAgent.hpp"

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
