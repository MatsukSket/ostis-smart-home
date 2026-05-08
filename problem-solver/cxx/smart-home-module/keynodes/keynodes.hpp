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
};

}