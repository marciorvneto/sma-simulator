#pragma once
#include "CalculationBlock.h"
#include <string>

struct EvaporatorParams {
  double heatTransferArea;
  double heatLoad;
  double globalHeatTransferCoefficient;
  double pipeDiameter;
};

class Evaporator : public CalculationBlock {
private:
  EvaporatorParams params;

public:
  Evaporator(const std::string &id);
  Evaporator(const std::string &id, EvaporatorParams params);
  void Calculate() override;
};
