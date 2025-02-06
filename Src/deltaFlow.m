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
%     deltaFlow.m
%
% Description
%     MATLAB GUI application for power flow analysis.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 5, 2025
%--------------------------------------------------------------------------------
%}


classdef deltaFlow < matlab.apps.AppBase

    % Properties for UI components
    properties (Access = public)
        UIFigure             matlab.ui.Figure
        MethodDropDown       matlab.ui.control.DropDown
        BusFileButton        matlab.ui.control.Button
        LineFileButton       matlab.ui.control.Button
        RelaxationEditField  matlab.ui.control.NumericEditField
        MaxIterEditField     matlab.ui.control.NumericEditField
        ToleranceEditField   matlab.ui.control.NumericEditField
        RunButton            matlab.ui.control.Button
        BusTable             matlab.ui.control.Table
        LineTable            matlab.ui.control.Table
        ResultsTable         matlab.ui.control.Table
        LogTextArea          matlab.ui.control.TextArea
    end
    
    % Internal properties for storing data
    properties (Access = private)
        busData
        lineData
        busFilePath
        lineFilePath
    end
    
    methods (Access = private)
        
        % Load Bus Data
        function loadBusData(app)
            [file, path] = uigetfile("*.xlsx;*.csv", "Select Bus Data File");
            if isequal(file, 0)
                app.LogTextArea.Value = ["No bus data file selected."; app.LogTextArea.Value];
                return;
            end
            app.busFilePath = fullfile(path, file);
            app.busData = readtable(app.busFilePath);
            if ~isempty(app.BusTable)
                app.BusTable.Data = app.busData;
            end
            app.LogTextArea.Value = ["Bus data loaded successfully."; app.LogTextArea.Value];
        end
        
        % Load Line Data
        function loadLineData(app)
            [file, path] = uigetfile("*.xlsx;*.csv", "Select Line Data File");
            if isequal(file, 0)
                app.LogTextArea.Value = ["No line data file selected."; app.LogTextArea.Value];
                return;
            end
            app.lineFilePath = fullfile(path, file);
            app.lineData = readtable(app.lineFilePath);
            if ~isempty(app.LineTable)
                app.LineTable.Data = app.lineData;
            end
            app.LogTextArea.Value = ["Line data loaded successfully."; app.LogTextArea.Value];
        end
        
        % Run Power Flow Analysis
        function runPowerFlow(app)
            if isempty(app.busFilePath) || isempty(app.lineFilePath)
                app.LogTextArea.Value = ["Error: Load both bus and line data first!"; app.LogTextArea.Value];
                return;
            end
            
            method = app.MethodDropDown.Value;
            R_coeff = app.RelaxationEditField.Value;
            maxIter = app.MaxIterEditField.Value;
            tolerance = app.ToleranceEditField.Value;

            % Extract only relevant numeric columns from tables
            busDataArray = [app.busData.ID, app.busData.Type, app.busData.V, app.busData.delta, ...
                            app.busData.Pg, app.busData.Qg, app.busData.Pl, app.busData.Ql, ...
                            app.busData.Qgmax, app.busData.Qgmin];

            lineDataArray = [app.lineData.From, app.lineData.To, app.lineData.R, ...
                             app.lineData.X, app.lineData.G, app.lineData.B, app.lineData.a];

            if strcmp(method, "Gauss-Seidel")
                app.LogTextArea.Value = ["Running Gauss-Seidel method..."; app.LogTextArea.Value];
                [V, P, Q] = GaussSeidel(lineDataArray, busDataArray, R_coeff, tolerance, maxIter);
            else
                app.LogTextArea.Value = ["Running Newton-Raphson method..."; app.LogTextArea.Value];
                [V, P, Q] = NewtonRaphson(lineDataArray, busDataArray, tolerance, maxIter);
            end

            % Display Results
            resultsTable = table(V, P, Q, "VariableNames", {"Voltage", "Active Power", "Reactive Power"});
            app.ResultsTable.Data = resultsTable;

            app.LogTextArea.Value = ["Power flow analysis completed!"; app.LogTextArea.Value];
        end
    end

    % Create UI Components
    methods (Access = private)
        function createComponents(app)
            % Main Figure
            app.UIFigure = uifigure("Position", [100, 100, 800, 600]);
            app.UIFigure.Name = "DeltaFlow - Power Flow Analysis";

            % Dropdown for method selection
            app.MethodDropDown = uidropdown(app.UIFigure, ...
                "Items", {"Gauss-Seidel", "Newton-Raphson"}, ...
                "Position", [50, 550, 150, 30], ...
                "Value", "Gauss-Seidel");

            % Buttons for file selection
            app.BusFileButton = uibutton(app.UIFigure, "push", ...
                "Text", "Load Bus Data", ...
                "Position", [50, 500, 150, 30], ...
                "ButtonPushedFcn", @(~, ~) loadBusData(app));

            app.LineFileButton = uibutton(app.UIFigure, "push", ...
                "Text", "Load Line Data", ...
                "Position", [50, 450, 150, 30], ...
                "ButtonPushedFcn", @(~, ~) loadLineData(app));

            % Edit fields for parameters
            app.RelaxationEditField = uieditfield(app.UIFigure, "numeric", ...
                "Position", [300, 550, 100, 30], "Value", 1.0);

            app.MaxIterEditField = uieditfield(app.UIFigure, "numeric", ...
                "Position", [300, 500, 100, 30], "Value", 1024);

            app.ToleranceEditField = uieditfield(app.UIFigure, "numeric", ...
                "Position", [300, 450, 100, 30], "Value", 1e-16);

            % Run Button
            app.RunButton = uibutton(app.UIFigure, "push", ...
                "Text", "Run Power Flow", ...
                "Position", [50, 400, 150, 30], ...
                "ButtonPushedFcn", @(~, ~) runPowerFlow(app));

            % Tables for Data
            app.BusTable = uitable(app.UIFigure, "Position", [50, 250, 700, 100]);
            app.LineTable = uitable(app.UIFigure, "Position", [50, 150, 700, 100]);
            app.ResultsTable = uitable(app.UIFigure, "Position", [50, 50, 700, 100]);

            % Log Text Area
            app.LogTextArea = uitextarea(app.UIFigure, "Position", [450, 450, 300, 130]);
        end
    end

    % App startup
    methods (Access = public)
        function app = deltaFlow()
            createComponents(app);
        end
    end
end
