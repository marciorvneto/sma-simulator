#pragma once

#include "CalculationBlock.h"
#include "Connector.h"
#include "Ref.h"
#include <vector>

class Runner {
public:
  virtual void Run(const std::vector<Ref<CalculationBlock>> &blocks,
                   const std::vector<Ref<Connector>> &connectors) = 0;
  virtual ~Runner() = default;
};
