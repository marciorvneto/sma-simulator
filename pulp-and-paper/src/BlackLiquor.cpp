#include "BlackLiquor.h"

double density_BL(double TC, double x);

double h_BL(double TC, double x) {
  // Black liquor enthalpy calculation based on the given correlation
  // TC = Temperature in °C
  // x = Dry solids mass fraction (0 to 1)
  // Returns enthalpy in kJ/kg

  double B = 105.0; // kJ/kg
  double c = 0.3;

  // Base enthalpy at 80°C
  double h_80 = 334.88 + B * (-1.0 + std::exp(c * x));

  // Heat capacity integral from 80°C to T
  // Term 1: 4.216(1-x) integrated from 80 to T
  // Term 2: [1.675 + 3.31*T/1000]*x integrated from 80 to T
  // Term 3: [4.87 + 20*T/1000]*(1-x)*x³ integrated from 80 to T

  double cp_integral =
      4.216 * (1.0 - x) * (TC - 80.0) +
      x * (1.675 * (TC - 80.0) + 3.31 * (std::pow(TC, 2) - 6400.0) / 2000.0) +
      (1.0 - x) * std::pow(x, 3) *
          (4.87 * (TC - 80.0) + (std::pow(TC, 2) - 6400.0) / 100.0);

  // Total enthalpy
  return h_80 + cp_integral;
}

double BPR_BL(double x, double Pbar) {
  // Boiling Point Rise calculation for black liquor
  // Pbar = Pressure in bar
  // x = Dry solids mass fraction (0 to 1)
  // Returns BPR in °C

  // Calculate BPR at atmospheric conditions
  double BPR_atm =
      6.173 * x - 7.48 * std::pow(x, 1.5) + 32.747 * std::pow(x, 2);

  // Get saturation temperature of pure water at pressure P (in bar)
  // Note: You'll need to implement Tsat_p function or use steam tables
  // For now, using Antoine equation approximation for water
  double Tsat_water;
  if (Pbar > 0) {
    // Antoine equation for water (pressure in bar, temperature in °C)
    // log10(P) = 8.07131 - 1730.63/(T + 233.426)
    // Rearranging: T = 1730.63/(8.07131 - log10(P)) - 233.426
    Tsat_water = 1730.63 / (8.07131 - std::log10(Pbar)) - 233.426;
  } else {
    Tsat_water = 100.0; // Default to 100°C at 1 bar
  }

  // Apply pressure correction
  double correction_factor = 1.0 + 0.6 * (Tsat_water - 100.0) / 100.0;

  // Calculate final BPR
  return BPR_atm * correction_factor;
}

double density_BL(double TC, double x) {
  // TC = Temperature in °C
  // x = Dry solids mass fraction (0 to 1)
  // Returns density in kg/m³

  double dens_bl_25 = 997.0 + 694.0 * x;
  double dens_bl = dens_bl_25 * (1.008 - 0.237 * TC / 1000.0 -
                                 1.94 * std::pow(TC / 1000.0, 2));

  return dens_bl;
}

double dynamic_viscosity_BL(double TC, double x) {
  // TC = Temperature in °C
  // x = Dry solids mass fraction (0 to 1)
  // Returns dynamic viscosity in Pa·s

  double rho =
      density_BL(x, TC); // Note: swapped parameter order to match dependency

  double TK = TC + 273.16; // Convert to Kelvin

  // Coefficients
  double a1 = 3.3532;
  double a2 = 3.7654;
  double a3 = -2.49;

  double b1 = -54420000.0;
  double b2 = 219150000.0;
  double b3 = 170420000.0;

  double Ah2o = -2.4273;
  double Bh2o = 61347000.0;

  // Calculate A and B parameters
  double A = Ah2o + a1 * x + a2 * std::pow(x, 2) + a3 * std::pow(x, 3);
  double B = Bh2o + b1 * x + b2 * std::pow(x, 2) + b3 * std::pow(x, 3);

  // Calculate kinematic viscosity
  double ln_nu = A + B / std::pow(TK, 3);

  // Convert to dynamic viscosity (Pa·s)
  return rho * std::exp(ln_nu) / 1000000.0;
}

// Heat capacity function (derived from enthalpy function)
double cp_BL(double TC, double x) {
  // Black liquor specific heat capacity
  // TC = Temperature in °C
  // x = Dry solids mass fraction (0 to 1)
  // Returns heat capacity in kJ/(kg·K)

  double cp = 4.216 * (1.0 - x) + x * (1.675 + 3.31 * TC / 1000.0) +
              (1.0 - x) * std::pow(x, 3) * (4.87 + 20.0 * TC / 1000.0);

  return cp;
}

// Integral of heat capacity from T0 to Tf
double intCp_BL(double T0C, double TfC, double x) {
  // Integral of heat capacity from T0C to TfC
  // T0C = Initial temperature in °C
  // TfC = Final temperature in °C
  // x = Dry solids mass fraction (0 to 1)
  // Returns integrated heat capacity in kJ/kg

  double cp_integral =
      4.216 * (1.0 - x) * (TfC - T0C) +
      x * (1.675 * (TfC - T0C) +
           3.31 * (std::pow(TfC, 2) - std::pow(T0C, 2)) / 2000.0) +
      (1.0 - x) * std::pow(x, 3) *
          (4.87 * (TfC - T0C) + (std::pow(TfC, 2) - std::pow(T0C, 2)) / 100.0);

  return cp_integral;
}

double convectiveCoeffJohansson(double specificMassFlow,
                                double dynamicVisosity) {
  return 201 * std::pow(specificMassFlow, 0.26) *
         std::pow(dynamicVisosity, -0.41);
}
