#include <cmath>
#include <limits>
#include <vector>

#include "Qlim.H"
#include "Logger.H"
#include "Data.H"

bool checkQlimits(
    const Eigen::VectorXd& V,
    const Eigen::VectorXd& delta,
    Eigen::VectorXi& type_bus,
    const Eigen::MatrixXd& G,
    const Eigen::MatrixXd& B,
    BusData& busData,
    const std::vector<int>& pv_bus_id,
    int n_bus
) {
    // Q-limits: zero means no limit, using +/-inf
    Eigen::VectorXd Qmax = busData.Qgmax;
    Eigen::VectorXd Qmin = busData.Qgmin;

    for (int i = 0; i < n_bus; ++i) {
        if (Qmax(i) == 0.0) Qmax(i) = std::numeric_limits<double>::infinity();
        if (Qmin(i) == 0.0) Qmin(i) = -std::numeric_limits<double>::infinity();
    }

    // Compute reactive power Q at each bus with converged V and delta
    Eigen::VectorXd Q = Eigen::VectorXd::Zero(n_bus);
    for (int i = 0; i < n_bus; ++i) {
        for (int j = 0; j < n_bus; ++j) {
            double dij = delta(i) - delta(j);
            Q(i) += V(i) * V(j) * (G(i, j) * std::sin(dij) - B(i, j) * std::cos(dij));
        }
    }

    // Qg = Q_calc + Ql (all in p.u.)
    Eigen::VectorXd Qg = Q + busData.Ql;

    // Check Q-limits only for PV buses
    bool qlim_hit = false;

    for (int idx : pv_bus_id) {
        if (Qg(idx) > Qmax(idx)) {
            type_bus(idx) = 3;  // PV to PQ
            busData.Qg(idx) = Qmax(idx);  // Fix Q at limit for next solver run
            qlim_hit = true;
            DEBUG("Q-limit (max) hit at bus {} : Qg = {:.4f} > Qmax = {:.4f}", idx + 1, Qg(idx), Qmax(idx));
        } else if (Qg(idx) < Qmin(idx)) {
            type_bus(idx) = 3;  // PV to PQ
            busData.Qg(idx) = Qmin(idx);  // Fix Q at limit for next solver run
            qlim_hit = true;
            DEBUG("Q-limit (min) hit at bus {} : Qg = {:.4f} < Qmin = {:.4f}", idx + 1, Qg(idx), Qmin(idx));
        }
    }

    if (!qlim_hit) {
        DEBUG("Power flow converged without hitting Q-limits.");
    }

    return qlim_hit;
}
