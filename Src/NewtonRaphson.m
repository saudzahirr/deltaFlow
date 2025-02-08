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
%                           Copyright (C) 2025
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
%     NewtonRaphson.m
%
% Description
%     MATLAB routine that solves power flow using the Newton-Raphson
%     iterative method.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 5, 2025
%--------------------------------------------------------------------------------
%}


function [V, P, Q] = NewtonRaphson(lineData, busData, maxIter, tolerance)
    % NewtonRaphson Solves the power flow equations using the Newton-Raphson 
    % iterative method.
    %
    % This function implements the Newton-Raphson method to solve the power 
    % flow problem in a power system. The function iteratively updates bus 
    % voltages based on the power system's admittance matrix and convergence 
    % criteria, until the specified tolerance or maximum iterations are met.
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
    %   maxIter   - Maximum number of iterations allowed for the Newton-Raphson 
    %               method.
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

    Y = CreateAdmittanceMatrix(lineData);
    Ymag = abs(Y);
    theta = angle(Y);
    N = size(Y, 1);

    type = busData(:, 2);
    Vmag = busData(:, 3);
    delta = busData(:, 4); 
    Pg = busData(:, 5);
    Qg = busData(:, 6);
    Pl = busData(:, 7);
    Ql = busData(:, 8);
    Qmax = busData(:, 9);
    Qmin = busData(:, 10);

    V = Vmag .* (cos(delta) + 1i * sin(delta));

    P_net = Pg - Pl; 
    Q_net = Qg - Ql;

    error = Inf;
    iteration = 0;

    while error >= tolerance && iteration < maxIter
        P_estimate = zeros(N, 1);
        Q_estimate = zeros(N, 1);

        for i = 2:N
            for n = 1:N
                P_estimate(i) = P_estimate(i) + Vmag(i) * Vmag(n) * Ymag(i, n) * cos(theta(i, n) + delta(n) - delta(i));
                Q_estimate(i) = Q_estimate(i) - Vmag(i) * Vmag(n) * Ymag(i, n) * sin(theta(i, n) + delta(n) - delta(i));
            end

            % Qlimit check
            if Qmax(i) ~= 0
                if Q_estimate(i) > Qmax(i)
                    Q_estimate(i) = Qmax(i);
                    busData(i, 2) = 3; % PV ---> PQ
                elseif Q_estimate(i) < Qmin(i)
                    Q_estimate(i) = Qmin(i);
                    busData(i, 2) = 3; % PV ---> PQ
                else
                    busData(i, 2) = 2;
                    Vmag(i) = busData(i, 3);
                end
            end
        end

        dP = P_net(2:N) - P_estimate(2:N);
        dQ = Q_net(busData(:, 2) == 3) - Q_estimate(busData(:, 2) == 3);

        %% Jacobian Matrix Computation
        % J1
        J1 = zeros(N, N);
        for i = 1:N
            for n = 1:N
                if n ~= i
                    J1(i, i) = J1(i, i) + Vmag(i) * Vmag(n) * Ymag(i, n) * sin(theta(i, n) + delta(n) - delta(i));
                    J1(i, n) = -Vmag(i) * Vmag(n) * Ymag(i, n) * sin(theta(i, n) + delta(n) - delta(i));
                    J1(n, i) = J1(i, n);
                end
            end
        end
        J11 = J1(busData(:, 2) ~= 1, busData(:, 2) ~= 1);

        % J2
        J2 = zeros(N, N);
        for i = 1:N
            for n = 1:N
                if n ~= i
                    J2(i, i) = J2(i, i) + Vmag(n) * Ymag(i, n) * cos(theta(i, n) + delta(n) - delta(i));
                    J2(i, n) = Vmag(i) * Ymag(i, n) * cos(theta(i, n) + delta(n) - delta(i));
                    J2(n, i) = J2(i, n);
                else
                    J2(i, i) = J2(i, i) + 2 * Vmag(i) * Ymag(i) * cos(theta(i,i));
                end
            end
        end
        J22 = J2(busData(:, 2) ~= 1, busData(:, 2) == 3);

        % J3
        J3 = zeros(N, N);
        for i = 1:N
            for n = 1:N
                if n ~= i
                    J3(i, i) = J3(i, i) + Vmag(i) * Vmag(n) * Ymag(i, n) * cos(theta(i, n) + delta(n) - delta(i));
                    J3(i, n) = -Vmag(i) * Vmag(n) * Ymag(i, n) * cos(theta(i, n) + delta(n) - delta(i));
                    J3(n, i) = J3(i, n);
                end
            end
        end
        J33 = J3(busData(:, 2) == 3, busData(:, 2) ~= 1);

        % J4
        J4 = zeros(N, N);
        for i = 1:N
            for n = 1:N
                if n == i
                    J4(i, i) = J4(i, i) - 2 * Vmag(i) * Ymag(i, i) * sin(theta(i,i));
                else
                    J4(i, i) = J4(i, i) - Vmag(n) * Ymag(i, n) * sin(theta(i, n) + delta(n) - delta(i));
                end
            end
        end

        J44 = J4(busData(:, 2) == 3, busData(:, 2) == 3);

        J = [J11 J22; J33 J44];

        %% Correction Vector Computation
        DF = [dP; dQ];
        DX = J \ DF;

        delta(busData(:, 2) ~= 1) = delta(busData(:, 2) ~= 1) + DX(1:length(find(busData(:, 2) ~= 1)));
        Vmag(busData(:, 2) == 3) = Vmag(busData(:, 2) == 3) + DX(length(find(busData(:, 2) ~= 1)) + 1:end);

        error = norm(DF);
        iteration = iteration + 1;
    end

    %% Slack bus power
    for n = 1:N
        Pg(1) = Pg(1) + Vmag(1) * Vmag(n) * Ymag(1, n) * cos(theta(1, n) + delta(n) - delta(1));
        Qg(1) = Qg(1) - Vmag(1) * Vmag(n) * Ymag(1, n) * sin(theta(1, n) + delta(n) - delta(1));
    end

    %% Reactive power of PV buses
    for i = find(busData(:, 2) == 2)'
        for n = 1:N
            Qg(i) = Qg(i) - Vmag(i) * Vmag(n) * Ymag(i, n) * sin(theta(i, n) + delta(n) - delta(i));      
        end
    end

    %% Line losses
    PL = sum(Pg) - sum(Pl);
    QL = sum(Qg) - sum(Ql);

    P = Pg - Pl; 
    Q = Qg - Ql;
    V = Vmag .* (cos(delta) + 1i * sin(delta));

    fprintf("Results converged after: %d iterations\n", iteration);
    fprintf("Error: %.32f\n", error);

end
