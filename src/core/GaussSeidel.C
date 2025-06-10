#include <complex>
#include <iostream>

#include "GaussSeidel.H"
#include "Logger.H"
#include "Reader.H"
#include "Utils.H"

bool GaussSeidel(const Eigen::MatrixXcd& Y, BusData& busData, int maxIter, double tolerance, double omega) {
    int N = Y.rows();
    const Eigen::VectorXi& type_bus = busData.Type;

    Eigen::VectorXd& Vmag = busData.V;
    Eigen::VectorXd& delta = busData.delta;
    delta *= M_PI / 180.0; // Convert to radians

    Eigen::VectorXd& Pg = busData.Pg;
    Eigen::VectorXd& Qg = busData.Qg;
    const Eigen::VectorXd& Pl = busData.Pl;
    const Eigen::VectorXd& Ql = busData.Ql;
    const Eigen::VectorXd& Qmax = busData.Qgmax;
    const Eigen::VectorXd& Qmin = busData.Qgmin;

    Eigen::VectorXd P = Pg - Pl;
    Eigen::VectorXd Q = Qg - Ql;

    // Initialize voltage as complex vector
    Eigen::VectorXcd V(N);
    for (int i = 0; i < N; ++i)
        V(i) = std::polar(Vmag(i), delta(i));

    int iteration = 0;
    double error = std::numeric_limits<double>::infinity();

    DEBUG("Relaxation Coefficient :: {}", omega);

    while (error >= tolerance && iteration < maxIter) {
        Eigen::VectorXcd dV = Eigen::VectorXcd::Zero(N);

        for (int n = 1; n < N; ++n) { // Skip slack bus
            std::complex<double> In = Y.row(n) * V;

            if (type_bus(n) == 2) {  // PV Bus
                Q(n) = -std::imag(std::conj(V(n)) * In);

                if (Q(n) >= Qmin(n) && Q(n) <= Qmax(n)) {
                    std::complex<double> I_excl = In - Y(n, n) * V(n);
                    std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Q(n)) / std::conj(V(n)) - I_excl) / Y(n, n);
                    std::complex<double> V_corrected = Vmag(n) * V_updated / std::abs(V_updated);
                    dV(n) = V_corrected - V(n);
                    V(n) = V_corrected;
                } else {
                    Q(n) = std::clamp(Q(n), Qmin(n), Qmax(n));
                    std::complex<double> I_excl = In - Y(n, n) * V(n);
                    std::complex<double> V_updated = ((P(n) - std::complex<double>(0, 1) * Q(n)) / std::conj(V(n)) - I_excl) / Y(n, n);
                    std::complex<double> V_relaxed = V(n) + omega * (V_updated - V(n));
                    dV(n) = V_relaxed - V(n);
                    V(n) = V_relaxed;
                }

            } else if (type_bus(n) == 3) { // PQ Bus
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

    // Recalculate slack bus power injection
    std::complex<double> I_slack = Y.row(0) * V;
    std::complex<double> S_slack = V(0) * std::conj(I_slack);
    P(0) = S_slack.real();
    Q(0) = S_slack.imag();

    // Recalculate reactive power for PV buses
    for (int i = 0; i < N; ++i) {
        if (type_bus(i) == 2) {
            std::complex<double> In = Y.row(i) * V;
            Q(i) = -std::imag(std::conj(V(i)) * In);
        }
    }

    // Update bus data: V magnitude, angle (degrees), Pg, Qg
    for (int i = 0; i < N; ++i) {
        busData.V(i) = std::abs(V(i));
        busData.delta(i) = std::arg(V(i)) * 180.0 / M_PI;
        busData.Pg(i) = P(i) + Pl(i);
        busData.Qg(i) = Q(i) + Ql(i);
    }

    // Calculate total power losses in the system
    double PLoss = busData.Pg.sum() - busData.Pl.sum();
    double QLoss = busData.Qg.sum() - busData.Ql.sum();

    DEBUG("Total real power loss: {}", PLoss);
    DEBUG("Total reactive power loss: {}", QLoss);

    DEBUG("Gauss-Seidel converged in {} iterations with max mismatch error {}", iteration, error);

    return true;
}
