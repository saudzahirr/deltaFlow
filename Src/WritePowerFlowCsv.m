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


function WritePowerFlowCsv(busData)
    % WritePowerFlowCsv writes bus power flow data to a CSV file,
    % and prints each row using a debug message.
    %
    % Input:
    %   busData - A numeric array with 12 columns arranged as:
    %             [ID, Type, V, delta, Pg, Qg, Pl, Ql, Qgmax, Qgmin, Gs, Bs]
    %
    % The output CSV file "deltaFlow.csv" will have the header:
    %   ID,Type,V,delta,Pg,Qg,Pl,Ql,Qgmax,Qgmin,Gs,Bs
    %
    % Debug output is printed via DEBUG(sprintf(...)).

    global DEBUG parentDir;

    csvPath = fullfile(parentDir, "deltaFlow.csv");

    fid = fopen(csvPath, "w");
    if fid == -1
        error("Cannot open deltaFlow.csv for writing");
    end

    fprintf(fid, "ID,Type,V,delta,Pg,Qg,Pl,Ql,Qgmax,Qgmin,Gs,Bs\n");

    DEBUG(sprintf("%-5s %-5s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s", ...
        "ID", "Type", "V", "delta", "Pg", "Qg", "Pl", "Ql", "Qgmax", "Qgmin", "Gs", "Bs"));

    N = size(busData, 1);

    for n = 1:N
        id    = busData(n, 1);
        type  = busData(n, 2);
        V     = busData(n, 3);
        delta = busData(n, 4);
        Pg    = busData(n, 5);
        Qg    = busData(n, 6);
        Pl    = busData(n, 7);
        Ql    = busData(n, 8);
        Qgmax = busData(n, 9);
        Qgmin = busData(n, 10);
        Gs    = busData(n, 11);
        Bs    = busData(n, 12);

        fprintf(fid, "%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n", ...
            id, type, V, delta, Pg, Qg, Pl, Ql, Qgmax, Qgmin, Gs, Bs);

        DEBUG(sprintf("%-5d %-5d %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f", ...
            id, type, V, delta, Pg, Qg, Pl, Ql, Qgmax, Qgmin, Gs, Bs));
    end

    fclose(fid);
end
