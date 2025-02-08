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
%     main.m
%
% Description
%     MATLAB script for solving steady state power flow.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 5, 2025
%--------------------------------------------------------------------------------
%}


clc
clear all

disp("Choose the method for power flow solution:");
disp("1. Gauss-Seidel");
disp("2. Newton-Raphson");
methodChoice = input("Enter your choice (1 for Gauss-Seidel, 2 for Newton-Raphson): ");

R_coeff = input("Enter the relaxation coefficient (e.g., 1.0): ");
maxIter = input("Enter the maximum number of iterations (e.g., 1024): ");
tolerance = input("Enter the tolerance (e.g., 1E-16): ");

busPath = input("Enter the path for the bus data file: ", "s");
linePath = input("Enter the path for the line data file: ", "s");

busTable = readtable(busPath);
lineTable = readtable(linePath);

busData = [busTable.ID, busTable.Type, busTable.V, busTable.delta, busTable.Pg, busTable.Qg, busTable.Pl, busTable.Ql, busTable.Qgmax, busTable.Qgmin];
lineData = [lineTable.From, lineTable.To, lineTable.R, lineTable.X, lineTable.G, lineTable.B, lineTable.a];

switch methodChoice
    case 1
        disp("Starting Gauss-Seidel method ...");
        [V, P, Q] = GaussSeidel(lineData, busData, R_coeff, maxIter, tolerance);
        disp(V);
        disp(P);
        disp(Q);

    case 2
        disp("Starting Newton-Raphson method ...");
        [V, P, Q] = NewtonRaphson(lineData, busData, maxIter, tolerance);
        disp("Voltage, Active Power, and Reactive Power solutions:");
        disp(V);
        disp(P);
        disp(Q);

    otherwise
        disp("Invalid choice. Please select either 1 for Gauss-Seidel or 2 for Newton-Raphson.");
end
