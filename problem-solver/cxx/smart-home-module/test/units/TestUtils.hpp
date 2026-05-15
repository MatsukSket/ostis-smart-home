#pragma once

#include <sc-memory/test/sc_test.hpp>
#include <sc-builder/scs_loader.hpp>

#include <agents/movement-lamp/LightBulbAgent.hpp>
#include <agents/movement-lamp/MotionSensorAgent.hpp>
#include <keynodes/Keynodes.hpp>

using namespace smart_home;

using SmartHomeTest = ScMemoryTest;

inline std::string const TEST_FILES_DIR = TEST_FILES_DIR_PATH;

void LoadTestStructure(ScAgentContext & context, std::string const & filename);
