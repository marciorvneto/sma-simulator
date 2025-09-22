#pragma once
#include "CalculationBlock.h"
#include "Connector.h"
#include "Ref.h"
#include "Runner.h"
#include <map>
#include <string>
#include <vector>

// Forward declaration
struct WegsteinData;

class WegsteinRunner : public Runner {
public:
  // Main method to run the Wegstein algorithm
  void Run(const std::vector<Ref<CalculationBlock>> &blocks,
           const std::vector<Ref<Connector>> &connectors);

private:
  // Run sequential calculation for acyclic flowsheets
  void RunSequential(const std::vector<Ref<CalculationBlock>> &blocks,
                     const std::vector<Ref<Connector>> &connectors);

  // Initialize tear stream variables with initial guesses
  void InitializeTearStreams(const std::vector<Ref<CalculationBlock>> &blocks,
                             const std::vector<Ref<Connector>> &tearConnectors,
                             std::map<std::string, WegsteinData> &wegsteinData);

  // Store current tear stream inputs before calculation
  void StoreTearStreamInputs(const std::vector<Ref<CalculationBlock>> &blocks,
                             const std::vector<Ref<Connector>> &tearConnectors,
                             std::map<std::string, WegsteinData> &wegsteinData);

  // Check convergence and update Wegstein data
  bool
  CheckConvergenceAndUpdate(const std::vector<Ref<CalculationBlock>> &blocks,
                            const std::vector<Ref<Connector>> &tearConnectors,
                            std::map<std::string, WegsteinData> &wegsteinData,
                            double maxRelError, double maxAbsError);

  // Apply Wegstein acceleration to get next iteration guesses
  void
  ApplyWegsteinAcceleration(const std::vector<Ref<CalculationBlock>> &blocks,
                            const std::vector<Ref<Connector>> &tearConnectors,
                            std::map<std::string, WegsteinData> &wegsteinData);

  // Helper method to find a block by ID
  Ref<CalculationBlock>
  FindBlock(const std::vector<Ref<CalculationBlock>> &blocks,
            const std::string &blockId);
};
