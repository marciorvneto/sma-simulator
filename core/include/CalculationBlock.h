#pragma once
#include "Pin.h"
#include <string>

using PinMap = std::unordered_map<std::string, Pin>;

class CalculationBlock {
private:
  std::string id;
  PinMap inputPins;
  PinMap outputPins;

public:
  CalculationBlock();
  CalculationBlock(const std::string &id);
  virtual void Calculate() = 0;
  virtual ~CalculationBlock() = default;

  inline std::string GetId() { return this->id; }
};
