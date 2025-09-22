#include "Numeric.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

// Constructor with numerical Jacobian only
NDNewtonRaphson::NDNewtonRaphson(VectorFunction f, const SolverOptions &options)
    : f_(f), options_(options), use_analytical_jacobian_(false) {}

// Constructor with analytical Jacobian
NDNewtonRaphson::NDNewtonRaphson(VectorFunction f, JacobianFunction jacobian,
                                 const SolverOptions &options)
    : f_(f), jacobian_(jacobian), options_(options),
      use_analytical_jacobian_(true) {}

// Calculate numerical Jacobian using finite differences
std::vector<std::vector<double>>
NDNewtonRaphson::numerical_jacobian(const std::vector<double> &x) {
  size_t n = x.size();
  std::vector<std::vector<double>> J(n, std::vector<double>(n));

  std::vector<double> f_x = f_(x);

  for (size_t j = 0; j < n; ++j) {
    std::vector<double> x_plus_h = x;
    x_plus_h[j] += options_.h;

    std::vector<double> f_x_plus_h = f_(x_plus_h);

    for (size_t i = 0; i < n; ++i) {
      J[i][j] = (f_x_plus_h[i] - f_x[i]) / options_.h;
    }
  }

  return J;
}

// Solve linear system Ax = b using Gaussian elimination with partial pivoting
std::vector<double>
NDNewtonRaphson::solve_linear_system(std::vector<std::vector<double>> A,
                                     std::vector<double> b) {
  size_t n = A.size();

  // Forward elimination with partial pivoting
  for (size_t k = 0; k < n - 1; ++k) {
    // Find pivot
    size_t pivot_row = k;
    for (size_t i = k + 1; i < n; ++i) {
      if (std::abs(A[i][k]) > std::abs(A[pivot_row][k])) {
        pivot_row = i;
      }
    }

    // Swap rows if necessary
    if (pivot_row != k) {
      std::swap(A[k], A[pivot_row]);
      std::swap(b[k], b[pivot_row]);
    }

    // Check for singular matrix
    if (std::abs(A[k][k]) < 1e-15) {
      throw std::runtime_error("Singular matrix encountered in linear solver");
    }

    // Eliminate
    for (size_t i = k + 1; i < n; ++i) {
      double factor = A[i][k] / A[k][k];
      for (size_t j = k; j < n; ++j) {
        A[i][j] -= factor * A[k][j];
      }
      b[i] -= factor * b[k];
    }
  }

  // Back substitution
  std::vector<double> x(n);
  for (int i = n - 1; i >= 0; --i) {
    x[i] = b[i];
    for (size_t j = i + 1; j < n; ++j) {
      x[i] -= A[i][j] * x[j];
    }
    x[i] /= A[i][i];
  }

  return x;
}

// Calculate L2 norm of a vector
double NDNewtonRaphson::vector_norm(const std::vector<double> &v) {
  double sum = 0.0;
  for (double val : v) {
    sum += val * val;
  }
  return std::sqrt(sum);
}

// Vector subtraction
std::vector<double>
NDNewtonRaphson::vector_subtract(const std::vector<double> &a,
                                 const std::vector<double> &b) {
  std::vector<double> result(a.size());
  for (size_t i = 0; i < a.size(); ++i) {
    result[i] = a[i] - b[i];
  }
  return result;
}

// Main solver function
NDNewtonRaphson::SolverResult
NDNewtonRaphson::solve(const std::vector<double> &initial_guess) {
  std::vector<double> x = initial_guess;
  size_t n = x.size();

  SolverResult result;
  result.solution = x;
  result.iterations = 0;
  result.converged = false;

  if (options_.verbose) {
    std::cout << "Starting Newton-Raphson solver with " << n << " variables\n";
    std::cout << std::setprecision(10) << std::scientific;
  }

  for (int iter = 0; iter < options_.max_iterations; ++iter) {
    // Evaluate function at current point
    std::vector<double> f_x = f_(x);

    // Calculate residual norm
    result.residual_norm = vector_norm(f_x);

    if (options_.verbose) {
      std::cout << "Iteration " << iter
                << ": ||f(x)|| = " << result.residual_norm << std::endl;
    }

    // Check convergence
    if (result.residual_norm < options_.tolerance) {
      result.converged = true;
      result.iterations = iter;
      result.solution = x;

      if (options_.verbose) {
        std::cout << "Converged after " << iter << " iterations\n";
      }

      return result;
    }

    // Calculate Jacobian
    std::vector<std::vector<double>> J;
    if (use_analytical_jacobian_) {
      J = jacobian_(x);
    } else {
      J = numerical_jacobian(x);
    }

    // Solve J * delta_x = -f(x)
    std::vector<double> neg_f_x(n);
    for (size_t i = 0; i < n; ++i) {
      neg_f_x[i] = -f_x[i];
    }

    try {
      std::vector<double> delta_x = solve_linear_system(J, neg_f_x);

      // Update solution: x = x + delta_x
      for (size_t i = 0; i < n; ++i) {
        x[i] += delta_x[i];
      }

    } catch (const std::runtime_error &e) {
      if (options_.verbose) {
        std::cout << "Linear solver failed: " << e.what() << std::endl;
      }
      result.iterations = iter;
      result.solution = x;
      return result;
    }
  }

  // Max iterations reached
  result.iterations = options_.max_iterations;
  result.solution = x;

  if (options_.verbose) {
    std::cout << "Maximum iterations (" << options_.max_iterations
              << ") reached\n";
  }

  return result;
}

// Utility methods
void NDNewtonRaphson::set_jacobian(JacobianFunction jacobian) {
  jacobian_ = jacobian;
  use_analytical_jacobian_ = true;
}

void NDNewtonRaphson::use_numerical_jacobian() {
  use_analytical_jacobian_ = false;
}

const NDNewtonRaphson::SolverOptions &NDNewtonRaphson::get_options() const {
  return options_;
}

bool NDNewtonRaphson::is_using_analytical_jacobian() const {
  return use_analytical_jacobian_;
}

void NDNewtonRaphson::set_options(const SolverOptions &options) {
  options_ = options;
}
