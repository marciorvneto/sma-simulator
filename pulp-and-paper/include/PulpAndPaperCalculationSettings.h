#pragma once

#include "BlackLiquor.h"
#include "SettingsTypes.h"
#include <cmath>
#include <functional>

class PulpAndPaperCalculationSettings {
private:
  std::function<double(BlackLiquorConvectiveCoefficientFunctionParams)>
      blackLiquorSideConvectiveCoefficientFunction;

public:
  PulpAndPaperCalculationSettings();
};
