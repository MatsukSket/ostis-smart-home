#include <gtest/gtest.h>

#include <sc-memory/test/sc_test.hpp>

#include "agents/device_state_agent.hpp"
#include "keynodes/keynodes.hpp"
#include "test_utils.hpp"

using DeviceStateAgentTest = ScMemoryTest;

TEST_F(DeviceStateAgentTest, ChangesDeviceStateAndCreatesKnowledgeBaseEvent)
{
  ScAgentContext & context = *m_ctx;
  context.SubscribeAgent<DeviceStateAgent>();

  LoadScsFile(context, "device_state_agent_test.scs");

  ScAddr const device = ResolveKeynode(context, "test_humidifier");
  ASSERT_TRUE(device.IsValid());

  ScAddr const actionNode = ResolveKeynode(context, "test_device_state_action");
  ASSERT_TRUE(actionNode.IsValid());

  ScAction action = context.ConvertToAction(actionNode);

  ASSERT_TRUE(action.InitiateAndWait(5000));
  ASSERT_TRUE(action.IsFinishedSuccessfully());

  EXPECT_EQ(GetRelationTarget(context, device, SmartHomeKeynodes::nrel_current_state), SmartHomeKeynodes::concept_on);
  EXPECT_EQ(
      CountDeviceEvents(
          context,
          SmartHomeKeynodes::concept_device_enabled_event,
          device,
          SmartHomeKeynodes::concept_on),
      1u);

  context.UnsubscribeAgent<DeviceStateAgent>();
}
