#include "Connectivity.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

BlockConnectors
GetBlockConnectors(const std::vector<Ref<Connector>> &connectors,
                   const Ref<CalculationBlock> &block) {
  std::vector<Ref<Connector>> outConnectors;
  std::vector<Ref<Connector>> inConnectors;
  for (auto &conn : connectors) {
    if (conn->GetOriginId() == block->GetId()) {
      outConnectors.push_back(conn);
    }
    if (conn->GetTargetId() == block->GetId()) {
      inConnectors.push_back(conn);
    }
  }
  BlockConnectors out = {
      .outConnectors = outConnectors,
      .inConnectors = inConnectors,
  };
  return out;
}

void PushDataAcrossConnectors(const std::vector<Ref<CalculationBlock>> &blocks,
                              const std::vector<Ref<Connector>> &connectors,
                              const Ref<CalculationBlock> &block) {
  BlockConnectors bc = GetBlockConnectors(connectors, block);
  for (auto &conn : bc.outConnectors) {
    auto originPinId = conn->GetOriginPin();
    auto targetId = conn->GetTargetId();
    auto targetPinId = conn->GetTargetPin();

    auto it = std::find_if(blocks.begin(), blocks.end(), [&](auto block) {
      return block->GetId() == targetId;
    });

    if (it == blocks.end()) {
      throw std::out_of_range("Could not find block with id " + targetId);
    }

    auto targetBlock = *it;
    auto &originPin = block->GetOutputPin(originPinId);
    auto &targetPin = targetBlock->GetInputPin(targetPinId);

    // Push the data

    for (auto &values : originPin->GetValuesMap()) {
      auto variableName = values.first;
      auto value = values.second;
      targetBlock->SetInputPinValue(targetPinId, variableName, value);
      // std::cout << "Pushing " << block->GetId() << ":" << originPinId << ":"
      //           << variableName << " -> " << targetId << ":" << targetPinId
      //           << ":" << variableName << "(" << value << ")" << std::endl;
    }
  }
}
