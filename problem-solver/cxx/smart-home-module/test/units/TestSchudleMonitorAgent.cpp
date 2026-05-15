#include "TestUtils.hpp"
#include <agents/schedule-agent/DeviceScheduleMonitorAgent.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>



std::pair<std::string, std::string> GetSystemTimeAndDay()
{
  std::time_t t = std::time(nullptr);
  std::tm * now = std::localtime(&t);
  std::ostringstream timeStream;
  timeStream << std::setw(2) << std::setfill('0') << now->tm_hour 
             << ":" << std::setw(2) << std::setfill('0') << now->tm_min;
  const char * days[] = {"вс", "пн", "вт", "ср", "чт", "пт", "сб"};
  return {timeStream.str(), days[now->tm_wday]};
}

void AddTimeConditionToSchedule(ScAgentContext & context, const std::string & timeStr, ScAddr timeRelation)
{
  ScAddr tupleAddr = context.HelperResolveSystemIdtf("test_schedule_tuple");
  ScAddr linkAddr = context.CreateLink();
  context.SetLinkContent(linkAddr, timeStr);
  
  ScAddr arc = context.GenerateConnector(ScType::ConstPermPosArc, tupleAddr, linkAddr);
  context.GenerateConnector(ScType::ConstPermPosArc, timeRelation, arc);
}



TEST_F(SmartHomeTest, DeviceTurnsOnWhenTimeMatches)
{
  ScAgentContext context;
  LoadTestStructure(context, "device_schedule_initial.scs");

  auto [currentTime, currentDay] = GetSystemTimeAndDay();
  AddTimeConditionToSchedule(context, currentTime, Keynodes::rrel_on_time);
  AddTimeConditionToSchedule(context, currentDay, Keynodes::rrel_day);

  context.SubscribeAgent<DeviceScheduleMonitorAgent>();
  context.GenerateAction(Keynodes::action_check_schedule).Initiate();

  sleep(2);

  context.UnsubscribeAgent<DeviceScheduleMonitorAgent>();

  ScAddr const device = context.HelperResolveSystemIdtf("humidifier_test");

  ASSERT_TRUE(device.IsValid());
  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_on, device, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_off, device, ScType::ConstPermPosArc));
}

TEST_F(SmartHomeTest, DeviceTurnsOffWhenTimeMatches)
{
  ScAgentContext context;
  LoadTestStructure(context, "device_schedule_initial.scs");

  ScAddr const device = context.HelperResolveSystemIdtf("humidifier_test");

  ScIterator3Ptr it = context.CreateIterator3(Keynodes::concept_state_off, ScType::ConstPermPosArc, device);
  while (it->Next()) context.EraseElement(it->Get(1));
  context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_state_on, device);

  auto [currentTime, currentDay] = GetSystemTimeAndDay();
  AddTimeConditionToSchedule(context, currentTime, Keynodes::rrel_off_time);
  AddTimeConditionToSchedule(context, currentDay, Keynodes::rrel_day);

  context.SubscribeAgent<DeviceScheduleMonitorAgent>();
  context.GenerateAction(Keynodes::action_check_schedule).Initiate();

  sleep(2);

  context.UnsubscribeAgent<DeviceScheduleMonitorAgent>();

  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_off, device, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_on, device, ScType::ConstPermPosArc));
}

TEST_F(SmartHomeTest, DeviceIgnoresIfWrongDay)
{
  ScAgentContext context;
  LoadTestStructure(context, "device_schedule_initial.scs");

  auto [currentTime, currentDay] = GetSystemTimeAndDay();
  
  std::string wrongDay = (currentDay == "пн") ? "вт" : "пн";

  AddTimeConditionToSchedule(context, currentTime, Keynodes::rrel_on_time);
  AddTimeConditionToSchedule(context, wrongDay, Keynodes::rrel_day); 

  context.SubscribeAgent<DeviceScheduleMonitorAgent>();
  context.GenerateAction(Keynodes::action_check_schedule).Initiate();

  sleep(2);

  context.UnsubscribeAgent<DeviceScheduleMonitorAgent>();

  ScAddr const device = context.HelperResolveSystemIdtf("humidifier_test");

  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_off, device, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_on, device, ScType::ConstPermPosArc));
}

TEST_F(SmartHomeTest, DeviceIgnoresScheduleIfHardOff)
{
  ScAgentContext context;
  LoadTestStructure(context, "device_schedule_initial.scs");

  ScAddr const device = context.HelperResolveSystemIdtf("humidifier_test");

  context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::concept_state_hard_off, device);

  auto [currentTime, currentDay] = GetSystemTimeAndDay();
  AddTimeConditionToSchedule(context, currentTime, Keynodes::rrel_on_time);
  AddTimeConditionToSchedule(context, currentDay, Keynodes::rrel_day);

  context.SubscribeAgent<DeviceScheduleMonitorAgent>();
  context.GenerateAction(Keynodes::action_check_schedule).Initiate();

  sleep(2);

  context.UnsubscribeAgent<DeviceScheduleMonitorAgent>();

  EXPECT_TRUE(context.CheckConnector(Keynodes::concept_state_off, device, ScType::ConstPermPosArc));
  EXPECT_FALSE(context.CheckConnector(Keynodes::concept_state_on, device, ScType::ConstPermPosArc));
}