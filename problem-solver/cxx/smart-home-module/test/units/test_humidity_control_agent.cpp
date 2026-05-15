#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/device_state_agent.hpp"
#include "agents/humidity_control_agent.hpp"
#include "keynodes/Keynodes.hpp"
#include "test_utils.hpp"

using HumidityControlAgentTest = ScMemoryTest;

namespace
{
void SubscribeAgents(ScAgentContext & context)
{
  context.SubscribeAgent<DeviceStateAgent>();
  context.SubscribeAgent<HumidityControlAgent>();
}

void UnsubscribeAgents(ScAgentContext & context)
{
  context.UnsubscribeAgent<HumidityControlAgent>();
  context.UnsubscribeAgent<DeviceStateAgent>();
}
}  // namespace

TEST_F(HumidityControlAgentTest, LowHumidityTurnsHumidifierOnAndThenOffAfterNormalization)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "humidity_control_low_test.scs");

  ScAddr const sensor = ResolveKeynode(context, "low_test_hygrometer");
  ScAddr const device = ResolveKeynode(context, "low_test_humidifier");
  ScAddr const normalizedMeasurement = ResolveKeynode(context, "low_test_normal_measurement");
  ScAddr const actionNode = ResolveKeynode(context, "low_test_humidity_control_action");
  ASSERT_TRUE(sensor.IsValid());
  ASSERT_TRUE(device.IsValid());
  ASSERT_TRUE(normalizedMeasurement.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(
      GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state),
      SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      GetRelationTarget(context, sensor, SmartHomeKeynodes::nrel_current_measurement),
      normalizedMeasurement);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          device,
          SmartHomeKeynodes::concept_on),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          device,
          SmartHomeKeynodes::concept_off),
      1u);

  UnsubscribeAgents(context);
}

TEST_F(HumidityControlAgentTest, HighHumidityTurnsDehumidifierOnAndThenOffAfterNormalization)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "humidity_control_high_test.scs");

  ScAddr const sensor = ResolveKeynode(context, "high_test_hygrometer");
  ScAddr const device = ResolveKeynode(context, "high_test_dehumidifier");
  ScAddr const normalizedMeasurement = ResolveKeynode(context, "high_test_normal_measurement");
  ScAddr const actionNode = ResolveKeynode(context, "high_test_humidity_control_action");
  ASSERT_TRUE(sensor.IsValid());
  ASSERT_TRUE(device.IsValid());
  ASSERT_TRUE(normalizedMeasurement.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(
      GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state),
      SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      GetRelationTarget(context, sensor, SmartHomeKeynodes::nrel_current_measurement),
      normalizedMeasurement);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          device,
          SmartHomeKeynodes::concept_on),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          device,
          SmartHomeKeynodes::concept_off),
      1u);

  UnsubscribeAgents(context);
}

TEST_F(HumidityControlAgentTest, NormalHumidityKeepsDeviceOff)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "humidity_control_normal_test.scs");

  ScAddr const device = ResolveKeynode(context, "normal_test_humidifier");
  ScAddr const actionNode = ResolveKeynode(context, "normal_test_humidity_control_action");
  ASSERT_TRUE(device.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(
      GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state),
      SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          device,
          SmartHomeKeynodes::concept_off),
      1u);

  UnsubscribeAgents(context);
}
