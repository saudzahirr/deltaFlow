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
%     WritePowerFlowCsv.m
%
% Description
%     MATLAB routine to exports power flow results to a CSV file.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 8, 2025
%--------------------------------------------------------------------------------
%}


function WritePowerFlowCsv(P, Q, V)
    % WritePowerFlowCsv exports power flow data to a CSV file.
    %
    % This function takes power flow results, including active and reactive power 
    % injections, and voltage phasors, and writes them to a CSV file ("deltaFlow.csv"). 
    % It also prints a formatted debug log for verification.
    %
    % Inputs:
    %   P - (Nx1 vector) Active power injections for N buses.
    %   Q - (Nx1 vector) Reactive power injections for N buses.
    %   V - (Nx1 vector) Complex voltage phasors at each bus.
    %

    global DEBUG
    N = length(V);

    fid = fopen("deltaFlow.csv", "w");

    fprintf(fid, "Bus ID,Voltage,Phase (deg),P,Q\n");
    DEBUG(sprintf("%-10s %-25s %-15s %-15s %-15s %-15s", ...
        "Bus ID", "Voltage Phasor", "Voltage", "Phase (deg)", "P", "Q"));

    for n = 1:N
        magV = abs(V(n));
        phaseV = angle(V(n)) * 180 / pi;

        fprintf(fid, "%d,%.32f,%.32f,%.32f,%.32f\n", ...
            n, magV, phaseV, P(n), Q(n));
        DEBUG(sprintf("%-10d %-25s %-15.8f %-15.2f %-15.6f %-15.6f", ...
            n, num2str(V(n)), magV, phaseV, P(n), Q(n)));
    end

    fclose(fid);
end
