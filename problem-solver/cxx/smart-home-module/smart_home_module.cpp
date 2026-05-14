#include "smart_home_module.hpp"

#include "agents/away_mode_agent.hpp"
#include "agents/device_state_agent.hpp"
#include "agents/humidity_control_agent.hpp"
#include "agents/power_saving_agent.hpp"

SC_MODULE_REGISTER(SmartHomeModule)
    ->Agent<DeviceStateAgent>()
    ->Agent<HumidityControlAgent>()
    ->Agent<AwayModeAgent>()
    ->Agent<PowerSavingAgent>();
