#include <cmath>
#include <complex>
#include <iostream>
#include <limits>

#include "GaussSeidel.H"
#include "Logger.H"

bool GaussSeidel(
    const Eigen::MatrixXcd& Y,
    Eigen::VectorXd& Vmag,
    Eigen::VectorXd& delta,
    const Eigen::VectorXi& type_bus,
    const Eigen::VectorXd& P,
    const Eigen::VectorXd& Q,
    int N,
    int maxIter,
    double tolerance,
    double omega
) {
    // Store scheduled voltage magnitudes for PV buses
    Eigen::VectorXd Vmag_sched = Vmag;

    // Initialize complex voltage vector
    Eigen::VectorXcd V(N);
    for (int i = 0; i < N; ++i)
        V(i) = std::polar(Vmag(i), delta(i));

    int iteration = 0;
    double error = std::numeric_limits<double>::infinity();

    if (omega <= 0.0 || omega >= 2.0) {
        WARN("Invalid input: Relaxation coefficient must be between 0 and 2.");
        DEBUG("Setting Relaxation coefficient to 1.");
        omega = 1.0;
    }

    if (omega < 1.0) {
        CRITICAL("Under-relaxation enabled (omega < 1), this will slow down convergence.");
    }
    else if (omega == 1.0) {
        DEBUG("Standard Gauss-Seidel enabled (omega = 1).");
    }
    else if (omega > 1.0) {
        DEBUG("Over-relaxation enabled (omega > 1), this will accelerate convergence.");
    }

    DEBUG("Relaxation Coefficient :: {}", omega);

    while (error >= tolerance && iteration < maxIter) {
        Eigen::VectorXcd dV = Eigen::VectorXcd::Zero(N);

        for (int n = 0; n < N; ++n) {
            if (type_bus(n) == 1) continue;  // Skip slack bus

            std::complex<double> In = Y.row(n) * V;

            if (type_bus(n) == 2) {  // PV Bus
                // Compute Q from current solution (no clamping)
                double Qn = -std::imag(std::conj(V(n)) * In);

                // GS update maintaining scheduled voltage magnitude
                std::complex<double> I_excl = In - Y(n, n) * V(n);
                std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Qn) / std::conj(V(n)) - I_excl) / Y(n, n);
                std::complex<double> V_corrected = Vmag_sched(n) * V_updated / std::abs(V_updated);
                dV(n) = V_corrected - V(n);
                V(n) = V_corrected;

            } else if (type_bus(n) == 3) {  // PQ Bus
                std::complex<double> I_excl = In - Y(n, n) * V(n);
                std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Q(n)) / std::conj(V(n)) - I_excl) / Y(n, n);
                std::complex<double> V_relaxed = V(n) + omega * (V_updated - V(n));
                dV(n) = V_relaxed - V(n);
                V(n) = V_relaxed;
            }
        }

        error = dV.norm();
        iteration++;
    }

    if (iteration >= maxIter) {
        WARN("Gauss-Seidel did not converge within max iterations ({}).", maxIter);
        DEBUG("Final error norm was {:.6e}, tolerance is {:.6e}.", error, tolerance);
        return false;
    }

    // Extract converged V magnitudes and angles
    for (int i = 0; i < N; ++i) {
        Vmag(i) = std::abs(V(i));
        delta(i) = std::arg(V(i));
    }

    DEBUG("Gauss-Seidel converged in {} iterations with error norm {:.6e}.", iteration, error);

    return true;
}
