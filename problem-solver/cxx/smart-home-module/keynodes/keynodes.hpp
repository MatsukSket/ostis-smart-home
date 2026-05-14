#pragma once

#include <sc-memory/sc_keynodes.hpp>

class SmartHomeKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_change_device_state{"action_change_device_state", ScType::ConstNodeClass};
  static inline ScKeynode const action_control_room_humidity{
      "action_control_room_humidity",
      ScType::ConstNodeClass};
  static inline ScKeynode const action_control_away_mode{"action_control_away_mode", ScType::ConstNodeClass};
  static inline ScKeynode const action_control_power_saving{"action_control_power_saving", ScType::ConstNodeClass};

  static inline ScKeynode const concept_smart_home{"concept_smart_home", ScType::ConstNodeClass};
  static inline ScKeynode const concept_device{"concept_device", ScType::ConstNodeClass};
  static inline ScKeynode const concept_humidifier{"concept_humidifier", ScType::ConstNodeClass};
  static inline ScKeynode const concept_dehumidifier{"concept_dehumidifier", ScType::ConstNodeClass};
  static inline ScKeynode const concept_sensor{"concept_sensor", ScType::ConstNodeClass};
  static inline ScKeynode const concept_hygrometer{"concept_hygrometer", ScType::ConstNodeClass};
  static inline ScKeynode const concept_motion_sensor{"concept_motion_sensor", ScType::ConstNodeClass};
  static inline ScKeynode const concept_door_sensor{"concept_door_sensor", ScType::ConstNodeClass};

  static inline ScKeynode const concept_state{"concept_state", ScType::ConstNodeClass};
  static inline ScKeynode const concept_on{"concept_on", ScType::ConstNodeClass};
  static inline ScKeynode const concept_off{"concept_off", ScType::ConstNodeClass};
  static inline ScKeynode const home_empty{"home_empty", ScType::ConstNodeClass};
  static inline ScKeynode const away_mode_active{"away_mode_active", ScType::ConstNodeClass};
  static inline ScKeynode const security_mode_active{"security_mode_active", ScType::ConstNodeClass};
  static inline ScKeynode const power_saving_mode{"power_saving_mode", ScType::ConstNodeClass};
  static inline ScKeynode const home_present{"home_present", ScType::ConstNodeClass};

  static inline ScKeynode const concept_humidity_value{"concept_humidity_value", ScType::ConstNodeClass};
  static inline ScKeynode const concept_low_humidity{"concept_low_humidity", ScType::ConstNodeClass};
  static inline ScKeynode const concept_normal_humidity{"concept_normal_humidity", ScType::ConstNodeClass};
  static inline ScKeynode const concept_high_humidity{"concept_high_humidity", ScType::ConstNodeClass};

  static inline ScKeynode const concept_device_state_event{"concept_device_state_event", ScType::ConstNodeClass};
  static inline ScKeynode const concept_device_enabled_event{
      "concept_device_enabled_event",
      ScType::ConstNodeClass};
  static inline ScKeynode const concept_device_disabled_event{
      "concept_device_disabled_event",
      ScType::ConstNodeClass};
  static inline ScKeynode const away_mode_requested{"away_mode_requested", ScType::ConstNodeClass};
  static inline ScKeynode const away_mode_activated{"away_mode_activated", ScType::ConstNodeClass};
  static inline ScKeynode const motion_detected{"motion_detected", ScType::ConstNodeClass};
  static inline ScKeynode const door_opened{"door_opened", ScType::ConstNodeClass};
  static inline ScKeynode const confirmed_event{"confirmed_event", ScType::ConstNodeClass};
  static inline ScKeynode const unverified_presence_event{"unverified_presence_event", ScType::ConstNodeClass};
  static inline ScKeynode const person_returned_home{"person_returned_home", ScType::ConstNodeClass};
  static inline ScKeynode const alarm_triggered{"alarm_triggered", ScType::ConstNodeClass};
  static inline ScKeynode const power_saving_started{"power_saving_started", ScType::ConstNodeClass};
  static inline ScKeynode const power_saving_finished{"power_saving_finished", ScType::ConstNodeClass};

  static inline ScKeynode const critical_device{"critical_device", ScType::ConstNodeClass};
  static inline ScKeynode const disabled_device{"disabled_device", ScType::ConstNodeClass};
  static inline ScKeynode const smart_device{"smart_device", ScType::ConstNodeClass};
  static inline ScKeynode const lighting_system{"lighting_system", ScType::ConstNodeClass};
  static inline ScKeynode const multimedia_system{"multimedia_system", ScType::ConstNodeClass};
  static inline ScKeynode const climate_system{"climate_system", ScType::ConstNodeClass};
  static inline ScKeynode const power_saving_agent_node{"power_saving_agent_node", ScType::ConstNode};
  static inline ScKeynode const climate_control_agent_node{"climate_control_agent_node", ScType::ConstNode};
  static inline ScKeynode const security_agent_node{"security_agent_node", ScType::ConstNode};

  static inline ScKeynode const nrel_current_state{"nrel_current_state", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_current_measurement{"nrel_current_measurement", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_normalized_measurement{
      "nrel_normalized_measurement",
      ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_controls{"nrel_controls", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_event_device{"nrel_event_device", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_event_state{"nrel_event_state", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_event_home{"nrel_event_home", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_event_source{"nrel_event_source", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_previous_state{"nrel_previous_state", ScType::ConstNodeNonRole};
  static inline ScKeynode const device_can_be_disabled{"device_can_be_disabled", ScType::ConstNodeNonRole};
  static inline ScKeynode const device_is_critical{"device_is_critical", ScType::ConstNodeNonRole};
  static inline ScKeynode const mode_affects_agent{"mode_affects_agent", ScType::ConstNodeNonRole};
  static inline ScKeynode const event_triggers_action{"event_triggers_action", ScType::ConstNodeNonRole};
  static inline ScKeynode const sensor_generates_event{"sensor_generates_event", ScType::ConstNodeNonRole};
  static inline ScKeynode const agent_controls_device{"agent_controls_device", ScType::ConstNodeNonRole};

  static inline ScKeynode const rrel_value{"rrel_value", ScType::ConstNodeRole};
};
