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
%     RelaxationCoefficient.m
%
% Description
%     MATLAB routine for requesting and validating the relaxation coefficient 
%     used in iterative Gauss-Seidel method.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 8, 2025
%--------------------------------------------------------------------------------
%}

function R_coeff = RelaxationCoefficient()
    % RelaxationCoefficient function requests and validates the relaxation 
    % coefficient input for iterative power flow methods (e.g., Gauss-Seidel).
    %
    % Input:
    %   None (User is prompted for input).
    %
    % Output:
    %   R_coeff - The relaxation coefficient input by the user, which must 
    %             be within the range 0 < R_coeff < 2. The function also 
    %             displays the type of relaxation based on the input value.

    global ERROR WARNING INFO DEBUG CRITICAL;

    R_coeff = input("Enter the relaxation coefficient (0 < R_coeff < 2): ");

    if R_coeff <= 0 || R_coeff >= 2
        WARNING("Invalid input: Relaxation coefficient must be between 0 and 2.");
        R_coeff = RelaxationCoefficient(); 
        return;
    end

    if R_coeff < 1
        CRITICAL("Under-relaxation enabled (R_coeff < 1), this will slow down convergence.");
    elseif R_coeff == 1
        DEBUG("Standard Gauss-Seidel enabled (R_coeff = 1).");
    elseif R_coeff > 1
        DEBUG("Over-relaxation enabled (R_coeff > 1), this will accelerate convergence.");
    end
end
