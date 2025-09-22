#include "Connector.h"

Connector::Connector(const std::string &originId, const std::string &originPin,
                     const std::string &targetId, const std::string &targetPin)
    : originId(originId), originPin(originPin), targetId(targetId),
      targetPin(targetPin), tear(false) {}
