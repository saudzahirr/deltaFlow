/*
 * Copyright (c) 2024 saudzahirr
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with deltaFlow; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * @brief Declaration of the Gauss-Seidel load flow solver for power system analysis.
 *
 * The Gauss-Seidel method is an iterative algorithm to solve the power flow equations
 * for bus voltages in a power system. It uses the bus admittance matrix $$ Y_{bus} $$ and iteratively
 * updates the bus voltages $$ V $$ to satisfy the power balance equations:
 *
 * $$ S_i = V_i \sum_{j=1}^N Y_{ij}^* V_j^* $$
 *
 * where:
 * - $$ S_i $$ is the specified complex power at bus $$ i $$
 * - $$ V_i $$ is the voltage at bus $$ i $$
 * - $$ Y_{ij} $$ is the element of the admittance matrix
 *
 * The Gauss-Seidel voltage update for the k-th iteration is:
 * 
 * $$ V_i^{(k+1)} = \frac{1}{Y_{ii}} \left( \frac{S_i^*}{V_i^{*(k)}} - \sum_{j=1, j \neq i}^N Y_{ij} V_j^{(k+1 \text{ or } k)} \right) $$
 *
 * For voltage-controlled buses (PV buses), the reactive power injection $$ Q_k $$ is unknown initially
 * but can be computed at iteration i by:
 * 
 * $$ Q_k^{(i)} = V_k^{(i)} \sum_{n=1}^N |Y_{kn}| V_n^{(i)} \sin \left( \delta_k^{(i)} - \delta_n^{(i)} - \theta_{kn} \right) $$
 *
 * The total reactive power generation at bus k is:
 * 
 * $$ Q_{Gk} = Q_k + Q_{Lk} $$
 *
 * where $$( Q_{Lk} )$$ is the reactive load demand at the bus.
 *
 * If the calculated $$( Q_{Gk} )$$ remains within its specified limits (e.g., $$ Q_{Gk}^{min} \leq Q_{Gk} \leq Q_{Gk}^{max} $$),
 * the voltage magnitude $$ V_k $$ is fixed at its specified value, and only the voltage angle $$ \delta_k $$ is updated
 * using the Gauss-Seidel formula.
 * This means:
 * 
 * - The updated voltage magnitude $$ V_k^{(i+1)} $$ is set to the specified magnitude,
 * - Only the angle $$ \delta_k^{(i+1)} $$ changes according to the iterative update.
 *
 * If $$( Q_{Gk} )$$ exceeds its limits during any iteration, the bus type is switched from
 * voltage-controlled (PV) to load bus (PQ), with $$ Q_{Gk} $$ fixed at the violated limit (either $$ Q_{Gk}^{max} $$ or $$ Q_{Gk}^{min} $$).
 * Under this condition, the voltage magnitude $$ V_k $$ is no longer fixed and is recalculated by the load flow program.
 *
 * To accelerate convergence, a relaxation (or acceleration) coefficient $$ \omega $$ (typically $$(0 < \omega \leq 1) $$)
 * can be applied to the voltage update:
 * 
 * $$ V_i^{(k+1)} = \omega \times V_i^{(k+1)} + (1 - \omega) \times V_i^{(k)} $$
 *
 * The iteration proceeds until the maximum change in bus voltages between iterations is below a specified tolerance,
 * or the maximum number of iterations is reached.
 */

#ifndef GAUSS_SEIDEL_H
#define GAUSS_SEIDEL_H

#include <Eigen/Dense>

class BranchData;
class BusData;
 
/**
  * @brief Solves the power flow equations using the Gauss-Seidel iterative method.
  *
  * This function updates the bus voltages in the \ref BusData structure using the Gauss-Seidel algorithm.
  *
  * @param Ybus The bus admittance matrix ($$ Y_{bus} $$).
  * @param busData The bus data, including initial guesses for voltages and specified powers.
  * @param maxIter Maximum number of iterations (default: 1024).
  * @param tolerance Convergence tolerance for bus voltage updates (default: $$ 1 \times 10^{-8} $$).
  * @param omega Relaxation parameter for the Successive Over-Relaxation (SOR) method (default: 1.0; SOR not applied).
  * @return true if the algorithm converged within the specified number of iterations, false otherwise.
  *
  * @note The function modifies the voltages in the \ref BusData structure in-place.
  */
bool GaussSeidel(const Eigen::MatrixXcd& Ybus, BusData& busData, int maxIter = 1024, double tolerance = 1E-8, double omega = 1.0);
 
#endif
