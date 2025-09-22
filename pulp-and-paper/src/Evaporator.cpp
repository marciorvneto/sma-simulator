#include "Evaporator.h"
#include "BlackLiquor.h"
#include "CalculationBlock.h"
#include "Numeric.h"
#include "Steam.h"
#include <iostream>

Evaporator::Evaporator(const std::string &id) : CalculationBlock(id) {
  InitializePins();
}
Evaporator::Evaporator(const std::string &id, ParamsMap params)
    : CalculationBlock(id, params) {
  InitializePins();
}

void Evaporator::InitializePins() {
  auto &S = AddInputPin("S");
  S->SetValue("m", 1);
  S->SetValue("P", 1);
  S->SetValue("T", 25);

  auto &F = AddInputPin("F");
  F->SetValue("m", 1);
  F->SetValue("T", 25);
  F->SetValue("x", 0.1);

  auto &V = AddOutputPin("V");
  V->SetValue("m", 1);
  V->SetValue("P", 1);
  V->SetValue("T", 25);

  auto &L = AddOutputPin("L");
  L->SetValue("m", 1);
  L->SetValue("T", 25);
  L->SetValue("x", 0.1);

  auto &C = AddOutputPin("C");
  C->SetValue("m", 1);
  C->SetValue("P", 1);
  C->SetValue("T", 25);
}

void Evaporator::Calculate() {
  if (this->method.IsNull()) {
    std::cout << "ERROR: No method set!" << std::endl;
    return;
  }
  this->method->Calculate();
}

// Methods

Evaporator::MethodGivenOutletPressure::MethodGivenOutletPressure(
    const Ref<CalculationBlock> &parent)
    : CalculationMethod(parent, "OutletPressureKnown") {}

void Evaporator::MethodGivenOutletPressure::Calculate() {
  // Assuming T in oC and P in bar

  // Assuming 18 variables in total:
  // F: m, T, x
  // L: m, T, x
  // S: m, T, P
  // V: m, T, P
  // C: m, T, P
  // A, Q, U

  const auto &S = parent->GetInputPin("S");
  const auto &F = parent->GetInputPin("F");
  const auto &V = parent->GetOutputPin("V");
  const auto &L = parent->GetOutputPin("L");
  const auto &C = parent->GetOutputPin("C");

  // Taking as known:
  // - TF
  // - mF
  // - xF
  // - xL
  // - PV
  // - PS
  // - U

  double TF = F->GetValue("T");
  double mF = F->GetValue("m");
  double xF = F->GetValue("x");
  double xL = L->GetValue("x");
  double PV = V->GetValue("P");
  double PS = S->GetValue("P");
  double U = parent->GetParam("U");

  // Initial estimates
  double mL = 0;
  double TL = 25;

  double mS = 0;
  double TS = 25;

  double mC = 0;
  double TC = 25;
  double PC = 1;

  double mV = 0;
  double TV = 25;

  double Q = 0;
  double A = 0;

  auto system = [&](std::vector<double> x) {
    mS = x[0];
    A = x[1];

    // Simple equations

    // --- Mass balances

    mC = mS;
    mL = mF * xF / xL;
    mV = mF - mL;

    // --- Thermodynamics

    TL = Steam::Tsat(PV) + BPR_BL(xL, PV);
    TV = TL;
    PC = PS;
    TC = Steam::Tsat(PC);
    TS = Steam::Tsat(PS);

    // --- Energy balances

    // std::cout << "EB" << std::endl;
    Q = U * A * (TS - TL);

    double hS = Steam::hV_p(PS);
    double hC = Steam::hL_p(PC);
    double hV = Steam::h_Tp(TV, PV);
    double hL = h_BL(TL, xL);
    double hF = h_BL(TF, xF);

    // std::cout << "=============" << std::endl;
    // std::cout << "hS " << hS << std::endl;
    // std::cout << "hC " << hC << std::endl;
    // std::cout << "hF " << hF << std::endl;
    // std::cout << "hL " << hL << std::endl;
    // std::cout << "hV " << hV << std::endl;
    // std::cout << "PS " << PS << std::endl;
    // std::cout << "TS " << TS << std::endl;
    // std::cout << "TL " << TL << std::endl;
    // std::cout << "TF " << TF << std::endl;
    // std::cout << "Q " << Q << std::endl;
    // std::cout << "A " << A << std::endl;
    // std::cout << "U " << U << std::endl;

    double ebSteamSide = hS * mS - mC * hC - Q;
    double ebLiquorSide = hF * mF + Q - mL * hL - mV * hV;

    std::vector<double> out = {ebSteamSide, ebLiquorSide};
    return out;
  };

  NDNewtonRaphson::SolverOptions options;
  options.max_iterations = 100;
  options.verbose = false;
  options.h = 1e-6;

  NDNewtonRaphson solver(system, options);
  auto result = solver.solve({0, 0});
  auto out = result.solution;

  parent->SetOutputPinValue("V", "m", mV);
  parent->SetOutputPinValue("V", "T", TV);
  parent->SetOutputPinValue("V", "P", PV);

  parent->SetOutputPinValue("C", "m", mC);
  parent->SetOutputPinValue("C", "T", TC);
  parent->SetOutputPinValue("C", "P", PC);

  parent->SetOutputPinValue("L", "m", mL);
  parent->SetOutputPinValue("L", "T", TL);
  parent->SetOutputPinValue("L", "x", xL);

  parent->SetInputPinValue("S", "m", mS);
  parent->SetInputPinValue("S", "T", TS);
  parent->SetInputPinValue("S", "P", PS);

  parent->SetInputPinValue("F", "m", mF);
  parent->SetInputPinValue("F", "T", TF);
  parent->SetInputPinValue("F", "x", xF);

  parent->SetParam("Q", Q);
  parent->SetParam("A", A);
}

