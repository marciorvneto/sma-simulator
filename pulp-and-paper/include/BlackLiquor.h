#pragma once
#include <cmath>

double h_BL(double TC, double x);
double cp_BL(double TC, double x);
double intCp_BL(double T0C, double TfC, double x);
double density_BL(double TC, double x);
// double conductivity_BL(double TC);
double dynamic_viscosity_BL(double TC, double x);

double BPR_BL(double x, double Pbar);

double convectiveCoeffJohansson(double specificMassFlow,
                                double dynamicVisosity);
