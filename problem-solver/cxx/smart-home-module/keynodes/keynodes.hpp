#pragma once

#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/sc_addr.hpp>

namespace smart_home
{

class Keynodes : public ScKeynodes
{
public:
  static ScAddr concept_effector;
  static ScAddr concept_state_on;
  static ScAddr concept_state_off;

  static ScAddr nrel_schedule;
  static ScAddr rrel_on_time;
  static ScAddr rrel_off_time;
  static ScAddr rrel_day;

  static ScAddr action_check_schedule;
};

} 