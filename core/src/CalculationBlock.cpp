#include "CalculationBlock.h"
#include <iostream>
CalculationBlock::CalculationBlock() : id("") {}
CalculationBlock::CalculationBlock(const std::string &id) : id(id) {}
CalculationBlock::CalculationBlock(const std::string &id, ParamsMap params)
    : id(id), params(params) {}

// Add to CalculationBlock.cpp (or inline in header)
void CalculationBlock::PrintAllValues() const {
  std::cout << "\n=== Block: " << id << " ===" << std::endl;

  // Print parameters
  std::cout << "\n--- Parameters ---" << std::endl;
  for (const auto &[name, value] : params) {
    std::cout << "  " << name << ": " << value << std::endl;
  }

  // Print input pins
  std::cout << "\n--- Input Pins ---" << std::endl;
  for (const auto &[pinName, pin] : inputPins) {
    std::cout << "  Pin: " << pinName << std::endl;
    for (const auto &[varName, value] : pin->GetValuesMap()) {
      std::cout << "    " << varName << ": " << value << std::endl;
    }
  }

  // Print output pins
  std::cout << "\n--- Output Pins ---" << std::endl;
  for (const auto &[pinName, pin] : outputPins) {
    std::cout << "  Pin: " << pinName << std::endl;
    for (const auto &[varName, value] : pin->GetValuesMap()) {
      std::cout << "    " << varName << ": " << value << std::endl;
    }
  }

  std::cout << "========================\n" << std::endl;
}
