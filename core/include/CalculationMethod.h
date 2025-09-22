#pragma once
#include "Ref.h"
#include <string>

class CalculationBlock;

class CalculationMethod {
protected:
  std::string name;
  Ref<CalculationBlock> parent;

public:
  CalculationMethod() = default;
  CalculationMethod(const Ref<CalculationBlock> &parent);
  CalculationMethod(const Ref<CalculationBlock> &parent,
                    const std::string &name);
  virtual void Calculate();

  inline std::string GetName() { return name; }
};
