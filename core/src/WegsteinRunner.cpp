#include "WegsteinRunner.h"
#include "Connectivity.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

// Structure to hold Wegstein data for each tear variable
struct WegsteinData {
  double x_prev = 0.0; // Previous input guess
  double y_prev = 0.0; // Previous output from function
  double x_curr = 0.0; // Current input guess
  double y_curr = 0.0; // Current output from function
  double q = 0.0;      // Wegstein acceleration parameter
  bool initialized = false;

  // Calculate the acceleration parameter q
  void UpdateAcceleration() {
    if (!initialized)
      return;

    double denominator = (y_curr - x_curr) - (y_prev - x_prev);
    if (std::abs(denominator) < 1e-12) {
      q = 0.0; // Fall back to direct substitution
    } else {
      q = (y_prev - x_prev) / denominator;
      // Limit q to avoid instability
      q = std::max(-1.0, std::min(1.0, q));
    }
  }

  // Calculate next guess using Wegstein's method
  double GetNextGuess() {
    if (!initialized) {
      return y_curr; // Direct substitution for first iteration
    }

    UpdateAcceleration();
    return y_curr + q * (y_curr - x_curr);
  }

  void Update(double new_x, double new_y) {
    x_prev = x_curr;
    y_prev = y_curr;
    x_curr = new_x;
    y_curr = new_y;
    initialized = true;
  }
};

void WegsteinRunner::Run(const std::vector<Ref<CalculationBlock>> &blocks,
                         const std::vector<Ref<Connector>> &connectors) {
  const double MAX_REL_ERROR = 1e-6;
  const double MAX_ABS_ERROR = 1e-8;
  const int MAX_ITERATIONS = 100;

  // Identify tear connectors
  std::vector<Ref<Connector>> tearConnectors;
  std::copy_if(connectors.begin(), connectors.end(),
               std::back_inserter(tearConnectors),
               [](auto &conn) { return conn->IsTearStream(); });

  if (tearConnectors.empty()) {
    std::cout << "No tear streams found, running sequential calculation..."
              << std::endl;
    RunSequential(blocks, connectors);
    return;
  }

  std::cout << "Found " << tearConnectors.size() << " tear streams"
            << std::endl;

  // Store Wegstein data for each tear variable
  std::map<std::string, WegsteinData> wegsteinData;

  // Initialize tear stream guesses
  InitializeTearStreams(blocks, tearConnectors, wegsteinData);

  // Main iteration loop
  for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {

    // Store current tear stream values as input guesses
    StoreTearStreamInputs(blocks, tearConnectors, wegsteinData);

    // Run all blocks in sequence
    for (auto block : blocks) {
      // std::cout << "Before calcualtion" << std::endl;
      // block->PrintAllValues();
      block->Calculate();
      // std::cout << "After calcualtion" << std::endl;
      // block->PrintAllValues();
      PushDataAcrossConnectors(blocks, connectors, block);
    }

    // Check convergence and update Wegstein data
    bool converged = CheckConvergenceAndUpdate(
        blocks, tearConnectors, wegsteinData, MAX_REL_ERROR, MAX_ABS_ERROR);

    if (converged) {
      std::cout << "\nConverged after " << (iteration + 1) << " iterations!"
                << std::endl;
      break;
    } else {
      // std::cout << "Not converged, continuing to next iteration..."
      //           << std::endl;
    }

    // Apply Wegstein acceleration for next iteration
    if (iteration < MAX_ITERATIONS - 1) {
      ApplyWegsteinAcceleration(blocks, tearConnectors, wegsteinData);
    }
  }

  std::cout << "Wegstein method completed." << std::endl;
}

void WegsteinRunner::RunSequential(
    const std::vector<Ref<CalculationBlock>> &blocks,
    const std::vector<Ref<Connector>> &connectors) {
  for (const auto &block : blocks) {
    block->Calculate();
    PushDataAcrossConnectors(blocks, connectors, block);
  }
}

void WegsteinRunner::InitializeTearStreams(
    const std::vector<Ref<CalculationBlock>> &blocks,
    const std::vector<Ref<Connector>> &tearConnectors,
    std::map<std::string, WegsteinData> &wegsteinData) {

  for (auto &tearConn : tearConnectors) {
    // Find origin block
    auto originBlock = FindBlock(blocks, tearConn->GetOriginId());
    if (originBlock.IsNull()) {
      continue;
    }

    auto &originPin = originBlock->GetOutputPin(tearConn->GetOriginPin());
    // Initialize Wegstein data for each variable in the tear stream
    for (auto &values : originPin->GetValuesMap()) {
      std::string key = tearConn->GetOriginId() + ":" +
                        tearConn->GetOriginPin() + ":" + values.first;
      wegsteinData[key] = WegsteinData();

      // Set initial guess (you might want to make this more sophisticated)
      double initialGuess = values.second != 0.0 ? values.second : 1.0;

      // Set the initial guess in the target block
      auto targetBlock = FindBlock(blocks, tearConn->GetTargetId());
      if (!targetBlock.IsNull()) {
        targetBlock->SetInputPinValue(tearConn->GetTargetPin(), values.first,
                                      initialGuess);
      }
    }
  }
}

