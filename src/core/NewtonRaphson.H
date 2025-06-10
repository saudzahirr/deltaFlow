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
 * @brief Declaration of the Newton-Raphson load flow solver for power system analysis.
 *
 * The Newton-Raphson method is a robust and widely-used iterative algorithm for solving 
 * the nonlinear power flow equations in electrical power systems.
 *
 * It uses the bus admittance matrix $$Y_{bus}$$ and iteratively updates the bus voltages 
 * $$V$$ by solving the set of nonlinear equations:
 *
 * $$\Delta x^{(k)} = -[J(x^{(k)})]^{-1} F(x^{(k)})$$
 *
 * where:
 *   - $$x$$ is the state vector (bus voltage magnitudes and angles),
 *   - $$F(x)$$ is the vector of power mismatch equations,
 *   - $$J(x)$$ is the Jacobian matrix of partial derivatives with respect to $$x$$,
 *   - $$\Delta x$$ is the update to the state vector at iteration $$k$$.
 *
 * The power injection equations at each bus are:
 *
 * **Active power (P):**
 * 
 * $$P_i = \sum_{j=1}^{n} |V_i||V_j|(G_{ij}\cos(\theta_{ij}) + B_{ij}\sin(\theta_{ij}))$$
 *
 * **Reactive power (Q):**
 * 
 * $$Q_i = \sum_{j=1}^{n} |V_i||V_j|(G_{ij}\sin(\theta_{ij}) - B_{ij}\cos(\theta_{ij}))$$
 *
 * where:
 *   - $$G_{ij} + jB_{ij}$$ is the element of the admittance matrix $$Y_{bus}$$,
 *   - $$\theta_{ij} = \delta_i - \delta_j$$ is the voltage angle difference between buses $$i$$ and $$j$$,
 *   - $$V_i, V_j$$ are the voltage magnitudes at buses $$i$$ and $$j$$.
 *
 * The Jacobian matrix has the general block structure:
 * 
 *
 * $$J = \begin{bmatrix} \frac{\partial P}{\partial \delta} & \frac{\partial P}{\partial |V|} \\\\ \frac{\partial Q}{\partial \delta} & \frac{\partial Q}{\partial |V|} \end{bmatrix}$$
 *
 *
 *  with submatrices defined as follows:
 *
 * $$J_1(kn) = \frac{\partial P_k}{\partial \delta_n} = \begin{cases} -V_k \sum_{n=1}^N V_n |Y_{kn}| \sin(\delta_k - \delta_n - \theta_{kn}) & \text{if } k = n \\ V_k V_n |Y_{kn}| \sin(\delta_k - \delta_n - \theta_{kn}) & \text{if } k \ne n \end{cases}$$
 * 
 *
 * $$J_2(kn) = \frac{\partial P_k}{\partial |V_n|} = \begin{cases} V_k |Y_{kk}| \cos(\theta_{kk}) + \sum_{n \ne k} V_n |Y_{kn}| \cos(\delta_k - \delta_n - \theta_{kn}) & \text{if } k = n \\ V_k |Y_{kn}| \cos(\delta_k - \delta_n - \theta_{kn}) & \text{if } k \ne n \end{cases}$$
 * 
 *
 * $$J_3(kn) = \frac{\partial Q_k}{\partial \delta_n} = \begin{cases} V_k \sum_{n=1}^N V_n |Y_{kn}| \cos(\delta_k - \delta_n - \theta_{kn}) & \text{if } k = n \\ -V_k V_n |Y_{kn}| \cos(\delta_k - \delta_n - \theta_{kn}) & \text{if } k \ne n \end{cases}$$
 * 
 *
 * $$J_4(kn) = \frac{\partial Q_k}{\partial |V_n|} = \begin{cases} -V_k |Y_{kk}| \sin(\theta_{kk}) + \sum_{n \ne k} V_n |Y_{kn}| \sin(\delta_k - \delta_n - \theta_{kn}) & \text{if } k = n \\ V_k |Y_{kn}| \sin(\delta_k - \delta_n - \theta_{kn}) & \text{if } k \ne n \end{cases}$$
 * 
 * The process continues until all mismatches are within a specified tolerance
 * or the maximum number of iterations is reached.
 */

#ifndef NEWTONRAPHSON_H
#define NEWTONRAPHSON_H

#include <Eigen/Dense>

class BranchData;
class BusData;

/**
  * @brief Solves the power flow equations using the Newton-Raphson iterative method.
  *
  * This function updates the bus voltages in the \ref BusData structure using the Newton-Raphson algorithm.
  *
  * @param Y The bus admittance matrix ($$ Y_{bus} $$).
  * @param busData The bus data, including initial guesses for voltages and specified powers.
  * @param maxIter Maximum number of iterations (default: 1024).
  * @param tolerance Convergence tolerance for power mismatches (default: $$ 1 \times 10^{-8} $$).
  * @return true if the algorithm converged within the specified number of iterations, false otherwise.
  *
  * @note The function modifies the voltages in the \ref BusData structure in-place.
  */
bool NewtonRaphson(const Eigen::MatrixXcd& Y, BusData& busData, int maxIter = 1024, double tolerance = 1E-8);

#endif
