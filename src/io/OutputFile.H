/*
 * Copyright (c) 2024 Saud Zahir
 *
 * This file is part of deltaFlow.
 *
 * deltaFlow is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * deltaFlow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with deltaFlow.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Generates professionally formatted output files (.out, .sta)
 */

#ifndef OUTPUT_FILE_H
#define OUTPUT_FILE_H

#include <chrono>
#include <cmath>
#include <complex>
#include <fstream>
#include <string>
#include <vector>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include <Eigen/Dense>
#include <fmt/chrono.h>
#include <fmt/core.h>

#include "Banner.H"
#include "Data.H"
#include "Version.H"

namespace OutputFile {

/**
 * @brief Returns the current hostname.
 */
inline std::string hostname() {
#ifdef _WIN32
    char buf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD bufLen = sizeof(buf);
    if (GetComputerNameA(buf, &bufLen)) return std::string(buf);
#else
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == 0) return std::string(buf);
#endif
    return "unknown";
}

/**
 * @brief Returns the current timestamp string.
 */
inline std::string timestamp() {
    auto now = std::time(nullptr);
    return fmt::format("{:%d-%b-%Y  %H:%M:%S}", fmt::localtime(now));
}

/**
 * @brief Returns the current date string.
 */
inline std::string dateStr() {
    auto now = std::time(nullptr);
    return fmt::format("{:%d-%b-%Y}", fmt::localtime(now));
}

/**
 * @brief Returns the current time string.
 */
inline std::string timeStr() {
    auto now = std::time(nullptr);
    return fmt::format("{:%H:%M:%S}", fmt::localtime(now));
}

/**
 */
