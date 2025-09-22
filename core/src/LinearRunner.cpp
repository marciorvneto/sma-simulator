#include "LinearRunner.h"
#include <iostream>

void LinearRunner::Run(const std::vector<Ref<CalculationBlock>> &blocks,
                       const std::vector<Ref<Connector>> &connectors) {
  std::cout << "Pushing values..." << std::endl;
  for (auto block : blocks) {
    PushDataAcrossConnectors(blocks, connectors, block);
  }
  std::cout << "Running..." << std::endl;
  for (auto block : blocks) {
    std::cout << "Calculating " << block->GetId() << std::endl;
    block->Calculate();
    PushDataAcrossConnectors(blocks, connectors, block);
  }
  std::cout << "Ended..." << std::endl;
};
