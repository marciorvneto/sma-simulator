#include "Evaporator.h"
#include "Ref.h"
#include "Simulator.h"
#include <iostream>
#include <vector>

int main() {
  Simulator sim;

  Ref<CalculationBlock> e1(
      new Evaporator("E1", {.heatTransferArea = 100,
                            .heatLoad = 12000,
                            .globalHeatTransferCoefficient = 1000,
                            .pipeDiameter = 25e-3}));

  std::vector<Ref<CalculationBlock>> blocks = {};
  blocks.push_back(e1);
  sim.Run(blocks, {});
}
