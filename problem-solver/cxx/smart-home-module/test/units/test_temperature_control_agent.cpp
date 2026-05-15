#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/device_state_agent.hpp"
#include "agents/temperature_control_agent.hpp"
#include "keynodes/Keynodes.hpp"
#include "test_utils.hpp"

using TemperatureControlAgentTest = ScMemoryTest;

namespace
{
void SubscribeAgents(ScAgentContext & context)
{
  context.SubscribeAgent<DeviceStateAgent>();
  context.SubscribeAgent<TemperatureControlAgent>();
}

void UnsubscribeAgents(ScAgentContext & context)
{
  context.UnsubscribeAgent<TemperatureControlAgent>();
  context.UnsubscribeAgent<DeviceStateAgent>();
}
}  // namespace

TEST_F(TemperatureControlAgentTest, HighTemperatureTurnsConditionerOnAndThenOffAfterNormalization)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "temperature_control_high_test.scs");

  ScAddr const sensor = ResolveKeynode(context, "high_temp_sensor");
  ScAddr const conditioner = ResolveKeynode(context, "high_temp_conditioner");
  ScAddr const heater = ResolveKeynode(context, "high_temp_heater");
  ScAddr const normalizedMeasurement = ResolveKeynode(context, "high_temp_normal_measurement");
  ScAddr const actionNode = ResolveKeynode(context, "high_temp_control_action");

  ASSERT_TRUE(sensor.IsValid());
  ASSERT_TRUE(conditioner.IsValid());
  ASSERT_TRUE(heater.IsValid());
  ASSERT_TRUE(normalizedMeasurement.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, sensor, SmartHomeKeynodes::nrel_current_measurement), normalizedMeasurement);
  EXPECT_EQ(GetRelationTarget(context, conditioner, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, heater, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          conditioner,
          SmartHomeKeynodes::concept_on),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          conditioner,
          SmartHomeKeynodes::concept_off),
      1u);

  UnsubscribeAgents(context);
}

TEST_F(TemperatureControlAgentTest, LowTemperatureTurnsHeaterOnAndThenOffAfterNormalization)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "temperature_control_low_test.scs");

  ScAddr const sensor = ResolveKeynode(context, "low_temp_sensor");
  ScAddr const conditioner = ResolveKeynode(context, "low_temp_conditioner");
  ScAddr const heater = ResolveKeynode(context, "low_temp_heater");
  ScAddr const normalizedMeasurement = ResolveKeynode(context, "low_temp_normal_measurement");
  ScAddr const actionNode = ResolveKeynode(context, "low_temp_control_action");

  ASSERT_TRUE(sensor.IsValid());
  ASSERT_TRUE(conditioner.IsValid());
  ASSERT_TRUE(heater.IsValid());
  ASSERT_TRUE(normalizedMeasurement.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, sensor, SmartHomeKeynodes::nrel_current_measurement), normalizedMeasurement);
  EXPECT_EQ(GetRelationTarget(context, conditioner, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, heater, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          heater,
          SmartHomeKeynodes::concept_on),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_disabled_event,
          heater,
          SmartHomeKeynodes::concept_off),
      1u);

  UnsubscribeAgents(context);
}

TEST_F(TemperatureControlAgentTest, NormalTemperatureKeepsBothDevicesOff)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "temperature_control_normal_test.scs");

  ScAddr const conditioner = ResolveKeynode(context, "normal_temp_conditioner");
  ScAddr const heater = ResolveKeynode(context, "normal_temp_heater");
  ScAddr const actionNode = ResolveKeynode(context, "normal_temp_control_action");

  ASSERT_TRUE(conditioner.IsValid());
  ASSERT_TRUE(heater.IsValid());
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, conditioner, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, heater, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);

  UnsubscribeAgents(context);
}

TEST_F(TemperatureControlAgentTest, SensorCanSwitchFromConditionerToHeaterAcrossActions)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "temperature_control_switch_test.scs");

  ScAddr const sensor = ResolveKeynode(context, "switch_temp_sensor");
  ScAddr const conditioner = ResolveKeynode(context, "switch_temp_conditioner");
  ScAddr const heater = ResolveKeynode(context, "switch_temp_heater");
  ScAddr const highActionNode = ResolveKeynode(context, "switch_high_temp_control_action");
  ScAddr const lowMeasurement = ResolveKeynode(context, "switch_low_temp_measurement");
  ScAddr const lowNormalizedMeasurement = ResolveKeynode(context, "switch_low_normal_measurement");

  ASSERT_TRUE(sensor.IsValid());
  ASSERT_TRUE(conditioner.IsValid());
  ASSERT_TRUE(heater.IsValid());
  ASSERT_TRUE(highActionNode.IsValid());
  ASSERT_TRUE(lowMeasurement.IsValid());
  ASSERT_TRUE(lowNormalizedMeasurement.IsValid());

  ScAction highAction = context.ConvertToAction(highActionNode);
  ASSERT_TRUE(highAction.InitiateAndWait(5000));
  ASSERT_TRUE(highAction.IsFinishedSuccessfully());

  SetRelation(context, sensor, lowMeasurement, SmartHomeKeynodes::nrel_current_measurement);
  SetRelation(context, sensor, lowNormalizedMeasurement, SmartHomeKeynodes::nrel_normalized_measurement);

  ScAction lowAction = context.GenerateAction(SmartHomeKeynodes::action_control_room_temperature);
  lowAction.SetArguments(sensor);
  ASSERT_TRUE(lowAction.InitiateAndWait(5000));
  ASSERT_TRUE(lowAction.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, conditioner, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(GetRelationTarget(context, heater, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_off);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          conditioner,
          SmartHomeKeynodes::concept_on),
      1u);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          heater,
          SmartHomeKeynodes::concept_on),
      1u);

  UnsubscribeAgents(context);
}

TEST_F(TemperatureControlAgentTest, MissingControlledPairFails)
{
  ScAgentContext & context = *m_ctx;
  SubscribeAgents(context);

  LoadScsFile(context, "temperature_control_invalid_test.scs");

  ScAddr const actionNode = ResolveKeynode(context, "invalid_temp_control_action");
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);
  ASSERT_TRUE(action.InitiateAndWait(5000));
  EXPECT_FALSE(action.IsFinishedSuccessfully());

  UnsubscribeAgents(context);
}
