#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/away_mode_agent.hpp"
#include "agents/power_saving_agent.hpp"
#include "keynodes/keynodes.hpp"
#include "test_utils.hpp"

using AwayModeAgentTest = ScMemoryTest;

namespace
{
void SubscribeAgents(ScAgentContext & context)
{
  context.SubscribeAgent<AwayModeAgent>();
  context.SubscribeAgent<PowerSavingAgent>();
}

void UnsubscribeAgents(ScAgentContext & context)
{
  context.UnsubscribeAgent<PowerSavingAgent>();
  context.UnsubscribeAgent<AwayModeAgent>();
}
}  // namespace

TEST_F(AwayModeAgentTest, ActivationChangesGlobalStateAndStartsPowerSaving)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "away_mode_activation_test.scs");

  ScAddr const home = ResolveKeynode(context, "away_activation_test_home");
  ScAddr const light = ResolveKeynode(context, "away_activation_light");
  ScAddr const tv = ResolveKeynode(context, "away_activation_tv");
  ScAddr const router = ResolveKeynode(context, "away_activation_router");
  ScAddr const actionNode = ResolveKeynode(context, "away_activation_action");
  ASSERT_TRUE(home.IsValid());
  ASSERT_TRUE(light.IsValid());
  ASSERT_TRUE(tv.IsValid());
  ASSERT_TRUE(router.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::home_empty));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::away_mode_active));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::security_mode_active));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::power_saving_mode));
  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::home_present));

  EXPECT_EQ(GetRelationTarget(context, light, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, tv, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, router, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_on);
  EXPECT_TRUE(HasClass(context, light, SmartHomeKeynodes::disabled_device));
  EXPECT_TRUE(HasClass(context, tv, SmartHomeKeynodes::disabled_device));
  EXPECT_FALSE(HasClass(context, router, SmartHomeKeynodes::disabled_device));

  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::away_mode_activated, home), 1u);
  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::power_saving_started, home), 1u);
  EXPECT_TRUE(CheckRelation(
      context,
      SmartHomeKeynodes::away_mode_active,
      SmartHomeKeynodes::climate_control_agent_node,
      SmartHomeKeynodes::mode_affects_agent));

  UnsubscribeAgents(context);
}

TEST_F(AwayModeAgentTest, UnconfirmedPresenceEventDoesNotDisableAwayMode)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "away_mode_false_trigger_test.scs");

  ScAddr const home = ResolveKeynode(context, "away_false_trigger_test_home");
  ScAddr const actionNode = ResolveKeynode(context, "away_false_action");
  ASSERT_TRUE(home.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::home_empty));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::away_mode_active));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::security_mode_active));
  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::home_present));
  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::unverified_presence_event, home), 1u);
  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::person_returned_home, home), 0u);

  UnsubscribeAgents(context);
}

TEST_F(AwayModeAgentTest, ConfirmedPresenceEventDisablesAwayModeAndRestoresDevices)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "away_mode_return_test.scs");

  ScAddr const home = ResolveKeynode(context, "away_return_test_home");
  ScAddr const light = ResolveKeynode(context, "away_return_light");
  ScAddr const router = ResolveKeynode(context, "away_return_router");
  ScAddr const actionNode = ResolveKeynode(context, "away_return_action");
  ASSERT_TRUE(home.IsValid());
  ASSERT_TRUE(light.IsValid());
  ASSERT_TRUE(router.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::home_empty));
  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::away_mode_active));
  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::security_mode_active));
  EXPECT_FALSE(HasClass(context, home, SmartHomeKeynodes::power_saving_mode));
  EXPECT_TRUE(HasClass(context, home, SmartHomeKeynodes::home_present));

  EXPECT_EQ(GetRelationTarget(context, light, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_on);
  EXPECT_EQ(GetRelationTarget(context, router, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_on);
  EXPECT_FALSE(HasClass(context, light, SmartHomeKeynodes::disabled_device));

  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::person_returned_home, home), 1u);
  EXPECT_EQ(CountHomeEvents(context, SmartHomeKeynodes::power_saving_finished, home), 2u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          light,
          SmartHomeKeynodes::concept_on),
      1u);

  UnsubscribeAgents(context);
}
