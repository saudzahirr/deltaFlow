#include <cmath>
#include <limits>
#include <iostream>
#include <vector>

#include "Logger.H"
#include "NewtonRaphson.H"
#include "Jacobian.H"
#include "PowerMismatch.H"
#include "ProgressBar.H"
#include "Data.H"
#include "Utils.H"

bool NewtonRaphson(
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    const Eigen::VectorXd& Ps,
    const Eigen::VectorXd& Qs,
    Eigen::VectorXd& V,
    Eigen::VectorXd& delta,
    int n_bus,
    int n_pq,
    const std::vector<int>& pq_bus_id,
    int maxIter,
    double tolerance
) {
    // Compute initial mismatch
    Eigen::VectorXd P(n_bus), Q(n_bus);
    Eigen::VectorXd mismatch = powerMismatch(Ps, Qs, G, B, V, delta, n_bus, pq_bus_id, P, Q);

    double error = mismatch.cwiseAbs().maxCoeff();
    int iter = 0;

    while (error >= tolerance) {
        if (iter >= maxIter) {
            printConvergenceStatus("Newton-Raphson", false, iter, maxIter, error, tolerance);
            WARN("Newton-Raphson did not converge within {} iterations.", maxIter);
            DEBUG("Final max mismatch was {:.6e}, tolerance is {:.6e}.", error, tolerance);
            return false;
        }
        iter++;

        // Build Jacobian
        Eigen::MatrixXd J = computeJacobian(V, delta, n_bus, n_pq, pq_bus_id, G, B, P, Q);

        // Solve J * correction = mismatch
        Eigen::VectorXd correction = J.colPivHouseholderQr().solve(mismatch);

        // Update delta for non-slack buses (indices 1..N-1)
        for (int i = 1; i < n_bus; ++i) {
            delta(i) += correction(i - 1);
        }

        // Update V for PQ buses only
        for (int k = 0; k < n_pq; ++k) {
            V(pq_bus_id[k]) += correction(n_bus - 1 + k);
        }

        // Recompute mismatch
        mismatch = powerMismatch(Ps, Qs, G, B, V, delta, n_bus, pq_bus_id, P, Q);

        error = mismatch.cwiseAbs().maxCoeff();
        printIterationProgress("Newton-Raphson", iter, maxIter, error, tolerance);
        DEBUG("NR iteration {}: max mismatch = {:.16e}", iter, error);
    }

    printConvergenceStatus("Newton-Raphson", true, iter, maxIter, error, tolerance);
    DEBUG("Newton-Raphson converged in {} iterations with max mismatch {:.6e}", iter, error);
    return true;
}
