#include "PulpAndPaperCalculationSettings.h"

PulpAndPaperCalculationSettings::PulpAndPaperCalculationSettings() {
  this->blackLiquorSideConvectiveCoefficientFunction =
      [](BlackLiquorConvectiveCoefficientFunctionParams params) {
        double wetPerimeter = M_PI * params.diameter;
        double specificMassFlow = params.massFlow / wetPerimeter;
        double dynamicViscosity =
            dynamic_viscosity_BL(params.temperatureC, params.solidsFraction);
        return convectiveCoeffJohansson(specificMassFlow, dynamicViscosity);
      };
}