inline bool writeOutputFile(
    const std::string& jobName,
    const std::string& inputFile,
    const std::string& solverName,
    const std::string& formatName,
    const BusData& busData,
    const BranchData& branchData,
    const Eigen::MatrixXcd& Y,
    int iterations,
    double finalError,
    double tolerance,
    double elapsedSec,
    double basemva = 100.0
) {
    std::string outFile = jobName + ".out";
    std::ofstream out(outFile);
    if (!out.is_open()) return false;

    int nBus = busData.V.size();
    int nBranch = branchData.From.size();
    int W = Banner::pageWidth();

    out << Banner::fileBanner();

    out << fmt::format("\n   deltaFlow v{:<32s}Date {:>14s}   Time {:>8s}\n",
        deltaFlow_VERSION, dateStr(), timeStr());
    out << "\n";

    out << Banner::sectionHeader("S Y S T E M   I N F O R M A T I O N");
    out << fmt::format("   Hostname             : {}\n", hostname());
    out << fmt::format("   CMake Version        : {}\n", CMake_VERSION);
    out << fmt::format("   Compiler Version     : GCC {}\n", gcc_VERSION);
    out << fmt::format("   deltaFlow Version    : {}\n", deltaFlow_VERSION);
    out << "\n";

    out << Banner::sectionHeader("J O B   P A R A M E T E R S");
    out << fmt::format("   Job Name             : {}\n", jobName);
    out << fmt::format("   Input File           : {}\n", inputFile);
    out << fmt::format("   Input Format         : {}\n", formatName);
    out << fmt::format("   Solver Method        : {}\n", solverName);
    out << fmt::format("   Convergence Tol.     : {:.6e}\n", tolerance);
    out << "\n";

    out << Banner::sectionHeader("M O D E L   S U M M A R Y");

    int nSlack = 0, nPV = 0, nPQ = 0;
    for (int i = 0; i < nBus; ++i) {
        if (busData.Type(i) == 1) nSlack++;
        else if (busData.Type(i) == 2) nPV++;
        else nPQ++;
    }

    out << fmt::format("   Number of Buses      : {:>6d}\n", nBus);
    out << fmt::format("     Slack Buses         : {:>6d}\n", nSlack);
    out << fmt::format("     PV Buses            : {:>6d}\n", nPV);
    out << fmt::format("     PQ Buses            : {:>6d}\n", nPQ);
    out << fmt::format("   Number of Branches   : {:>6d}\n", nBranch);
    out << fmt::format("   Base MVA             : {:>10.1f}\n", basemva);
    out << "\n";

    out << Banner::sectionHeader("S O L V E R   S U M M A R Y");
    out << fmt::format("   Method               : {}\n", solverName);
    out << fmt::format("   Iterations           : {:>6d}\n", iterations);
    out << fmt::format("   Final Error          : {:.6e}\n", finalError);
    out << fmt::format("   Tolerance            : {:.6e}\n", tolerance);
    out << fmt::format("   Status               : CONVERGED\n");
    out << fmt::format("   Elapsed Time (sec)   : {:.3f}\n", elapsedSec);
    out << "\n";

    out << Banner::sectionHeader("B U S   D A T A   R E S U L T S");
    out << fmt::format("   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "Bus", "Voltage", "Angle", "Load", "Load", "Gen", "Gen", "Injected");
    out << fmt::format("   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "No.", "Mag.", "Degree", "MW", "Mvar", "MW", "Mvar", "Mvar");
    out << "   " << std::string(W - 4, '=') << "\n";

    for (int i = 0; i < nBus; ++i) {
        double injectedMvar = busData.Qg(i) - busData.Ql(i);
        out << fmt::format("   {:>4d}  {:>9.4f}  {:>9.4f}  {:>10.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
            i + 1, busData.V(i), busData.delta(i),
            busData.Pl(i), busData.Ql(i), busData.Pg(i), busData.Qg(i), injectedMvar);
    }

    double totalPl = busData.Pl.sum();
    double totalQl = busData.Ql.sum();
    double totalPg = busData.Pg.sum();
    double totalQg = busData.Qg.sum();
    double totalInjected = totalQg - totalQl;

    out << "   " << std::string(W - 4, '=') << "\n";
    out << fmt::format("   Total{:>27.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
        totalPl, totalQl, totalPg, totalQg, totalInjected);
    out << "\n";

    out << Banner::sectionHeader("L I N E   F L O W   A N D   L O S S E S");
    out << fmt::format("   {:>4s}  {:>4s}  {:>9s} {:>9s} {:>9s}   {:>9s} {:>9s}  {:>9s}\n",
        "From", "To", "MW", "Mvar", "MVA", "Loss MW", "Loss Mvar", "Tap");
    out << "   " << std::string(W - 4, '=') << "\n";

    auto Bc = branchData.B;
    int nLine = nBranch;

    Eigen::VectorXcd Vc(nBus);
    Eigen::VectorXcd S(nBus);
    for (int i = 0; i < nBus; ++i) {
        double mag = busData.V(i);
        double ang_rad = busData.delta(i) * M_PI / 180.0;
        Vc(i) = std::polar(mag, ang_rad);
        double P = busData.Pg(i) - busData.Pl(i);
        double Q = busData.Qg(i) - busData.Ql(i);
        S(i) = std::complex<double>(P, Q);
    }

    std::complex<double> SLT = 0.0;

    for (int n = 1; n <= nBus; ++n) {
        int n_idx = n - 1;
        bool busprt = false;

        for (int L = 0; L < nLine; ++L) {
            if (!busprt) {
                double P_inj = busData.Pg(n_idx) - busData.Pl(n_idx);
                double Q_inj = busData.Qg(n_idx) - busData.Ql(n_idx);
                double S_mag = std::abs(S(n_idx)) * basemva;
                out << fmt::format("   {:>4d}        {:>9.3f} {:>9.3f} {:>9.3f}\n",
                    n, P_inj, Q_inj, S_mag);
                busprt = true;
            }

            auto writeLineFlow = [&](int from, int to, int L) {
                int f_idx = from - 1;
                int t_idx = to - 1;
                double aL = (branchData.tapRatio(L) == 0.0) ? 1.0 : branchData.tapRatio(L);

                std::complex<double> In, Ik;
                if (branchData.From(L) == from) {
                    In = (Vc(f_idx) - aL * Vc(t_idx)) * Y(L) / (aL * aL) + Bc(L) / (aL * aL) * Vc(f_idx);
                    Ik = (Vc(t_idx) - Vc(f_idx) / aL) * Y(L) + Bc(L) * Vc(t_idx);
                } else {
                    In = (Vc(f_idx) - Vc(t_idx) / aL) * Y(L) + Bc(L) * Vc(f_idx);
                    Ik = (Vc(t_idx) - aL * Vc(f_idx)) * Y(L) / (aL * aL) + Bc(L) / (aL * aL) * Vc(t_idx);
                }
                std::complex<double> Snk = Vc(f_idx) * std::conj(In) * basemva;
                std::complex<double> Skn = Vc(t_idx) * std::conj(Ik) * basemva;
                std::complex<double> SL = Snk + Skn;
                SLT += SL;

                if (aL != 1.0) {
                    out << fmt::format("         {:>4d}  {:>9.3f} {:>9.3f} {:>9.3f}   {:>9.3f} {:>9.3f}  {:>9.3f}\n",
                        to, std::real(Snk), std::imag(Snk), std::abs(Snk),
                        std::real(SL), std::imag(SL), aL);
                } else {
                    out << fmt::format("         {:>4d}  {:>9.3f} {:>9.3f} {:>9.3f}   {:>9.3f} {:>9.3f}\n",
                        to, std::real(Snk), std::imag(Snk), std::abs(Snk),
                        std::real(SL), std::imag(SL));
                }
            };

            if (branchData.From(L) == n) {
                writeLineFlow(n, branchData.To(L), L);
            } else if (branchData.To(L) == n) {
                writeLineFlow(n, branchData.From(L), L);
            }
        }
    }

    SLT /= 2.0;
    out << "\n";
    out << fmt::format("   Total loss                        {:>9.3f} {:>9.3f}\n",
        std::real(SLT), std::imag(SLT));
    out << "\n";

    out << "\n";
    out << "\n";
    out << "     JOB TIME SUMMARY\n";
    out << fmt::format("       TOTAL CPU TIME (SEC) = {:>12.5f}\n", elapsedSec);
    out << fmt::format("       WALLCLOCK TIME (SEC) = {:>12d}\n", static_cast<int>(std::round(elapsedSec)));
    out << "\n";

    out << Banner::sectionHeader("A N A L Y S I S   C O M P L E T E");
    out << Banner::center("THE ANALYSIS HAS BEEN COMPLETED SUCCESSFULLY") << "\n";
    out << "\n";

    out.close();
    return true;
}

/**
 * @brief Writes the status file (.sta) — compact solver summary
 */
inline bool writeStatusFile(
    const std::string& jobName,
    const std::string& inputFile,
    const std::string& solverName,
    const std::string& formatName,
    int nBus,
    int nBranch,
    int iterations,
    double finalError,
    double tolerance,
    bool converged,
    double elapsedSec
) {
    std::string staFile = jobName + ".sta";
    std::ofstream out(staFile);
    if (!out.is_open()) return false;

    out << fmt::format("deltaFlow v{:<36s}Date {:>14s} Time {:>8s}\n",
        deltaFlow_VERSION, dateStr(), timeStr());

    out << " SUMMARY OF JOB INFORMATION:\n";
    out << fmt::format(" {:>6s} {:>6s} {:>10s} {:>12s} {:>12s}  {:>8s}\n",
        "ITER", "STATUS", "ERROR", "TOLERANCE", "ELAPSED", "RESULT");

    out << fmt::format(" {:>6d} {:>6s} {:>10.3e} {:>12.3e} {:>12.3f}  {:>8s}\n",
        iterations,
        converged ? "CONV" : "FAIL",
        finalError,
        tolerance,
        elapsedSec,
        converged ? "OK" : "FAILED");

    out << "\n";
    if (converged) {
        out << " THE ANALYSIS HAS COMPLETED SUCCESSFULLY\n";
    } else {
        out << " THE ANALYSIS HAS FAILED TO CONVERGE\n";
    }

    out << "\n";
    out << "Jobinfo File:\n";
    out << fmt::format("Inputfile: {}\n", inputFile);
    out << fmt::format("Solver: {}\n", solverName);
    out << fmt::format("Format: {}\n", formatName);
    out << fmt::format("Hostname: {}\n", hostname());
    out << "\n";

    out << "solver and hardware info\n";
    out << "------------------------\n";
    out << fmt::format("version           : {}\n", deltaFlow_VERSION);
    out << fmt::format("compiler          : GCC {}\n", gcc_VERSION);
    out << fmt::format("cmake             : {}\n", CMake_VERSION);
    out << fmt::format("hostname          : {}\n", hostname());
    out << "\n";

    out << "model info\n";
    out << "----------\n";
    out << fmt::format(" # of buses       : {}\n", nBus);
    out << fmt::format(" # of branches    : {}\n", nBranch);
    out << "\n";

    out << "run and timing info\n";
    out << "-------------------\n";
    out << fmt::format("simulation end    : {}\n", timestamp());
    out << fmt::format("elapsed    time   : {:.3f} seconds\n", elapsedSec);
    out << fmt::format("iterations        : {}\n", iterations);
    out << fmt::format("final error       : {:.6e}\n", finalError);
    out << fmt::format("termination status: {}\n", converged ? "normal" : "failed");
    out << "\n";

    out.close();
    return true;
}

/**
 * @brief Writes the message file (.msg)
 */
inline bool writeMessageFile(
    const std::string& jobName,
    const std::string& solverName,
    const std::vector<std::pair<int, double>>& iterationHistory,
    double tolerance,
    bool converged
) {
    std::string msgFile = jobName + ".msg";
    std::ofstream out(msgFile);
    if (!out.is_open()) return false;

    out << Banner::fileBanner();

    out << fmt::format("\n   deltaFlow v{:<32s}Date {:>14s}   Time {:>8s}\n",
        deltaFlow_VERSION, dateStr(), timeStr());
    out << "\n";

    out << Banner::sectionHeader("I T E R A T I O N   H I S T O R Y");

    out << fmt::format("   {:>6s}  {:>16s}  {:>12s}  {:>8s}\n",
        "Iter", "Max Mismatch", "Tolerance", "Status");
    out << "   " << std::string(Banner::pageWidth() - 4, '-') << "\n";

    for (const auto& [iter, error] : iterationHistory) {
        std::string status = (error < tolerance) ? "CONV" : "";
        out << fmt::format("   {:>6d}  {:>16.6e}  {:>12.6e}  {:>8s}\n",
            iter, error, tolerance, status);
    }

    out << "   " << std::string(Banner::pageWidth() - 4, '-') << "\n";
    out << "\n";

    if (converged) {
        out << "   " << solverName << " CONVERGED\n";
    } else {
        out << "   *** WARNING: " << solverName << " DID NOT CONVERGE\n";
    }

    out << "\n";
    out.close();
    return true;
}

/**
 * @brief Writes the .dat file
 */
inline bool writeDatFile(
    const std::string& jobName,
    const std::string& inputFile,
    const std::string& solverName,
    const std::string& formatName,
    const BusData& busData,
    const BranchData& branchData,
    const std::vector<std::pair<int, double>>& iterationHistory,
    int totalIterations,
    double finalError,
    double tolerance,
    bool converged,
    double elapsedSec,
    double basemva = 100.0
) {
    std::string datFile = jobName + ".dat";
    std::ofstream out(datFile);
    if (!out.is_open()) return false;

    int nBus = busData.V.size();
    int nBranch = branchData.From.size();
    int W = Banner::pageWidth();

    out << Banner::fileBanner();
    out << fmt::format("\n   deltaFlow v{:<32s}Date {:>14s}   Time {:>8s}\n",
        deltaFlow_VERSION, dateStr(), timeStr());

    out << Banner::sectionHeader("I N P U T   P R O C E S S I N G");
    out << fmt::format("   Input File           : {}\n", inputFile);
    out << fmt::format("   Input Format         : {}\n", formatName);

    int nSlack = 0, nPV = 0, nPQ = 0;
    for (int i = 0; i < nBus; ++i) {
        if (busData.Type(i) == 1) nSlack++;
        else if (busData.Type(i) == 2) nPV++;
        else nPQ++;
    }

    out << fmt::format("   Number of Buses      : {:>6d}\n", nBus);
    out << fmt::format("     Slack Buses        : {:>6d}\n", nSlack);
    out << fmt::format("     PV Buses           : {:>6d}\n", nPV);
    out << fmt::format("     PQ Buses           : {:>6d}\n", nPQ);
    out << fmt::format("   Number of Branches   : {:>6d}\n", nBranch);
    out << fmt::format("   Base MVA             : {:>10.1f}\n", basemva);

    std::string solverBanner;
    for (const char &c : solverName) {
        solverBanner += std::toupper(c);
        solverBanner += ' ';
    }
    out << Banner::sectionHeader(solverBanner + "   S O L V E R   E X E C U T I O N");
    out << fmt::format("   Method               : {}\n", solverName);
    out << fmt::format("   Max Iterations       : {:>6d}\n", static_cast<int>(iterationHistory.size()) > 0 ?
        static_cast<int>(iterationHistory.back().first) : totalIterations);
    out << fmt::format("   Convergence Tol.     : {:.6e}\n", tolerance);
    out << "\n";

    out << fmt::format("   {:>6s}  {:>16s}  {:>12s}  {:>8s}\n",
        "Iter", "Max Mismatch", "Tolerance", "Status");
    out << "   " << std::string(W - 4, '-') << "\n";

    for (const auto& [iter, error] : iterationHistory) {
        std::string status;
        if (error < tolerance)
            status = "CONV";
        else
            status = "----";
        out << fmt::format("   {:>6d}  {:>16.6e}  {:>12.6e}  {:>8s}\n",
            iter, error, tolerance, status);
    }

    out << "   " << std::string(W - 4, '-') << "\n";
    out << "\n";

    if (converged) {
        out << fmt::format("   {} CONVERGED after {} iterations.\n", solverName, totalIterations);
        out << fmt::format("   Final max mismatch = {:.6e}\n", finalError);
    } else {
        out << fmt::format("   *** WARNING: {} DID NOT CONVERGE after {} iterations.\n", solverName, totalIterations);
        out << fmt::format("   Final max mismatch = {:.6e}\n", finalError);
    }

    out << Banner::sectionHeader("B U S   D A T A   R E S U L T S");
    out << fmt::format("   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "Bus", "Voltage", "Angle", "Load", "Load", "Gen", "Gen", "Injected");
    out << fmt::format("   {:>4s}  {:>9s}  {:>9s}  {:>10s} {:>10s}  {:>10s} {:>10s}  {:>10s}\n",
        "No.", "Mag.", "Degree", "MW", "Mvar", "MW", "Mvar", "Mvar");
    out << "   " << std::string(W - 4, '=') << "\n";

    for (int i = 0; i < nBus; ++i) {
        double injectedMvar = busData.Qg(i) - busData.Ql(i);
        out << fmt::format("   {:>4d}  {:>9.4f}  {:>9.4f}  {:>10.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
            i + 1, busData.V(i), busData.delta(i),
            busData.Pl(i), busData.Ql(i), busData.Pg(i), busData.Qg(i), injectedMvar);
    }

    out << "   " << std::string(W - 4, '=') << "\n";

    double totalPl = busData.Pl.sum();
    double totalQl = busData.Ql.sum();
    double totalPg = busData.Pg.sum();
    double totalQg = busData.Qg.sum();
    double totalInjected = totalQg - totalQl;

    out << fmt::format("   Total{:>27.4f} {:>10.4f}  {:>10.4f} {:>10.4f}  {:>10.4f}\n",
        totalPl, totalQl, totalPg, totalQg, totalInjected);

    out << "\n\n";
    out << "     JOB TIME SUMMARY\n";
    out << fmt::format("       TOTAL CPU TIME (SEC) = {:>12.5f}\n", elapsedSec);
    out << fmt::format("       WALLCLOCK TIME (SEC) = {:>12d}\n", static_cast<int>(std::round(elapsedSec)));
    out << "\n";

    out << Banner::sectionHeader("A N A L Y S I S   C O M P L E T E");
    if (converged) {
        out << Banner::center("THE ANALYSIS HAS BEEN COMPLETED SUCCESSFULLY") << "\n";
    } else {
        out << Banner::center("*** THE ANALYSIS HAS NOT CONVERGED ***") << "\n";
    }
    out << "\n";

    out.close();
    return true;
}

}

#endif
