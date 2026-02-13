#include <cmath>

#include "PowerMismatch.H"

Eigen::VectorXd powerMismatch(
    const Eigen::VectorXd& Ps,
    const Eigen::VectorXd& Qs,
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta,
    int n_bus,
    const std::vector<int>& pq_bus_id,
    Eigen::VectorXd& P,
    Eigen::VectorXd& Q
) {
    P = Eigen::VectorXd::Zero(n_bus);
    Q = Eigen::VectorXd::Zero(n_bus);

    // Compute P(i) and Q(i) at each bus
    for (int i = 0; i < n_bus; ++i) {
        for (int j = 0; j < n_bus; ++j) {
            double dij = delta(i) - delta(j);
            P(i) += V(i) * V(j) * (G(i, j) * std::cos(dij) + B(i, j) * std::sin(dij));
            Q(i) += V(i) * V(j) * (G(i, j) * std::sin(dij) - B(i, j) * std::cos(dij));
        }
    }

    // delta_P for all non-slack buses (bus index 1..N-1, 0-based)
    Eigen::VectorXd delta_P = Ps - P;
    Eigen::VectorXd delta_Q = Qs - Q;

    // Only keep delta_Q for PQ buses
    int n_pq = static_cast<int>(pq_bus_id.size());
    Eigen::VectorXd delta_Q_pq(n_pq);
    for (int k = 0; k < n_pq; ++k) {
        delta_Q_pq(k) = delta_Q(pq_bus_id[k]);
    }

    // Mismatch = [delta_P(2:end); delta_Q(pq)]
    // In 0-based: delta_P(1..N-1)
    Eigen::VectorXd mismatch(n_bus - 1 + n_pq);
    mismatch.head(n_bus - 1) = delta_P.tail(n_bus - 1);
    mismatch.tail(n_pq) = delta_Q_pq;

    return mismatch;
}
