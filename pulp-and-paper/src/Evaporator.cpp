#include "Evaporator.h"
#include "CalculationBlock.h"

Evaporator::Evaporator(const std::string &id) : CalculationBlock(id) {}
Evaporator::Evaporator(const std::string &id, EvaporatorParams params)
    : CalculationBlock(id), params(params) {}

void Evaporator::Calculate() {}
