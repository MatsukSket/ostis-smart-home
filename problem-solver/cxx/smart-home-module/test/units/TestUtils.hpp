#pragma once

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <agents/movement-lamp/LightBulbAgent.hpp>
#include <agents/movement-lamp/MotionSensorAgent.hpp>
#include <keynodes/Keynodes.hpp>

using namespace smart_home;

using SmartHomeTest = ScMemoryTest;

inline std::string const TEST_FILES_DIR = TEST_FILES_DIR_PATH;

inline void LoadTestStructure(ScAgentContext & context, std::string const & filename)
{
  ScsLoader loader;
  loader.loadScsFile(context, TEST_FILES_DIR + filename);
}

inline ScAddr FindLampBySensor(ScAgentContext & context, ScAddr const & sensor)
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
}
