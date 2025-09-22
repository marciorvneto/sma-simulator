#pragma once
#include <functional>
#include <vector>

class NDNewtonRaphson {
public:
  // Type definitions for cleaner code
  using VectorFunction =
      std::function<std::vector<double>(const std::vector<double> &)>;
  using JacobianFunction = std::function<std::vector<std::vector<double>>(
      const std::vector<double> &)>;

  struct SolverOptions {
    double tolerance = 1e-10; // Convergence tolerance
    int max_iterations = 100; // Maximum number of iterations
    double h = 1e-8;          // Step size for numerical differentiation
    bool verbose = false;     // Print iteration details
  };

  struct SolverResult {
    std::vector<double> solution;
    int iterations;
    double residual_norm;
    bool converged;
  };

private:
  VectorFunction f_;
  JacobianFunction jacobian_;
  SolverOptions options_;
  bool use_analytical_jacobian_;

  // Private helper methods
  std::vector<std::vector<double>>
  numerical_jacobian(const std::vector<double> &x);
  std::vector<double> solve_linear_system(std::vector<std::vector<double>> A,
                                          std::vector<double> b);
  double vector_norm(const std::vector<double> &v);
  std::vector<double> vector_subtract(const std::vector<double> &a,
                                      const std::vector<double> &b);

public:
  // Constructor with numerical Jacobian only
  explicit NDNewtonRaphson(VectorFunction f, const SolverOptions &options);

  // Constructor with analytical Jacobian
  NDNewtonRaphson(VectorFunction f, JacobianFunction jacobian,
                  const SolverOptions &options);

  // Main solver function
  SolverResult solve(const std::vector<double> &initial_guess);

  // Utility methods
  void set_jacobian(JacobianFunction jacobian);
  void use_numerical_jacobian();
  const SolverOptions &get_options() const;
  bool is_using_analytical_jacobian() const;
  void set_options(const SolverOptions &options);
};
