#pragma once
#include "CalculationBlock.h"
#include "Connector.h"
#include "Ref.h"
#include <vector>

struct BlockConnectors {
  std::vector<Ref<Connector>> outConnectors;
  std::vector<Ref<Connector>> inConnectors;
};

BlockConnectors
GetBlockConnectors(const std::vector<Ref<Connector>> &connectors,
                   const Ref<CalculationBlock> &block);

void PushDataAcrossConnectors(const std::vector<Ref<CalculationBlock>> &blocks,
                              const std::vector<Ref<Connector>> &connectors,
                              const Ref<CalculationBlock> &block);
