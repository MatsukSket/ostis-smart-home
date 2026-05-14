#pragma once

#include <cstddef>
#include <string>

#include <sc-memory/sc_agent_context.hpp>

extern std::string const SMART_HOME_TEST_FILES_DIR_PATH;

void LoadScsFile(ScAgentContext & context, std::string const & fileName);

ScAddr ResolveKeynode(ScAgentContext & context, std::string const & systemIdentifier);

void SetRelation(ScAgentContext & context, ScAddr const & source, ScAddr const & target, ScAddr const & relation);

ScAddr GetRelationTarget(ScAgentContext & context, ScAddr const & source, ScAddr const & relation);

bool CheckRelation(
    ScAgentContext & context,
    ScAddr const & source,
    ScAddr const & target,
    ScAddr const & relation);

bool HasClass(ScAgentContext & context, ScAddr const & element, ScAddr const & classAddr);

size_t CountClassInstances(ScAgentContext & context, ScAddr const & classAddr);

size_t CountHomeEvents(ScAgentContext & context, ScAddr const & eventClass, ScAddr const & home);

size_t CountDeviceEvents(
    ScAgentContext & context,
    ScAddr const & eventClass,
    ScAddr const & device,
    ScAddr const & state);
