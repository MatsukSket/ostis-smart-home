#include "test_utils.hpp"

#include <sc-builder/scs_loader.hpp>

#include "keynodes/keynodes.hpp"

std::string const SMART_HOME_TEST_FILES_DIR_PATH = SMART_HOME_TEST_STRUCTURES_DIR;

void LoadScsFile(ScAgentContext & context, std::string const & fileName)
{
  ScsLoader loader;
  loader.loadScsFile(context, SMART_HOME_TEST_FILES_DIR_PATH + fileName);
}

ScAddr ResolveKeynode(ScAgentContext & context, std::string const & systemIdentifier)
{
  return context.SearchElementBySystemIdentifier(systemIdentifier);
}

void SetRelation(ScAgentContext & context, ScAddr const & source, ScAddr const & target, ScAddr const & relation)
{
  ScAddr const relationPair = context.GenerateConnector(ScType::ConstCommonArc, source, target);
  context.GenerateConnector(ScType::ConstPermPosArc, relation, relationPair);
}

ScAddr GetRelationTarget(ScAgentContext & context, ScAddr const & source, ScAddr const & relation)
{
  ScIterator5Ptr const iterator =
      context.CreateIterator5(source, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  return iterator->Next() ? iterator->Get(2) : ScAddr::Empty;
}

bool CheckRelation(
    ScAgentContext & context,
    ScAddr const & source,
    ScAddr const & target,
    ScAddr const & relation)
{
  ScIterator5Ptr const iterator = context.CreateIterator5(source, ScType::Unknown, target, ScType::Unknown, relation);
  return iterator->Next();
}

bool HasClass(ScAgentContext & context, ScAddr const & element, ScAddr const & classAddr)
{
  return context.CheckConnector(classAddr, element, ScType::ConstPermPosArc);
}

size_t CountClassInstances(ScAgentContext & context, ScAddr const & classAddr)
{
  size_t result = 0;
  ScIterator3Ptr const iterator = context.CreateIterator3(classAddr, ScType::ConstPermPosArc, ScType::ConstNode);
  while (iterator->Next())
    ++result;

  return result;
}

size_t CountHomeEvents(ScAgentContext & context, ScAddr const & eventClass, ScAddr const & home)
{
  size_t result = 0;
  ScIterator3Ptr const iterator = context.CreateIterator3(eventClass, ScType::ConstPermPosArc, ScType::ConstNode);
  while (iterator->Next())
  {
    ScAddr const event = iterator->Get(2);
    if (CheckRelation(context, event, home, SmartHomeKeynodes::nrel_event_home))
      ++result;
  }

  return result;
}

size_t CountDeviceEvents(
    ScAgentContext & context,
    ScAddr const & eventClass,
    ScAddr const & device,
    ScAddr const & state)
{
  size_t result = 0;
  ScIterator3Ptr const iterator = context.CreateIterator3(eventClass, ScType::ConstPermPosArc, ScType::ConstNode);
  while (iterator->Next())
  {
    ScAddr const event = iterator->Get(2);
    if (CheckRelation(context, event, device, SmartHomeKeynodes::nrel_event_device)
        && CheckRelation(context, event, state, SmartHomeKeynodes::nrel_event_state))
    {
      ++result;
    }
  }

  return result;
}
