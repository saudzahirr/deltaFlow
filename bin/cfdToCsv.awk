#!/bin/awk

#---------------------------------*- awk -*-------------------------------------
#
#                                   /\\
#                                  /  \\
#                                 /    \\
#                                /      \\
#                                =========
#
#                                deltaFlow
#                        Power System Analysis Tool
#                           Copyright (C) 2025
#--------------------------------------------------------------------------------
# License
#     This file is part of deltaFlow.
#
#    deltaFlow is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    deltaFlow is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with deltaFlow.  If not, see <https://www.gnu.org/licenses/>.
#
# Script
#     cfdToCsv.awk
#
# Description
#     Script to convert IEEE Common Data Format into structured CSV files for 
#     power flow analysis in the deltaFlow project. It processes bus and branch
#     data, ensuring correct formatting and consistency.
#
# Author
#     Saud Zahir
#
# Date
#     09 December 2024
#--------------------------------------------------------------------------------


BEGIN {
    busCount = 0;
    branchCount = 0;

    busDataColumns = "ID,Name,Type,V,delta,Pg,Qq,Pl,Ql,Qgmax,Qgmin";
    lineDataColumns = "From,To,R,X,G,B,a";
    busTypes["0"] = "3";
    busTypes["1"] = "3";
    busTypes["2"] = "2";
    busTypes["3"] = "1";
}

function strip(s) {
    sub(/^[ \t]+/, "", s);  # Remove leading spaces and tabs
    sub(/[ \t]+$/, "", s);  # Remove trailing spaces and tabs
    return s;
}

NR == 1 {
    outputDir = substr($0, 46, 53);
    gsub(/ /, "", outputDir);   # Remove empty spaces globally
    gsub(/\r/, "", outputDir);  # Remove carriage return
    command = "mkdir -p " outputDir;
    system(command);
    busDataCSV = outputDir "/bus-data.csv";
    lineDataCSV = outputDir "/line-data.csv";

    print busDataColumns > busDataCSV;
    print lineDataColumns > lineDataCSV;
}

/^BUS DATA/ {read = "busData"; next;}
/^-999/ {read = ""; next;}

/^BRANCH DATA/ {read = "branchData"; next;}
/^-999/ {read = ""; next;}

/^LOSS ZONES/ {read = "lossZones"; next;}
/^-99/ {read = ""; next;}

/^INTERCHANGE DATA/ {read = "interchangeData"; next;}
/^-9/ {read = ""; next;}

/^TIE LINES/ {read = "tieLines"; next;}
/^-999/ {read = ""; next;}

read == "busData" && $1 ~/^[0-9]+$/ {
    busCount++;
    busID = strip(substr($0, 1, 4));
    busIndex[busID] = busCount;
    busName[busCount]  = strip(substr($0, 5, 11));
    busType[busCount]  = strip(substr($0, 25, 2));
    voltage[busCount]  = strip(substr($0, 28, 6));
    delta[busCount]    = strip(substr($0, 34, 7));
    pLoad[busCount]    = strip(substr($0, 41, 9));
    qLoad[busCount]    = strip(substr($0, 50, 10));
    pGen[busCount]     = strip(substr($0, 60, 8));
    qGen[busCount]     = strip(substr($0, 68, 8));
    qGMax[busCount]    = strip(substr($0, 91, 8));
    qGMin[busCount]    = strip(substr($0, 99, 8));
}

read == "branchData" && $1 ~/^[0-9]+$/ && $2 ~/^[0-9]+$/ {
    branchCount++;
    fromBus[branchCount] = strip(substr($0, 1, 4));
    toBus[branchCount]   = strip(substr($0, 6, 4));
    R[branchCount]       = strip(substr($0, 20, 10));
    X[branchCount]       = strip(substr($0, 30, 10));
    B[branchCount]       = strip(substr($0, 41, 10));
    a[branchCount]       = strip(substr($0, 77, 6));
}

END {
    for (i = 1; i <= busCount; i++) {
        if (busType[i] != 2) {
            voltage[i] = "1.000";
        }
        delta[i] = 0;

        printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
            i, busName[i], busTypes[busType[i]], voltage[i], delta[i],
            pLoad[i], qLoad[i], pGen[i], qGen[i], qGMax[i], qGMin[i] >> busDataCSV;
    }

    for (i = 1; i <= branchCount; i++) {
        if (a[i] ~ /^0\.0*$/) {
            a[i] = "1.000";
        }

        printf "%s,%s,%s,%s,%s,%s,%s\n",
            busIndex[fromBus[i]], busIndex[toBus[i]], R[i], X[i], "0.00000", B[i], a[i] >> lineDataCSV;
    }
}
