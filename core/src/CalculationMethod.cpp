#include "CalculationMethod.h"
#include "CalculationBlock.h"

CalculationMethod::CalculationMethod(const Ref<CalculationBlock> &parent)
    : parent(parent) {}
CalculationMethod::CalculationMethod(const Ref<CalculationBlock> &parent,
                                     const std::string &name)
    : parent(parent), name(name) {}

void CalculationMethod::Calculate() {}
