#include "SmartHomeModule.hpp"
#include "agents/away_mode_agent.hpp"
#include "agents/device_state_agent.hpp"
#include "agents/humidity_control_agent.hpp"
#include "agents/power_saving_agent.hpp"
#include "agents/temperature_control_agent.hpp"
#include "agents/schedule-agent/DeviceScheduleMonitorAgent.hpp"
#include "agents/schedule-agent/ScheduleTickerAgent.hpp"
#include "agents/movement-lamp/MotionSensorAgent.hpp"
#include "agents/movement-lamp/LightBulbAgent.hpp"

using namespace smart_home;

SC_MODULE_REGISTER(SmartHomeModule)
    ->Agent<AwayModeAgent>()
    ->Agent<DeviceStateAgent>()
    ->Agent<HumidityControlAgent>()
    ->Agent<PowerSavingAgent>()
    ->Agent<TemperatureControlAgent>()
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
