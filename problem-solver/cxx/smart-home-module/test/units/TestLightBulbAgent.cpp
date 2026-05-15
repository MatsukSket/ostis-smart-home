#include "TestUtils.hpp"

TEST_F(SmartHomeTest, LightBulbTurnsOnWhenMotionDetected)
{
  ScAgentContext context;
  LoadTestStructure(context, "motion_sensor_initial.scs");

  context.SubscribeAgent<LightBulbAgent>();

  LoadTestStructure(context, "motion_detected.scs");

  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();

  sleep(2);

  context.UnsubscribeAgent<LightBulbAgent>();

  ScAddr const sensor = Keynodes::sensor_motion_bedroom;
  ScAddr const lamp = [&]() -> ScAddr
  {
    ScIterator5Ptr it = context.CreateIterator5(
        ScType::ConstNode,
        ScType::ConstCommonArc,
        sensor,
        ScType::ConstPermPosArc,
        Keynodes::nrel_sensor);
    while (it->Next())
    {
      ScAddr const candidate = it->Get(0);
      if (context.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
        return candidate;
    }
    return ScAddr::Empty;
  }();

  ASSERT_TRUE(lamp.IsValid());
  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_on, lamp, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_off, lamp, ScType::ConstPermPosArc));
}

TEST_F(SmartHomeTest, LightBulbTurnsOffWhenMotionNotDetected)
{
  ScAgentContext context;
  LoadTestStructure(context, "motion_sensor_initial.scs");
  LoadTestStructure(context, "motion_detected.scs");

  context.SubscribeAgent<LightBulbAgent>();

  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
  sleep(1);

  ScAddr const sensor = Keynodes::sensor_motion_bedroom;

  ScIterator3Ptr it = context.CreateIterator3(
      Keynodes::concept_action_detected, ScType::ConstPermPosArc, sensor);
  while (it->Next())
    context.EraseElement(it->Get(1));

  context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_action_not_detected, sensor);
  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();

  sleep(2);

  context.UnsubscribeAgent<LightBulbAgent>();

  ScAddr const lamp = [&]() -> ScAddr
  {
    ScIterator5Ptr it5 = context.CreateIterator5(
        ScType::ConstNode,
        ScType::ConstCommonArc,
        sensor,
        ScType::ConstPermPosArc,
        Keynodes::nrel_sensor);
    while (it5->Next())
    {
      ScAddr const candidate = it5->Get(0);
      if (context.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
        return candidate;
    }
    return ScAddr::Empty;
  }();

  ASSERT_TRUE(lamp.IsValid());
  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_off, lamp, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_on, lamp, ScType::ConstPermPosArc));
}

TEST_F(SmartHomeTest, LightBulbNotFoundWhenNoSensorRelation)
{
  ScAgentContext context;

  ScAddr const sensor = context.GenerateNode(ScType::ConstNode);
  context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_motion_sensor, sensor);
  context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_action_detected, sensor);

  context.SubscribeAgent<LightBulbAgent>();
  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
  sleep(2);
  context.UnsubscribeAgent<LightBulbAgent>();

  SUCCEED();
}

TEST_F(SmartHomeTest, LampStateRemainsOnAfterRepeatTrigger)
{
  ScAgentContext context;
  LoadTestStructure(context, "motion_sensor_initial.scs");
  LoadTestStructure(context, "motion_detected.scs");

  context.SubscribeAgent<LightBulbAgent>();

  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
  sleep(1);
  context.GenerateAction(Keynodes::action_check_motion_sensor).Initiate();
  sleep(1);

  context.UnsubscribeAgent<LightBulbAgent>();

  ScAddr const sensor = Keynodes::sensor_motion_bedroom;
  ScAddr const lamp = [&]() -> ScAddr
  {
    ScIterator5Ptr it = context.CreateIterator5(
        ScType::ConstNode,
        ScType::ConstCommonArc,
        sensor,
        ScType::ConstPermPosArc,
        Keynodes::nrel_sensor);
    while (it->Next())
    {
      ScAddr const candidate = it->Get(0);
      if (context.CheckConnector(Keynodes::concept_light_bulb, candidate, ScType::ConstPermPosArc))
        return candidate;
    }
    return ScAddr::Empty;
  }();

  ASSERT_TRUE(lamp.IsValid());
  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_on, lamp, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_off, lamp, ScType::ConstPermPosArc));
}
