#pragma once
#include "CalculationBlock.h"
#include <string>

class Evaporator : public CalculationBlock {
public:
  class MethodGivenOutletPressure : public CalculationMethod {
  public:
    MethodGivenOutletPressure(const Ref<CalculationBlock> &parent);
    void Calculate() override;
  };

  class MethodGivenInletData : public CalculationMethod {
  public:
    MethodGivenInletData(const Ref<CalculationBlock> &parent);
    void Calculate() override;
  };

private:
  void InitializePins();
  void SetDefaultCalculationMethod();

public:
  Evaporator(const std::string &id);
  Evaporator(const std::string &id, ParamsMap params);
  void Calculate() override;
};
