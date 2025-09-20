#include "Simulator.h"
#include "LinearRunner.h"

Simulator::Simulator() : runner(new LinearRunner()) {}

void Simulator::Run(const std::vector<Ref<CalculationBlock>> &blocks,
                    const std::vector<Ref<Connector>> &connectors) {
  this->runner->Run(blocks, connectors);
}
