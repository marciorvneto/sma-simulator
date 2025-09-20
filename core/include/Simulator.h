#pragma once
#include "CalculationBlock.h"
#include "Connector.h"
#include "Ref.h"
#include "Runner.h"
#include <vector>

class Simulator {
private:
  Ref<Runner> runner;
  std::vector<CalculationBlock> blocks;
  std::vector<Connector> connectors;

public:
  Simulator();
  void Run(const std::vector<Ref<CalculationBlock>> &blocks,
           const std::vector<Ref<Connector>> &connectors);
};
