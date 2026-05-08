#pragma once

#include <sc-memory/sc_module.hpp>
#include "SmartHomeModule.generated.hpp"

namespace smart_home
{

class SmartHomeModule : public ScModule
{
  SC_CLASS(LoadOrder(100))
  SC_GENERATED_BODY()

  virtual sc_result InitializeImpl() override;
  virtual sc_result ShutdownImpl() override;
};

}