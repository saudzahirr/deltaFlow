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
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with deltaFlow.  If not, see <https://www.gnu.org/licenses/>.
%
% Script
%     CreateAdmittanceMatrix.m
%
% Description
%     MATLAB routine that calculates the bus admittance matrix
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 5, 2025
%--------------------------------------------------------------------------------
%}


function Ybus = CreateAdmittanceMatrix(lineData, busData)
    % CreateAdmittanceMatrix Calculates the Bus Admittance Matrix (Ybus)
    % with transformer tap settings (a).
    %
    % This function creates the Ybus matrix, which is used in power system 
    % analysis to represent the admittance of a network in terms of its lines 
    % and buses, including transformer tap settings.
    %
    % Input:
    %   lineData - A matrix containing the line data for the power network.
    %              Each row of the matrix represents a line with the following
    %              columns:
    %              [FromBus, ToBus, Resistance (R), Reactance (X), Conductance (G), Susceptance (B), TapSetting (a)].
    %              The matrix must have dimensions nLines x 7.
    %
    % Output:
    %   Ybus - The Bus Admittance Matrix (Ybus) of the power system. The 
    %          matrix has dimensions N x N, where N is the number of buses 
    %          in the system, and represents the network's admittance characteristics.

    R = lineData(:, 3);  % Resistance of the lines
    X = lineData(:, 4);  % Reactance of the lines
    B = 0.5i * lineData(:, 6);  % Susceptance of the lines (half of given B)
    a = lineData(:, 7);  % Tap setting values (transformer tap ratios)

    Z = R + 1i * X;
    Y = 1 ./ Z;

    nLine = length(lineData(:, 1));  % Number of lines
    N = max(max(lineData(:, 1), lineData(:, 2)));  % Total number of buses

    Ybus = zeros(N, N);

    for k = 1:nLine
        % Off-diagonal elements (admittance between two buses)
        Ybus(lineData(k, 1), lineData(k, 2)) = Ybus(lineData(k, 1), lineData(k, 2)) - Y(k) / a(k);
        Ybus(lineData(k, 2), lineData(k, 1)) = Ybus(lineData(k, 1), lineData(k, 2));  % Symmetric matrix

        % Diagonal elements (self-admittance of the buses)
        Ybus(lineData(k, 1), lineData(k, 1)) = Ybus(lineData(k, 1), lineData(k, 1)) + Y(k) / (a(k)^2) + B(k);
        Ybus(lineData(k, 2), lineData(k, 2)) = Ybus(lineData(k, 2), lineData(k, 2)) + Y(k) + B(k);
    end

    G_shunt = busData(:, 11);
    B_shunt = busData(:, 12);

    for n = 1:N
        Ybus(n, n) = Ybus(n, n) + (G_shunt(n) + 1i * B_shunt(n));
    end
end
