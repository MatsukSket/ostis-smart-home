#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/power_saving_agent.hpp"
#include "keynodes/Keynodes.hpp"
#include "test_utils.hpp"

using PowerSavingAgentTest = ScMemoryTest;

TEST_F(PowerSavingAgentTest, DisablesOnlyNonCriticalDevicesAndCreatesEvents)
{
  ScAgentContext & context = *m_ctx;
  context.SubscribeAgent<PowerSavingAgent>();

  LoadScsFile(context, "power_saving_agent_test.scs");

  ScAddr const home = ResolveKeynode(context, "power_saving_test_home");
  ScAddr const light = ResolveKeynode(context, "power_saving_test_light");
  ScAddr const computer = ResolveKeynode(context, "power_saving_test_computer");
  ScAddr const security = ResolveKeynode(context, "power_saving_test_security");
  ScAddr const actionNode = ResolveKeynode(context, "power_saving_action");
  ASSERT_TRUE(home.IsValid());
  ASSERT_TRUE(light.IsValid());
  ASSERT_TRUE(computer.IsValid());
  ASSERT_TRUE(security.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::power_saving_mode));
  EXPECT_EQ(GetRelationTarget(context, light, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, computer, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, security, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_on);

  EXPECT_TRUE(HasClass(context, light, SmartHomeKeynodes::disabled_device));
  EXPECT_TRUE(HasClass(context, computer, SmartHomeKeynodes::disabled_device));
  EXPECT_FALSE(HasClass(context, security, SmartHomeKeynodes::disabled_device));
  EXPECT_EQ(GetRelationTarget(context, light, SmartHomeKeynodes::nrel_previous_state), SmartHomeKeynodes::concept_on);
  EXPECT_EQ(GetRelationTarget(context, computer, SmartHomeKeynodes::nrel_previous_state), SmartHomeKeynodes::concept_on);
  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::power_saving_started, home), 1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          light,
          SmartHomeKeynodes::concept_off),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          computer,
          SmartHomeKeynodes::concept_off),
      1u);
  EXPECT_TRUE(CheckRelation(
      context,
      SmartHomeKeynodes::power_saving_mode,
      SmartHomeKeynodes::climate_control_agent_node,
      SmartHomeKeynodes::mode_affects_agent));

  context.UnsubscribeAgent<PowerSavingAgent>();
}
