#pragma once

#include <vector>

#include <sc-memory/sc_agent_context.hpp>

#include "keynodes/Keynodes.hpp"

namespace smart_home::automation
{

inline bool HasClass(ScMemoryContext & context, ScAddr const & element, ScAddr const & classAddr)
{
  return element.IsValid() && context.CheckConnector(classAddr, element, ScType::ConstPermPosArc);
}

inline ScAddr GetRelationTarget(ScMemoryContext & context, ScAddr const & source, ScAddr const & relation)
{
  if (!source.IsValid())
    return ScAddr::Empty;

  ScIterator5Ptr const iterator =
      context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  return iterator->Next() ? iterator->Get(2) : ScAddr::Empty;
}

inline std::vector<ScAddr> GetRelationTargets(ScMemoryContext & context, ScAddr const & source, ScAddr const & relation)
{
  std::vector<ScAddr> result;

  if (!source.IsValid())
    return result;

  ScIterator5Ptr const iterator =
      context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  while (iterator->Next())
    result.push_back(iterator->Get(2));

  return result;
}

inline bool HasScheduleControl(ScMemoryContext & context, ScAddr const & device)
{
  return GetRelationTarget(context, device, Keynodes::nrel_schedule).IsValid();
}

inline bool IsDeviceHardOff(ScMemoryContext & context, ScAddr const & device)
{
  if (!device.IsValid())
    return false;

  if (HasClass(context, device, Keynodes::concept_state_hard_off))
    return true;

  return GetRelationTarget(context, device, Keynodes::nrel_current_state) == Keynodes::concept_state_hard_off;
}

inline bool IsAutomationBlocked(ScMemoryContext & context, ScAddr const & device)
{
  return IsDeviceHardOff(context, device) || HasScheduleControl(context, device);
}

}  // namespace smart_home::automation
