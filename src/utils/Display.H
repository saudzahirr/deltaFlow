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
 * @brief Display and formatting utilities for terminal and file output.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include "Version.H"

/**
 * @namespace Display
 * @brief Terminal and file output formatting for deltaFlow.
 *
 * Provides colored terminal banners, plain-text banners for log/output files,
 * section headers, and common formatting constants.
 */
namespace Display {
    constexpr fmt::rgb BRAND_COLOR{153, 0, 204};   ///< deltaFlow brand color
    constexpr int pageWidth = 80;                   ///< Standard output page width

    /**
     * @brief Returns a separator line of the given character.
     * @param ch Fill character (default: '=').
     * @return String of pageWidth characters.
     */
    inline std::string separator(char ch = '=') {
        return std::string(pageWidth, ch);
    }

    /**
     * @brief Returns a centered string within the page width.
     * @param text Text to center.
     * @return Padded string.
     */
    inline std::string center(const std::string& text) {
        int pad = (pageWidth - static_cast<int>(text.size())) / 2;
        if (pad <= 0) return text;
        return std::string(pad, ' ') + text;
    }

    /**
     * @brief Returns the machine hostname.
     * @return Hostname string, or "unknown" on failure.
     */
    inline std::string hostname() {
        char buf[256];
    #ifdef _WIN32
        DWORD len = sizeof(buf);
        if (GetComputerNameA(buf, &len)) return std::string(buf);
    #else
        if (gethostname(buf, sizeof(buf)) == 0) return std::string(buf);
    #endif
        return "unknown";
    }

    /**
     * @brief Returns the plain-text license notice box.
     */
    inline std::string licenseNotice() {
        constexpr int W = 67;
        constexpr int inner = W - 2;

        auto star = [&](const std::string& text) -> std::string {
            int pad = inner - static_cast<int>(text.size());
            if (pad < 0) pad = 0;
            return " *" + text + std::string(pad, ' ') + "*\n";
        };
        auto rule = [&]() -> std::string {
            return " " + std::string(W, '*') + "\n";
        };
        auto mid = [&](const std::string& text) -> std::string {
            int left = (inner - static_cast<int>(text.size())) / 2;
            if (left < 0) left = 0;
            return std::string(left, ' ') + text;
        };

        std::string s;
        s += rule();
        s += star("");
        s += star(mid(R"(/\\)"));
        s += star(mid(R"(/  \\)"));
        s += star(mid(R"(/    \\)"));
        s += star(mid(R"(/      \\)"));
        s += star(mid("========="));
        s += star(mid("deltaFlow  v" + std::string(deltaFlow_VERSION)));
        s += star("");
        s += star("  Copyright (c) 2024 Saud Zahir.");
        s += star("  All rights reserved.");
        s += star("");
        s += star("  deltaFlow is free software: you can redistribute it");
        s += star("  and/or modify it under the terms of the GNU General");
        s += star("  Public License as published by the Free Software");
        s += star("  Foundation, either version 3 of the License, or");
        s += star("  (at your option) any later version.");
        s += star("");
        s += star("  deltaFlow is distributed in the hope that it will be useful,");
        s += star("  but WITHOUT ANY WARRANTY; without even the implied warranty");
        s += star("  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
        s += star("  See the GNU General Public License for more details.");
        s += star("");
        s += star("  <https://www.gnu.org/licenses/>");
        s += star("");
        s += rule();
        return s;
    }

    /**
     * @brief Returns the plain-text product info box.
     */
    inline std::string productBox() {
        constexpr int W = 59;

        auto line = [&](const std::string& text) -> std::string {
            int pad = W - 2 - static_cast<int>(text.size());
            if (pad < 0) pad = 0;
            return "            |" + text + std::string(pad, ' ') + "|\n";
        };
        auto rule = [&]() -> std::string {
            return "            |" + std::string(W - 2, '_') + "|\n";
        };

        std::string s;
        s += "             " + std::string(W - 2, '_') + "\n";
        s += line("");
        s += line("  deltaFlow -- Power Flow Analysis Software");
        s += line("  A Program for Steady-State Power System Analysis");
        s += line(fmt::format("  Version : {}", deltaFlow_VERSION));
        s += line(fmt::format("  CMake   : {}", CMake_VERSION));
        s += line(fmt::format("  GCC     : {}", gcc_VERSION));
        s += line("");
        s += line("  Copyright (c) 2024 Saud Zahir");
        s += line("  Licensed under GNU GPL v3.0");
        s += line("");
        s += line(fmt::format("  Hostname  : {}", hostname()));
        s += rule();
        return s;
    }

    /**
     * @brief Returns a full plain-text banner for output/log files.
     */
    inline std::string fileBanner() {
        return licenseNotice() + "\n" + productBox();
    }

    /**
     * @brief Returns a section header for output files.
     * @param title Section title.
     * @return Formatted section header string.
     */
    inline std::string sectionHeader(const std::string& title) {
        std::string s;
        s += "\n";
        s += separator('*') + "\n";
        s += "\n";
        s += center(title) + "\n";
        s += center(std::string(title.size() + 4, '-')) + "\n";
        s += "\n";
        s += separator('*') + "\n";
        return s;
    }

    /**
     * @brief Returns a sub-section header for output files.
     * @param title Sub-section title.
     * @return Formatted sub-section header string.
     */
    inline std::string subSectionHeader(const std::string& title) {
        std::string s;
        s += "\n";
        s += "   " + std::string(title.size() + 4, '-') + "\n";
        s += "   " + title + "\n";
        s += "   " + std::string(title.size() + 4, '-') + "\n";
        return s;
    }


    /**
     * @brief Prints the colored license notice box to terminal.
     */
    inline void printLicenseNotice() {
        constexpr int W = 67;
        constexpr int inner = W - 2;
        constexpr fmt::rgb BORDER{0x5a, 0x71, 0x79};

        auto mid = [&](const std::string& text) -> std::string {
            int left = (inner - static_cast<int>(text.size())) / 2;
            if (left < 0) left = 0;
            return std::string(left, ' ') + text;
        };

        auto starLine = [&](const std::string& text, fmt::rgb color) {
            int pad = inner - static_cast<int>(text.size());
            if (pad < 0) pad = 0;
            fmt::print(fg(BORDER), " *");
            fmt::print(fg(color) | fmt::emphasis::bold, "{}", text);
            fmt::print(fg(BORDER), "{}", std::string(pad, ' '));
            fmt::print(fg(BORDER), "*\n");
        };

        auto rule = [&]() {
            fmt::print(fg(BORDER), " {}\n", std::string(W, '*'));
        };

        rule();
        starLine("",                                                          BORDER);
        starLine(mid(R"(/\\)"),                                               BRAND_COLOR);
        starLine(mid(R"(/  \\)"),                                             BRAND_COLOR);
        starLine(mid(R"(/    \\)"),                                           BRAND_COLOR);
        starLine(mid(R"(/      \\)"),                                         BRAND_COLOR);
        starLine(mid("========="),                                            BRAND_COLOR);
        starLine(mid("deltaFlow  v" + std::string(deltaFlow_VERSION)),        BRAND_COLOR);
        starLine("",                                                          BORDER);
        starLine("  Copyright (c) 2024 Saud Zahir.",                          BORDER);
        starLine("  All rights reserved.",                                    BORDER);
        starLine("",                                                          BORDER);
        starLine("  deltaFlow is free software: you can redistribute it",     BORDER);
        starLine("  and/or modify it under the terms of the GNU General",     BORDER);
        starLine("  Public License as published by the Free Software",        BORDER);
        starLine("  Foundation, either version 3 of the License, or",         BORDER);
        starLine("  (at your option) any later version.",                     BORDER);
        starLine("",                                                          BORDER);
        starLine("  deltaFlow is distributed in the hope that it will be useful,", BORDER);
        starLine("  but WITHOUT ANY WARRANTY; without even the implied warranty",  BORDER);
        starLine("  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.",      BORDER);
        starLine("  See the GNU General Public License for more details.",         BORDER);
        starLine("",                                                          BORDER);
        starLine("  <https://www.gnu.org/licenses/>",                         BORDER);
        starLine("",                                                          BORDER);
        rule();
    }

    /**
     * @brief Prints the colored product info box to terminal.
     */
    inline void printProductBox() {
        constexpr int W = 59;
        std::string host = hostname();

        auto border = [&](const std::string& text) -> std::string {
            int pad = W - 2 - static_cast<int>(text.size());
            if (pad < 0) pad = 0;
            return text + std::string(pad, ' ');
        };

        auto printLine = [&](const std::string& text, fmt::rgb color) {
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "            |");
            fmt::print(fg(color), "{}", border(text));
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "|\n");
        };

