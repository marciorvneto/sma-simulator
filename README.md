# SMA Simulator

A modular simulation engine for process engineering applications, with specialized support for pulp and paper industry processes.

## Project Structure

```
.
├── CMakeLists.txt
├── README.md
├── core/                   # Core simulation engine
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── pulp-and-paper/         # Pulp and paper industry modules
│   ├── CMakeLists.txt
│   ├── include/
│   ├── src/
│   └── vendor/             # Third-party dependencies (git submodules)
└── sandbox/                # Examples and testing
    ├── CMakeLists.txt
    └── src/
```

## Features

- **Modular Architecture**: Core simulation engine with industry-specific modules
- **Process Block Simulation**: Support for various calculation blocks (Evaporators, etc.)
- **Stream Connections**: Connect process blocks with material and energy streams
- **Tear Stream Handling**: Advanced convergence algorithms for recycle streams
- **Extensible Design**: Easy to add new process equipment and calculation methods

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- GDB (for debugging)

### Build Instructions

```bash
# Clone the repository with submodules
git clone --recursive <repository-url>
cd sma-simulator

# If you already cloned without --recursive, initialize submodules:
# git submodule update --init --recursive

# Create build directory
mkdir build
cd build

# Configure (Debug mode for development)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make

# Or for release build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Debug Build

For debugging with GDB:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
gdb ./sandbox/sandbox_executable
```

## Quick Start

The `sandbox/src/main.cpp` provides a complete example of setting up and running a simulation with two connected evaporators:

```cpp
#include "Evaporator.h"
#include "Ref.h"
#include "Simulator.h"

int main() {
  Simulator sim;
  
  // Create evaporator E1
  Ref<CalculationBlock> e1(new Evaporator("E1", {
    {"A", 2100},    // Area
    {"Q", 12000},   // Heat duty
    {"U", 0.5},     // Heat transfer coefficient
    {"D", 25e-3},   // Diameter
  }));
  
  // Set input conditions
  e1->SetInputPinValue("S", "m", 4.0);    // Steam mass flow
  e1->SetInputPinValue("S", "P", 1.1);    // Steam pressure
  e1->SetInputPinValue("F", "T", 86.93);  // Feed temperature
  e1->SetInputPinValue("F", "m", 8.21);   // Feed mass flow
  e1->SetInputPinValue("F", "x", 0.14);   // Feed concentration
  
  // Set calculation method
  e1->SetCalculationMethod(
    Ref<CalculationMethod>(new Evaporator::MethodGivenInletData(e1))
  );
  
  // Create connections between units
  std::vector<Ref<Connector>> conns = {
    Ref<Connector>("E1", "V", "E2", "S"),  // Vapor from E1 to steam of E2
    Ref<Connector>("E2", "L", "E1", "F")   // Liquid from E2 to feed of E1
  };
  
  // Run simulation
  sim.Run(blocks, conns);
  
  // Print results
  for (auto &block : blocks) {
    block->PrintAllValues();
  }
}
```

## Core Components

### Simulator
The main simulation engine that coordinates calculation blocks and stream connections.

### CalculationBlock
Base class for process equipment (evaporators, reactors, separators, etc.). Each block has:
- Input and output pins for material and energy streams
- Configurable parameters
- Multiple calculation methods

### Connectors
Link output pins of one block to input pins of another, enabling material and energy balance calculations across the flowsheet.

### Ref Template
Smart pointer system for memory management and object lifecycle control.

## Key Features

### Tear Streams
Handle recycle streams and iterative convergence:
```cpp
V1->MarkAsTearStream(true);
```

### Multiple Calculation Methods
Each process block can use different calculation approaches:
```cpp
e1->SetCalculationMethod(
  Ref<CalculationMethod>(new Evaporator::MethodGivenInletData(e1))
);
```

### Parameter Configuration
Flexible parameter setting for equipment specifications:
```cpp
Evaporator e1("E1", {
  {"A", 2100},     // Heat transfer area
  {"Q", 12000},    // Heat duty
  {"U", 0.5},      // Overall heat transfer coefficient
});
```

## Development

### Adding New Process Equipment

1. Create a new class inheriting from `CalculationBlock`
2. Implement required calculation methods
3. Define input/output pins and parameters
4. Add to the appropriate module (core or industry-specific)

### Adding New Industries

Create a new subdirectory similar to `pulp-and-paper/` with:
- `CMakeLists.txt`
- `include/` for headers
- `src/` for implementation
- `vendor/` for third-party dependencies

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests in the `sandbox/` directory
4. Ensure all builds pass
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

For questions and support, please [create an issue](link-to-issues) or contact [maintainer-email].