Evaporator::MethodGivenInletData::MethodGivenInletData(
    const Ref<CalculationBlock> &parent)
    : CalculationMethod(parent, "InletDataKnown") {}

void Evaporator::MethodGivenInletData::Calculate() {
  // Assuming T in oC and P in bar

  // Assuming 18 variables in total:
  // F: m, T, x
  // L: m, T, x
  // S: m, T, P
  // V: m, T, P
  // C: m, T, P
  // A, Q, U

  const auto &S = parent->GetInputPin("S");
  const auto &F = parent->GetInputPin("F");
  const auto &V = parent->GetOutputPin("V");
  const auto &L = parent->GetOutputPin("L");
  const auto &C = parent->GetOutputPin("C");

  // Taking as known:
  // - TF
  // - mF
  // - xF
  // - PS
  // - mS
  // - U
  // - A

  double TF = F->GetValue("T");
  double mF = F->GetValue("m");
  double xF = F->GetValue("x");
  double PS = S->GetValue("P");
  double mS = S->GetValue("m");
  double U = parent->GetParam("U");
  double A = parent->GetParam("A");

  // Initial estimates
  double mL = 0;
  double TL = 25;
  double xL = 0.3;

  double TS = 25;

  double mC = 0;
  double TC = 25;
  double PC = 1;

  double mV = 0;
  double TV = 25;
  double PV = 0.5;

  double Q = 0;

  auto system = [&](std::vector<double> x) {
    xL = std::exp(x[0]);
    PV = x[1];
    // std::cout << "=============" << std::endl;
    // std::cout << "PV " << PV << std::endl;
    // std::cout << "xL " << xL << std::endl;

    // Simple equations

    // --- Mass balances

    mC = mS;
    mL = mF * xF / xL;
    mV = mF - mL;

    // --- Thermodynamics

    TL = Steam::Tsat(PV) + BPR_BL(xL, PV);
    TV = TL;
    PC = PS;
    TC = Steam::Tsat(PC);
    TS = Steam::Tsat(PS);

    // std::cout << "Tsat " << Steam::Tsat(PV) << std::endl;
    // std::cout << "BPR " << BPR_BL(xL, PV) << std::endl;
    // std::cout << "TL " << TL << std::endl;
    // std::cout << "TC " << TC << std::endl;
    // std::cout << "TS " << TS << std::endl;
    //
    // --- Energy balances

    // std::cout << "EB" << std::endl;
    Q = U * A * (TS - TL);

    double hS = Steam::hV_p(PS);
    double hC = Steam::hL_p(PC);
    double hV = Steam::h_Tp(TV, PV);
    double hL = h_BL(TL, xL);
    double hF = h_BL(TF, xF);

    // std::cout << "hS " << hS << std::endl;
    // std::cout << "hC " << hC << std::endl;
    // std::cout << "hF " << hF << std::endl;
    // std::cout << "hL " << hL << std::endl;
    // std::cout << "hV " << hV << std::endl;
    // std::cout << "PS " << PS << std::endl;
    // std::cout << "TF " << TF << std::endl;
    // std::cout << "Q " << Q << std::endl;
    // std::cout << "A " << A << std::endl;
    // std::cout << "U " << U << std::endl;

    double ebSteamSide = hS * mS - mC * hC - Q;
    double ebLiquorSide = hF * mF + Q - mL * hL - mV * hV;

    std::vector<double> out = {ebSteamSide, ebLiquorSide};
    return out;
  };

  NDNewtonRaphson::SolverOptions options;
  options.max_iterations = 100;
  options.verbose = false;
  options.h = 1e-6;

  NDNewtonRaphson solver(system, options);
  auto result = solver.solve({std::log(0.5), 1});
  auto out = result.solution;

  parent->SetOutputPinValue("V", "m", mV);
  parent->SetOutputPinValue("V", "T", TV);
  parent->SetOutputPinValue("V", "P", PV);

  parent->SetOutputPinValue("C", "m", mC);
  parent->SetOutputPinValue("C", "T", TC);
  parent->SetOutputPinValue("C", "P", PC);

  parent->SetOutputPinValue("L", "m", mL);
  parent->SetOutputPinValue("L", "T", TL);
  parent->SetOutputPinValue("L", "x", xL);

  parent->SetInputPinValue("S", "m", mS);
  parent->SetInputPinValue("S", "T", TS);
  parent->SetInputPinValue("S", "P", PS);

  parent->SetInputPinValue("F", "m", mF);
  parent->SetInputPinValue("F", "T", TF);
  parent->SetInputPinValue("F", "x", xF);

  parent->SetParam("Q", Q);
  parent->SetParam("A", A);
}
