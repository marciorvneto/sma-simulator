#pragma once
#include <string>
#include <unordered_map>

class Pin {
private:
  std::string id;
  std::unordered_map<std::string, double> values;

public:
  Pin(const std::string &id);
  inline std::string GetId() { return this->id; }
  inline double GetValue(const std::string &variableName) {
    return values[variableName];
  }
  inline void SetValue(const std::string &variableName, double value) {
    values[variableName] = value;
  }
};
