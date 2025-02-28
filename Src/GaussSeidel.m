%{
%---------------------------------*- MATLAB -*-----------------------------------
%
%                                   /\\
%                                  /  \\
%                                 /    \\
%                                /      \\
%                                =========
%
%                                deltaFlow
%                        Power System Analysis Tool
%                           Copyright (c) 2025
%--------------------------------------------------------------------------------
% License
%     This file is part of deltaFlow.
%
%    deltaFlow is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    deltaFlow is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of 
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with deltaFlow. If not, see <https://www.gnu.org/licenses/>.
%
% Script
%     GaussSeidel.m
%
% Description
%     MATLAB routine that solves power flow using the Gauss-Seidel
%     iterative method.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 5, 2025
%--------------------------------------------------------------------------------
%}


function [V, P, Q] = GaussSeidel(lineData, busData, maxIter, tolerance)
    % GaussSeidel Solves the power flow equations using the Gauss-Seidel iterative method.
    %
    % This function implements the Gauss-Seidel method to solve the power flow 
    % problem in a power system. The function iteratively updates bus voltages 
    % based on the power system's admittance matrix and convergence criteria, 
    % until the specified tolerance or maximum iterations are met.
    % 
    % Input:
    %   lineData  - A matrix containing transmission line data with the following 
    %               columns:
    %               [FromBus, ToBus, Resistance (R), Reactance (X), 
    %                Conductance (G), Susceptance (B), TapSetting (a)].
    %               The matrix should be nLines x 7.
    %
    %   busData   - A matrix containing bus data with the following columns:
    %               [BusNumber, BusType, VoltageMagnitude, VoltageAngle, 
    %                Pg, Qg, Pl, Ql, Qmax, Qmin].
    %               The matrix should have N x 10, where N is the number of buses.
    %
    %   maxIter   - Maximum number of iterations allowed for the Gauss-Seidel method.
    %
    %   tolerance - Convergence criterion based on the maximum voltage update 
    %               between iterations. The iteration stops when the voltage 
    %               difference falls below this value.
    %
    % Output:
    %   V - Complex vector representing the bus voltages in the form: 
    %       V = Vmag * exp(j * delta), where Vmag is the voltage magnitude and 
    %       delta is the voltage angle.
    %
    %   P - Vector of active power at each bus, representing the difference 
    %       between generation and load.
    %
    %   Q - Vector of reactive power at each bus, representing the difference 
    %       between generation and load.

    global ERROR WARNING INFO DEBUG CRITICAL;

    R_coeff = RelaxationCoefficient();

    Y = CreateAdmittanceMatrix(lineData, busData);
    Ymag = abs(Y);
    theta = angle(Y);
    N = size(Y, 1);

    type = busData(:,2);
    Vmag = busData(:,3);
    delta = busData(:,4) * pi/180;
    Pg = busData(:,5);
    Qg = busData(:,6);
    Pl = busData(:,7);
    Ql = busData(:,8);
    Qmax = busData(:,9);
    Qmin = busData(:,10);

    V = Vmag .* (cos(delta) + 1i * sin(delta));
    P = Pg - Pl;
    Q = Qg - Ql;

    error = Inf;
    iteration = 0;

    while error >= tolerance && iteration <= maxIter
        % Slack bus voltage is not updated
        for n = 2:N
            if type(n) == 2  % PV Bus
                I = 0;
                for k = 1:N
                    I = I + Y(n, k) * V(k);
                end
                Q(n) = -imag(conj(V(n)) * I);

                % Qlimit check
                if Q(n) >= Qmin(n) && Q(n) <= Qmax(n)
                    I = 0;
                    for k = 1:N
                        if k ~= n
                            I = I + Y(n, k) * V(k);
                        end
                    end

                    V_updated(n) = 1 / Y(n, n) * ((P(n) - 1i * Q(n)) / conj(V(n)) - I);
                    V_corrected(n) = Vmag(n) * V_updated(n) / abs(V_updated(n));
                    dV(n) = V_corrected(n) - V(n);
                    V(n) = V_corrected(n);

                else  % PV ---> PQ bus
                    if Q(n) < Qmin(n)
                        Q(n) = Qmin(n);
                    else
                        Q(n) = Qmax(n);
                    end

                    I = 0;
                    for k = 1:N
                        if k ~= n
                            I = I + Y(n, k) * V(k);
                        end
                    end

                    V_updated(n) = 1 / Y(n, n) * ((P(n) - 1i * Q(n)) / conj(V(n)) - I);
                    V_relaxed(n) = V(n) + R_coeff * (V_updated(n) - V(n));
                    dV(n) = V_relaxed(n) - V(n);
                    V(n) = V_relaxed(n);
                end

            else  % PQ Bus
                I = 0;
                for k = 1:N
                    if k ~= n
                        I = I + Y(n, k) * V(k);
                    end
                end

                V_updated(n) = 1 / Y(n, n) * ((P(n) - 1i * Q(n)) / conj(V(n)) - I);
                V_relaxed(n) = V(n) + R_coeff * (V_updated(n) - V(n));
                dV(n) = V_relaxed(n) - V(n);
                V(n) = V_relaxed(n);
            end
        end

        % Calculate error (convergence criterion)
        error = norm(dV);
        iteration = iteration + 1;
    end

    % Compute Slack Bus Power (Bus 1)
    I = 0;
    for k = 1:N
        I = I + Y(1, k) * V(k);
    end

    S_slack = V(1) * conj(I);
    P(1) = real(S_slack);
    Q(1) = imag(S_slack);

    DEBUG(sprintf("Gauss-Seidel converged after: %d iterations", iteration));
    DEBUG(sprintf("Convergence Discrepancy: %.32f", error));
end
