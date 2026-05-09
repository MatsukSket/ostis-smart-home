#pragma once

#include <sc-memory/sc_keynodes.hpp>

namespace smart_home
{

class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const concept_effector{"concept_effector"};
  static inline ScKeynode const concept_state_on{"concept_state_on"};
  static inline ScKeynode const concept_state_off{"concept_state_off"};

  static inline ScKeynode const nrel_schedule{"nrel_schedule"};
  static inline ScKeynode const rrel_on_time{"rrel_on_time"};
  static inline ScKeynode const rrel_off_time{"rrel_off_time"};
  static inline ScKeynode const rrel_day{"rrel_day"};

  static inline ScKeynode const action_check_schedule{"action_check_schedule"};
  static inline ScKeynode const action_start_schedule_ticker{"action_start_schedule_ticker"};

  static inline ScKeynode const concept_light_bulb{"concept_light_bulb"};
  static inline ScKeynode const concept_motion_sensor{"concept_motion_sensor"};
  static inline ScKeynode const concept_action_detected{"concept_action_detected"};
  static inline ScKeynode const concept_action_not_detected{"concept_action_not_detected"};

  static inline ScKeynode const nrel_sensor{"nrel_sensor"};

  static inline ScKeynode const sensor_motion_bedroom{"sensor_motion_bedroom"};

  static inline ScKeynode const action_toggle_motion_sensor{"action_toggle_motion_sensor"};
  static inline ScKeynode const action_check_motion_sensor{"action_check_motion_sensor"};
};

}
