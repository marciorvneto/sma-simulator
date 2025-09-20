#pragma once
#include "Runner.h"

class LinearRunner : public Runner {
public:
  void Run(const std::vector<Ref<CalculationBlock>> &blocks,
           const std::vector<Ref<Connector>> &connectors);
};
