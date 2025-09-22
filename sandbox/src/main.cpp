#include "Evaporator.h"
#include "Ref.h"
#include "Simulator.h"
#include <iostream>
#include <vector>

int main() {
  Simulator sim;

  Ref<CalculationBlock> e1(new Evaporator("E1", {
                                                    {"A", 2100},
                                                    {"Q", 12000},
                                                    {"U", 0.5},
                                                    {"D", 25e-3},

                                                }));

  e1->SetInputPinValue("S", "m", 4.0);
  e1->SetInputPinValue("S", "P", 1.1);
  e1->SetInputPinValue("F", "T", 86.93);
  e1->SetInputPinValue("F", "m", 8.21);
  e1->SetInputPinValue("F", "x", 0.14);
  e1->SetCalculationMethod(
      Ref<CalculationMethod>(new Evaporator::MethodGivenInletData(e1)));

  Ref<CalculationBlock> e2(new Evaporator("E2", {
                                                    {"A", 2500},
                                                    {"Q", 12000},
                                                    {"U", 0.5},
                                                    {"D", 25e-3},
                                                }));

  e2->SetInputPinValue("S", "m", 3.9);
  e2->SetInputPinValue("S", "P", 0.7);
  e2->SetInputPinValue("F", "T", 25);
  e2->SetInputPinValue("F", "m", 12);
  e2->SetInputPinValue("F", "x", 0.1);
  e2->SetCalculationMethod(
      Ref<CalculationMethod>(new Evaporator::MethodGivenInletData(e2)));

  std::vector<Ref<CalculationBlock>> blocks = {};
  blocks.push_back(e1);
  blocks.push_back(e2);

  std::vector<Ref<Connector>> conns = {};
  Ref<Connector> V1("E1", "V", "E2", "S");
  Ref<Connector> L2("E2", "L", "E1", "F");

  V1->MarkAsTearStream(true);

  conns.push_back(V1);
  conns.push_back(L2);

  sim.Run(blocks, conns);

  std::cout << ">>> Results: " << std::endl;

  for (auto &block : blocks) {
    block->PrintAllValues();
  }
}
