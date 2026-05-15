#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/device_state_agent.hpp"
#include "agents/humidity_control_agent.hpp"
#include "agents/movement-lamp/LightBulbAgent.hpp"
#include "keynodes/Keynodes.hpp"
#include "test_utils.hpp"

using AutomationPrioritiesTest = ScMemoryTest;

TEST_F(AutomationPrioritiesTest, HardOffBlocksHumidityAutomation)
{
  ScAgentContext & context = *m_ctx;
  context.SubscribeAgent<DeviceStateAgent>();
  context.SubscribeAgent<HumidityControlAgent>();

  LoadScsFile(context, "humidity_control_hard_off_test.scs");

  ScAddr const device = ResolveKeynode(context, "hard_off_test_humidifier");
  ScAddr const actionNode = ResolveKeynode(context, "hard_off_test_humidity_control_action");

  ASSERT_TRUE(device.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);
  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(
      GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state),
      SmartHomeKeynodes::concept_state_hard_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          device,
          SmartHomeKeynodes::concept_on),
      0u);

  context.UnsubscribeAgent<HumidityControlAgent>();
  context.UnsubscribeAgent<DeviceStateAgent>();
}

TEST_F(AutomationPrioritiesTest, ScheduleBlocksHumidityAutomation)
{
  ScAgentContext & context = *m_ctx;
  context.SubscribeAgent<DeviceStateAgent>();
  context.SubscribeAgent<HumidityControlAgent>();

  LoadScsFile(context, "humidity_control_schedule_test.scs");

  ScAddr const device = ResolveKeynode(context, "schedule_test_humidifier");
  ScAddr const actionNode = ResolveKeynode(context, "schedule_test_humidity_control_action");

  ASSERT_TRUE(device.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);
  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          device,
          SmartHomeKeynodes::concept_on),
      0u);

  context.UnsubscribeAgent<HumidityControlAgent>();
  context.UnsubscribeAgent<DeviceStateAgent>();
}

TEST_F(AutomationPrioritiesTest, HardOffBlocksLightAutomation)
{
  ScAgentContext & context = *m_ctx;
  context.SubscribeAgent<smart_home::LightBulbAgent>();

  LoadScsFile(context, "motion_sensor_hard_off_test.scs");
  context.GenerateAction(SmartHomeKeynodes::action_check_motion_sensor).Initiate();
  sleep(1);

  context.UnsubscribeAgent<smart_home::LightBulbAgent>();

  ScAddr const lamp = ResolveKeynode(context, "lamp_bedroom");
  ASSERT_TRUE(lamp.IsValid());
  EXPECT_FALSE(context.CheckConnector(SmartHomeKeynodes::concept_state_on, lamp, ScType::ConstPermPosArc));
  EXPECT_TRUE(context.CheckConnector(SmartHomeKeynodes::concept_state_off, lamp, ScType::ConstPermPosArc));
}