void WegsteinRunner::StoreTearStreamInputs(
    const std::vector<Ref<CalculationBlock>> &blocks,
    const std::vector<Ref<Connector>> &tearConnectors,
    std::map<std::string, WegsteinData> &wegsteinData) {
  for (auto &tearConn : tearConnectors) {
    auto targetBlock = FindBlock(blocks, tearConn->GetTargetId());
    if (targetBlock.IsNull())
      continue;

    auto &targetPin = targetBlock->GetInputPin(tearConn->GetTargetPin());

    for (auto &values : targetPin->GetValuesMap()) {
      std::string key = tearConn->GetOriginId() + ":" +
                        tearConn->GetOriginPin() + ":" + values.first;
      if (wegsteinData.find(key) != wegsteinData.end()) {
        // Store current input as x_curr
        wegsteinData[key].x_curr = values.second;
      }
    }
  }
}

bool WegsteinRunner::CheckConvergenceAndUpdate(
    const std::vector<Ref<CalculationBlock>> &blocks,
    const std::vector<Ref<Connector>> &tearConnectors,
    std::map<std::string, WegsteinData> &wegsteinData, double maxRelError,
    double maxAbsError) {
  bool allConverged = true;
  int convergedCount = 0;
  int totalVariables = 0;

  for (auto &tearConn : tearConnectors) {
    auto originBlock = FindBlock(blocks, tearConn->GetOriginId());
    if (originBlock.IsNull()) {
      continue;
    }

    auto &originPin = originBlock->GetOutputPin(tearConn->GetOriginPin());

    for (auto &values : originPin->GetValuesMap()) {
      totalVariables++;
      std::string key = tearConn->GetOriginId() + ":" +
                        tearConn->GetOriginPin() + ":" + values.first;

      // std::cout << "DEBUG: Looking for key: " << key << std::endl;

      if (wegsteinData.find(key) != wegsteinData.end()) {
        // std::cout << "DEBUG: Found wegstein data for key: " << key <<
        // std::endl;

        double y_new = values.second;             // Output from function
        double x_curr = wegsteinData[key].x_curr; // Current input

        // Update Wegstein data
        wegsteinData[key].Update(x_curr, y_new);

        // Check convergence
        double absError = std::abs(y_new - x_curr);
        double relError =
            std::abs(x_curr) > 1e-12 ? absError / std::abs(x_curr) : absError;

        // More strict convergence: BOTH criteria must be met
        bool thisVarConverged =
            (absError <= maxAbsError) && (relError <= maxRelError);

        if (thisVarConverged) {
          convergedCount++;
        } else {
          allConverged = false;
        }
      }
    }
  }

  return allConverged;
}

void WegsteinRunner::ApplyWegsteinAcceleration(
    const std::vector<Ref<CalculationBlock>> &blocks,
    const std::vector<Ref<Connector>> &tearConnectors,
    std::map<std::string, WegsteinData> &wegsteinData) {
  for (auto &tearConn : tearConnectors) {
    auto targetBlock = FindBlock(blocks, tearConn->GetTargetId());
    if (targetBlock.IsNull())
      continue;

    auto originBlock = FindBlock(blocks, tearConn->GetOriginId());
    if (originBlock.IsNull())
      continue;

    auto &originPin = originBlock->GetOutputPin(tearConn->GetOriginPin());

    for (auto &values : originPin->GetValuesMap()) {
      std::string key = tearConn->GetOriginId() + ":" +
                        tearConn->GetOriginPin() + ":" + values.first;

      if (wegsteinData.find(key) != wegsteinData.end()) {
        double nextGuess = wegsteinData[key].GetNextGuess();

        // Set the accelerated guess for the next iteration
        targetBlock->SetInputPinValue(tearConn->GetTargetPin(), values.first,
                                      nextGuess);
      }
    }
  }
}

Ref<CalculationBlock>
WegsteinRunner::FindBlock(const std::vector<Ref<CalculationBlock>> &blocks,
                          const std::string &blockId) {
  auto it =
      std::find_if(blocks.begin(), blocks.end(), [&blockId](const auto &block) {
        return block->GetId() == blockId;
      });
  if (it != blocks.end()) {
    return *it;
  }
  // Return a moved Ref with nullptr - this will have IsNull() == true
  Ref<CalculationBlock> nullRef(static_cast<CalculationBlock *>(nullptr));
  return nullRef;
}
