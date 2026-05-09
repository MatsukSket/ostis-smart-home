#include "LampMotionControlAgent.hpp"
#include <sc-memory/sc_event_subscription.hpp>

using namespace smart_home;

static void HandleSensorChange(
    ScMemoryContext & ctx,
    ScAddr const & sensorAddr,
    ScAddr const & targetState,
    ScAddr const & oppositeState)
{
  ScIterator3Ptr motionIt = ctx.CreateIterator3(
      Keynodes::concept_motion_sensor,
      ScType::ConstPermPosArc,
      sensorAddr);

  if (!motionIt->Next())
    return;

  ScIterator3Ptr lampIt = ctx.CreateIterator3(
      Keynodes::concept_light_bulb,
      ScType::ConstPermPosArc,
      ScType::ConstNode);

  while (lampIt->Next())
  {
    ScAddr lampAddr = lampIt->Get(2);

    ScIterator5Ptr sensorRelationIt = ctx.CreateIterator5(
        lampAddr,
        ScType::ConstCommonArc,
        sensorAddr,
        ScType::ConstPermPosArc,
        Keynodes::nrel_sensor);

    if (sensorRelationIt->Next())
    {
      ScIterator3Ptr checkIt = ctx.CreateIterator3(
          targetState,
          ScType::ConstPermPosArc,
          lampAddr);

      if (!checkIt->Next())
      {
        ScIterator3Ptr oldIt = ctx.CreateIterator3(
            oppositeState,
            ScType::ConstPermPosArc,
            lampAddr);
        while (oldIt->Next())
          ctx.EraseElement(oldIt->Get(1));

        ctx.GenerateConnector(ScType::ConstPermPosArc, targetState, lampAddr);
      }
    }
  }
}

ScAddr LampMotionControlAgent::GetEventSubscriptionElement() const
{
  return Keynodes::concept_action_detected;
}

ScEventType LampMotionControlAgent::GetEventType() const
{
  return ScEventType::AddOutputEdge;
}

ScResult LampMotionControlAgent::DoProgram(ScElementaryEvent const & event, ScAction & action)
{
  ScAddr const sensorAddr = event.GetArcTargetElement();
  HandleSensorChange(m_context, sensorAddr, Keynodes::concept_state_on, Keynodes::concept_state_off);
  return action.FinishSuccessfully();
}

ScAddr LampMotionControlAgentOff::GetEventSubscriptionElement() const
{
  return Keynodes::concept_action_not_detected;
}

ScEventType LampMotionControlAgentOff::GetEventType() const
{
  return ScEventType::AddOutputEdge;
}

ScResult LampMotionControlAgentOff::DoProgram(ScElementaryEvent const & event, ScAction & action)
{
  ScAddr const sensorAddr = event.GetArcTargetElement();
  HandleSensorChange(m_context, sensorAddr, Keynodes::concept_state_off, Keynodes::concept_state_on);
  return action.FinishSuccessfully();
}
