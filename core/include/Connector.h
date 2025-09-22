#pragma once
#include <string>

class Connector {
private:
  std::string originId;
  std::string originPin;
  std::string targetId;
  std::string targetPin;
  bool tear;

public:
  Connector(const std::string &originId, const std::string &originPin,
            const std::string &targetId, const std::string &targetPin);

  inline std::string GetOriginId() { return this->originId; }
  inline std::string GetTargetId() { return this->targetId; }
  inline std::string GetOriginPin() { return this->originPin; }
  inline std::string GetTargetPin() { return this->targetPin; }
  inline bool IsTearStream() { return this->tear; }
  inline void MarkAsTearStream(bool tear) { this->tear = tear; }
};
