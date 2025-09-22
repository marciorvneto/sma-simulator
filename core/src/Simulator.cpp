#include "Simulator.h"
#include "WegsteinRunner.h"

Simulator::Simulator() : runner(new WegsteinRunner()) {}

void Simulator::Run(const std::vector<Ref<CalculationBlock>> &blocks,
                    const std::vector<Ref<Connector>> &connectors) {
  this->runner->Run(blocks, connectors);
}
