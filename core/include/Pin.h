#pragma once
#include <string>
#include <unordered_map>

using PinMap = std::unordered_map<std::string, double>;

class Pin {
private:
  std::string id;
  PinMap values;

public:
  Pin();
  Pin(const std::string &id);
  inline std::string GetId() { return this->id; }
  inline double GetValue(const std::string &variableName) {
    return values[variableName];
  }
  inline PinMap &GetValuesMap() { return values; }
  inline void SetValue(const std::string &variableName, double value) {
    values[variableName] = value;
  }
};