        auto printKV = [&](const std::string& key, const std::string& val,
                           fmt::rgb keyColor, fmt::rgb valColor) {
            std::string full = key + val;
            int pad = W - 2 - static_cast<int>(full.size());
            if (pad < 0) pad = 0;
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "            |");
            fmt::print(fg(keyColor), "{}", key);
            fmt::print(fg(valColor) | fmt::emphasis::bold, "{}", val);
            fmt::print("{}", std::string(pad, ' '));
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "|\n");
        };

        constexpr fmt::rgb INFO{100, 200, 255};
        constexpr fmt::rgb VAL{255, 255, 100};
        constexpr fmt::rgb DIM{180, 180, 180};

        fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
            "             {}\n", std::string(W - 2, '_'));

        printLine("",                                                    BRAND_COLOR);
        printLine("  deltaFlow -- Power Flow Analysis Software",         BRAND_COLOR);
        printLine("  A Program for Steady-State Power System Analysis",  DIM);
        printKV("  Version : ", deltaFlow_VERSION,                       INFO, VAL);
        printKV("  CMake   : ", CMake_VERSION,                           INFO, VAL);
        printKV("  GCC     : ", gcc_VERSION,                             INFO, VAL);
        printLine("",                                                    BRAND_COLOR);
        printKV("  Copyright (c) 2024 ", "Saud Zahir",                   DIM, fmt::rgb{255, 165, 0});
        printLine("  Licensed under GNU GPL v3.0",                       fmt::rgb{100, 255, 100});
        printLine("",                                                    BRAND_COLOR);
        printKV("  Hostname  : ", host,                                  INFO, fmt::rgb{255, 200, 100});

        fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
            "            |{}|\n", std::string(W - 2, '_'));
    }

    /**
     * @brief Prints the full colored banner to terminal.
     */
    inline void printTerminalBanner() {
        fmt::print("\n");
        printLicenseNotice();
        fmt::print("\n");
        printProductBox();
        fmt::print("\n");
    }

    /**
     * @brief Prints a colored section header to terminal.
     * @param title Section title.
     */
    inline void printSectionHeader(const std::string& title) {
        fmt::print("\n");
        fmt::print(fg(BRAND_COLOR), "{}\n", separator('*'));
        fmt::print("\n");
        fmt::print(fg(BRAND_COLOR) | fmt::emphasis::bold, "{}\n", center(title));
        fmt::print(fg(BRAND_COLOR), "{}\n", center(std::string(title.size() + 4, '-')));
        fmt::print("\n");
        fmt::print(fg(BRAND_COLOR), "{}\n", separator('*'));
        fmt::print("\n");
    }

}

#endif
