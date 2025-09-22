#pragma once
#include "CalculationMethod.h"
#include "Pin.h"
#include "Ref.h"
#include <string>

using PinRefMap = std::unordered_map<std::string, Ref<Pin>>;
using ParamsMap = std::unordered_map<std::string, double>;

class CalculationMethod;

class CalculationBlock {
protected:
  std::string id;
  PinRefMap inputPins;
  PinRefMap outputPins;
  ParamsMap params;
  Ref<CalculationMethod> method;

  inline Ref<Pin> &AddInputPin(const std::string &name) {
    this->inputPins[name] = Ref<Pin>(new Pin(name));
    return this->inputPins[name];
  }
  inline Ref<Pin> &AddOutputPin(const std::string &name) {
    this->outputPins[name] = Ref<Pin>(new Pin(name));
    return this->outputPins[name];
  }

public:
  CalculationBlock();
  CalculationBlock(const std::string &id);
  CalculationBlock(const std::string &id, ParamsMap params);
  virtual void Calculate() = 0;
  virtual ~CalculationBlock() = default;

  inline Ref<Pin> &GetInputPin(const std::string &name) {
    return this->inputPins.at(name);
  }

  inline double GetInputPinValue(const std::string &pinName,
                                 const std::string &variableName) {
    return GetInputPin(pinName)->GetValue(variableName);
  }
  inline Ref<Pin> &GetOutputPin(const std::string &name) {
    return this->outputPins.at(name);
  }
  inline double GetOutputPinValue(const std::string &pinName,
                                  const std::string &variableName) {
    return GetOutputPin(pinName)->GetValue(variableName);
  }
  inline void SetInputPinValue(const std::string &pinName,
                               const std::string &variableName, double value) {
    GetInputPin(pinName)->SetValue(variableName, value);
  }
  inline void SetOutputPinValue(const std::string &pinName,
                                const std::string &variableName, double value) {
    GetOutputPin(pinName)->SetValue(variableName, value);
  }

  inline double GetParam(const std::string &name) {
    return this->params.at(name);
  }
  inline void SetParam(const std::string &name, double value) {
    this->params[name] = value;
  }

  inline std::string GetId() { return this->id; }

  inline void SetCalculationMethod(const Ref<CalculationMethod> &method) {
    this->method = method;
  }

  void PrintAllValues() const;
};
