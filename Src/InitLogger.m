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
%     InitLogger.m
%
% Description
%     MATLAB routine that initializes a logging system to handle different 
%     log levels and store log messages in a file and console output.
%
% Author(s)
%     Saud Zahir
%
% Date
%     February 8, 2025
%--------------------------------------------------------------------------------
%}


function InitLogger()
    % InitLogger Initializes the logging system with different log levels.
    %
    % This function sets up a logger that writes log messages to a file and
    % prints them to the console. The log levels available are:
    % ERROR, WARNING, INFO, DEBUG, and CRITICAL. Each log level corresponds
    % to different types of messages that can be logged based on their severity.
    % A log file named "deltaFlow.log" is created for storing the messages.
    %
    % Output:
    %   The function does not return any output but initializes global log level
    %   handlers (ERROR, WARNING, INFO, DEBUG, CRITICAL) for use in the script.

    global ERROR WARNING INFO DEBUG CRITICAL;

    logFile = fopen("deltaFlow.log", "w");
    if logFile == -1
        error("Failed to open log file");
    end

    ERROR = @(message) logMessage(1, message, logFile);    % ERROR level
    WARNING = @(message) logMessage(2, message, logFile);  % WARNING level
    INFO = @(message) logMessage(3, message, logFile);     % INFO level
    DEBUG = @(message) logMessage(4, message, logFile);    % DEBUG level
    CRITICAL = @(message) logMessage(5, message, logFile); % CRITICAL level

    INFO("---------------------------------*- MATLAB -*-----------------------------------");
    INFO("");
    INFO("                                   /\\");
    INFO("                                  /  \\");
    INFO("                                 /    \\");
    INFO("                                /      \\");
    INFO("                                =========");
    INFO("");
    INFO("                                deltaFlow");
    INFO("                        Power System Analysis Tool");
    INFO("                           Copyright (C) 2025");
    INFO("--------------------------------------------------------------------------------");

end

% Private function
function logMessage(level, message, logFile)
    timestamp = datestr(now, "yyyy-mm-dd HH:MM:SS");

    switch level
        case 1
            logLevel = "ERROR";
        case 2
            logLevel = "WARNING";
        case 3
            logLevel = "INFO";
        case 4
            logLevel = "DEBUG";
        case 5
            logLevel = "CRITICAL";
        otherwise
            logLevel = "UNKNOWN";
    end

    logEntry = sprintf("%s - %-8s - %s\n", timestamp, logLevel, message);

    fprintf(logFile, "%s", logEntry);
    fprintf("%s", logEntry);
end
