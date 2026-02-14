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
#include <vector>

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
    constexpr fmt::rgb LOGO_COLOR{153, 0, 204};      ///< deltaFlow logo color
    constexpr int pageWidth = 80;                     ///< Standard output page width
    constexpr int licenseBoxWidth = 67;               ///< License notice box width
    constexpr int productBoxWidth = 59;               ///< Product info box width

    /**
     * @brief Right-pads a string with spaces to the given width.
     * @param text Input string.
     * @param width Target width.
     * @return Padded string (unchanged if text is already wider).
     */
    inline std::string padRight(const std::string& text, int width) {
        int pad = width - static_cast<int>(text.size());
        return (pad > 0) ? text + std::string(pad, ' ') : text;
    }

    /**
     * @brief Centers a string within a given width.
     * @param text Text to center.
     * @param width Target width.
     * @return Centered string.
     */
    inline std::string centerIn(const std::string& text, int width) {
        int left = (width - static_cast<int>(text.size())) / 2;
        return (left > 0) ? std::string(left, ' ') + text : text;
    }

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
        return centerIn(text, pageWidth);
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

    /// A single line in the license notice box.
    struct LicenseLine {
        std::string text;    ///< Line content
        bool        isLogo;  ///< true for logo/brand lines
    };

    /**
     * @brief Returns the license notice content lines.
     *
     * Shared by licenseNotice() (plain-text) and printLicenseNotice() (colored).
     */
    inline std::vector<LicenseLine> licenseContent() {
        constexpr int inner = licenseBoxWidth - 2;
        auto mid = [](const std::string& t) { return centerIn(t, inner); };

        return {
            {"",                                                            false},
            {mid(R"(/\\)"),                                                 true},
            {mid(R"(/  \\)"),                                               true},
            {mid(R"(/    \\)"),                                             true},
            {mid(R"(/      \\)"),                                           true},
            {mid("========="),                                              true},
            {mid("deltaFlow  v" + std::string(deltaFlow_VERSION)),          true},
            {"",                                                            false},
            {"  Copyright (c) 2024 Saud Zahir.",                            false},
            {"  All rights reserved.",                                      false},
            {"",                                                            false},
            {"  deltaFlow is free software: you can redistribute it",       false},
            {"  and/or modify it under the terms of the GNU General",       false},
            {"  Public License as published by the Free Software",          false},
            {"  Foundation, either version 3 of the License, or",           false},
            {"  (at your option) any later version.",                       false},
            {"",                                                            false},
            {"  deltaFlow is distributed in the hope that it will be useful,", false},
            {"  but WITHOUT ANY WARRANTY; without even the implied warranty",   false},
            {"  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.",       false},
            {"  See the GNU General Public License for more details.",          false},
            {"",                                                            false},
            {"  <https://www.gnu.org/licenses/>",                           false},
            {"",                                                            false},
        };
    }

    /// A single line in the product info box.
    struct ProductLine {
        std::string key;       ///< Label or full text
        std::string value;     ///< Value (empty for non-KV lines)
        fmt::rgb    keyColor;  ///< Key/text color (terminal only)
        fmt::rgb    valColor;  ///< Value color (terminal only, ignored when value is empty)
    };

    /**
     * @brief Returns the product info box content lines.
     *
     * Shared by productBox() (plain-text) and printProductBox() (colored).
     */
    inline std::vector<ProductLine> productContent() {
        constexpr fmt::rgb INFO{100, 200, 255};
        constexpr fmt::rgb VAL{255, 255, 100};
        constexpr fmt::rgb DIM{180, 180, 180};

        return {
            {"",                                                   "", LOGO_COLOR,           {}},
            {"  deltaFlow -- Power Flow Analysis Software",        "", LOGO_COLOR,           {}},
            {"  A Program for Steady-State Power System Analysis", "", DIM,                  {}},
            {"  Version : ", deltaFlow_VERSION,                        INFO,                 VAL},
            {"  CMake   : ", CMake_VERSION,                            INFO,                 VAL},
            {"  GCC     : ", gcc_VERSION,                              INFO,                 VAL},
            {"",                                                   "", LOGO_COLOR,           {}},
            {"  Copyright (c) 2024 ", "Saud Zahir",                    DIM,  fmt::rgb{255, 165, 0}},
            {"  Licensed under GNU GPL v3.0",                      "", fmt::rgb{100,255,100},{}},
            {"",                                                   "", LOGO_COLOR,           {}},
            {"  Hostname  : ", hostname(),                             INFO, fmt::rgb{255, 200, 100}},
        };
    }

    /**
     * @brief Returns the plain-text license notice box.
     */
    inline std::string licenseNotice() {
        constexpr int inner = licenseBoxWidth - 2;
        std::string rule = " " + std::string(licenseBoxWidth, '*') + "\n";

        std::string s = rule;
        for (const auto& line : licenseContent())
            s += " *" + padRight(line.text, inner) + "*\n";
        s += rule;
        return s;
    }

    /**
     * @brief Returns the plain-text product info box.
     */
    inline std::string productBox() {
        constexpr int inner = productBoxWidth - 2;

        std::string s;
        s += "             " + std::string(inner, '_') + "\n";
        for (const auto& p : productContent())
            s += "            |" + padRight(p.key + p.value, inner) + "|\n";
        s += "            |" + std::string(inner, '_') + "|\n";
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
        constexpr int inner = licenseBoxWidth - 2;
        constexpr fmt::rgb BORDER{0x5a, 0x71, 0x79};

        auto rule = [&]() {
            fmt::print(fg(BORDER), " {}\n", std::string(licenseBoxWidth, '*'));
        };

        rule();
        for (const auto& line : licenseContent()) {
            fmt::rgb color = line.isLogo ? LOGO_COLOR : BORDER;
            fmt::print(fg(BORDER), " *");
            fmt::print(fg(color) | fmt::emphasis::bold, "{}", line.text);
            fmt::print(fg(BORDER), "{}*\n", padRight("", inner - static_cast<int>(line.text.size())));
        }
        rule();
    }

    /**
     * @brief Prints the colored product info box to terminal.
     */
    inline void printProductBox() {
        constexpr int inner = productBoxWidth - 2;

        auto printLine = [&](const std::string& text, fmt::rgb color) {
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "            |");
            fmt::print(fg(color), "{}", padRight(text, inner));
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "|\n");
        };

        auto printKV = [&](const std::string& key, const std::string& val,
                           fmt::rgb keyColor, fmt::rgb valColor) {
            int pad = inner - static_cast<int>(key.size()) - static_cast<int>(val.size());
            if (pad < 0) pad = 0;
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "            |");
            fmt::print(fg(keyColor), "{}", key);
            fmt::print(fg(valColor) | fmt::emphasis::bold, "{}", val);
            fmt::print("{}", std::string(pad, ' '));
            fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "|\n");
        };

        fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
            "             {}\n", std::string(inner, '_'));

        for (const auto& p : productContent()) {
            if (p.value.empty())
                printLine(p.key, p.keyColor);
            else
                printKV(p.key, p.value, p.keyColor, p.valColor);
        }

        fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
            "            |{}|\n", std::string(inner, '_'));
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
        fmt::print(fg(LOGO_COLOR), "{}\n", separator('*'));
        fmt::print("\n");
        fmt::print(fg(LOGO_COLOR) | fmt::emphasis::bold, "{}\n", center(title));
        fmt::print(fg(LOGO_COLOR), "{}\n", center(std::string(title.size() + 4, '-')));
        fmt::print("\n");
        fmt::print(fg(LOGO_COLOR), "{}\n", separator('*'));
        fmt::print("\n");
    }

}

#endif
