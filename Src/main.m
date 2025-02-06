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

% R_coeff = 1.6;
R_coeff = 1.0; % Relaxation coefficient
maxIter = 1024;
tolerance = 1E-16;
format long;

% Bus Type  Slack = 1, PV = 2, PQ = 3
busTable = readtable("./data/bus-data.csv");
lineTable = readtable("./data/line-data.csv");

busData = [busTable.ID, busTable.Type, busTable.V, busTable.delta, busTable.Pg, busTable.Qg, busTable.Pl, busTable.Ql, busTable.Qgmax, busTable.Qgmin];
lineData = [lineTable.From, lineTable.To, lineTable.R, lineTable.X, lineTable.G, lineTable.B, lineTable.a];

[V, P, Q] = GaussSeidel(lineData, busData, R_coeff, maxIter, tolerance);
disp(V);
disp(P);
disp(Q);
% exit;
