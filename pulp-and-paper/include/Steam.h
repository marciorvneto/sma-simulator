#pragma once
#include "IF97.h"

constexpr double C_TO_K = 273.16;

namespace Steam {
inline double hV_p(double Pbar) { return IF97::hvap_p(Pbar * 1e5) / 1000; }
inline double hL_p(double Pbar) { return IF97::hliq_p(Pbar * 1e5) / 1000; }
inline double h_Tp(double TC, double Pbar) {
  return IF97::hmass_Tp(TC + C_TO_K, Pbar * 1e5) / 1000;
}
inline double Tsat(double Pbar) { return IF97::Tsat97(Pbar * 1e5) - C_TO_K; }
} // namespace Steam
