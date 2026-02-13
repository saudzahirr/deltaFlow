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
 * @brief Banner and branding utilities for deltaFlow.
 *
 * Provides the deltaFlow logo and formatted header/footer for
 * terminal (colored) and file (plain text) output.
 */

#ifndef BANNER_H
#define BANNER_H

#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #define NOMINMAX
  #include <windows.h>
  #undef ERROR
#else
  #include <unistd.h>
#endif

#include "Version.H"

namespace Banner {

constexpr fmt::rgb BRAND_COLOR{153, 0, 204};

/**
 * @brief Returns the width of the output page (characters).
 */
inline int pageWidth() { return 80; }

/**
 * @brief Returns a separator line of characters.
 */
inline std::string separator(char ch = '=') {
    return std::string(pageWidth(), ch);
}

/**
 * @brief Returns a centered string within the page width.
 */
inline std::string center(const std::string& text) {
    int pad = (pageWidth() - static_cast<int>(text.size())) / 2;
    if (pad <= 0) return text;
    return std::string(pad, ' ') + text;
}

/**
 * @brief Returns the starred license+logo box (single box with logo inside).
 *
 * The box is 67 columns of '*' with a leading space:
 *   " " + 67*'*' = 68-char lines.
 * Inner content width is 65 characters.
 */
inline std::string licenseNotice() {
    constexpr int W = 67; // number of '*' in the rule line
    constexpr int inner = W - 2; // 65 chars of content between the two border '*'

    auto star = [&](const std::string& text) -> std::string {
        int pad = inner - static_cast<int>(text.size());
        if (pad < 0) pad = 0;
        return " *" + text + std::string(pad, ' ') + "*\n";
    };
    auto rule = [&]() -> std::string {
        return " " + std::string(W, '*') + "\n";
    };
    auto centerIn = [&](const std::string& text) -> std::string {
        int left = (inner - static_cast<int>(text.size())) / 2;
        if (left < 0) left = 0;
        return std::string(left, ' ') + text;
    };

    std::string blk;
    blk += rule();
    blk += star("");
    blk += star(centerIn(R"(/\\)"));
    blk += star(centerIn(R"(/  \\)"));
    blk += star(centerIn(R"(/    \\)"));
    blk += star(centerIn(R"(/      \\)"));
    blk += star(centerIn("========="));
    blk += star(centerIn("deltaFlow  v" + std::string(deltaFlow_VERSION)));
    blk += star("");
    blk += star("  Copyright (c) 2024 Saud Zahir.");
    blk += star("  All rights reserved.");
    blk += star("");
    blk += star("  deltaFlow is free software: you can redistribute it");
    blk += star("  and/or modify it under the terms of the GNU General");
    blk += star("  Public License as published by the Free Software");
    blk += star("  Foundation, either version 3 of the License, or");
    blk += star("  (at your option) any later version.");
    blk += star("");
    blk += star("  deltaFlow is distributed in the hope that it will be useful,");
    blk += star("  but WITHOUT ANY WARRANTY; without even the implied warranty");
    blk += star("  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    blk += star("  See the GNU General Public License for more details.");
    blk += star("");
    blk += star("  <https://www.gnu.org/licenses/>");
    blk += star("");
    blk += rule();

    return blk;
}

/**
 * @brief Returns the boxed product info block (LS-DYNA style).
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

    std::string box;
    box += "             " + std::string(W - 2, '_') + "\n";
    box += line("");
    box += line("  deltaFlow -- Power Flow Analysis Software");
    box += line("  A Program for Steady-State Power System Analysis");
    box += line(fmt::format("  Version : {}", deltaFlow_VERSION));
    box += line(fmt::format("  CMake   : {}", CMake_VERSION));
    box += line(fmt::format("  GCC     : {}", gcc_VERSION));
    box += line("");
    box += line("  Copyright (c) 2024 Saud Zahir");
    box += line("  Licensed under GNU GPL v3.0");
    box += line("");
    std::string host = "unknown";
#ifdef _WIN32
    char hbuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD hbufLen = sizeof(hbuf);
    if (GetComputerNameA(hbuf, &hbufLen)) host = std::string(hbuf);
#else
    char hbuf[256];
    if (gethostname(hbuf, sizeof(hbuf)) == 0) host = std::string(hbuf);
#endif
    box += line(fmt::format("  Hostname  : {}", host));
    box += rule();

    return box;
}

/**
 * @brief Prints the colored product info box to terminal.
 */
inline void printProductBox() {
    constexpr int W = 59;
    std::string host = "unknown";
#ifdef _WIN32
    char hbuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD hbufLen = sizeof(hbuf);
    if (GetComputerNameA(hbuf, &hbufLen)) host = std::string(hbuf);
#else
    char hbuf[256];
    if (gethostname(hbuf, sizeof(hbuf)) == 0) host = std::string(hbuf);
#endif

    auto border = [&](const std::string& text) -> std::string {
        int pad = W - 2 - static_cast<int>(text.size());
        if (pad < 0) pad = 0;
        return text + std::string(pad, ' ');
    };

    // Top rule
    fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
        "             {}\n", std::string(W - 2, '_'));

    // Helper: print a line with colored border and colored content
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

    printLine("", BRAND_COLOR);
    printLine("  deltaFlow -- Power Flow Analysis Software", BRAND_COLOR);
    printLine("  A Program for Steady-State Power System Analysis", fmt::rgb{180, 180, 180});
    printKV("  Version : ", deltaFlow_VERSION, fmt::rgb{100, 200, 255}, fmt::rgb{255, 255, 100});
    printKV("  CMake   : ", CMake_VERSION,     fmt::rgb{100, 200, 255}, fmt::rgb{255, 255, 100});
    printKV("  GCC     : ", gcc_VERSION,       fmt::rgb{100, 200, 255}, fmt::rgb{255, 255, 100});
    printLine("", BRAND_COLOR);
    printKV("  Copyright (c) 2024 ", "Saud Zahir", fmt::rgb{180, 180, 180}, fmt::rgb{255, 165, 0});
    printLine("  Licensed under GNU GPL v3.0", fmt::rgb{100, 255, 100});
    printLine("", BRAND_COLOR);
    printKV("  Hostname  : ", host, fmt::rgb{100, 200, 255}, fmt::rgb{255, 200, 100});

    // Bottom rule
    fmt::print(fg(fmt::color::white) | fmt::emphasis::bold,
        "            |{}|\n", std::string(W - 2, '_'));
}

/**
 * @brief Prints the colored license notice box to terminal.
 *
 */
inline void printLicenseNotice() {
    constexpr int W = 67;
    constexpr int inner = W - 2;
    constexpr fmt::rgb TEXT_COLOR{0x5a, 0x71, 0x79};
    constexpr fmt::rgb BORDER_COLOR{0x5a, 0x71, 0x79};

    auto centerIn = [&](const std::string& text) -> std::string {
        int left = (inner - static_cast<int>(text.size())) / 2;
        if (left < 0) left = 0;
        return std::string(left, ' ') + text;
    };

    // Print a star-bordered line with border in BORDER_COLOR and content in given color
    auto starLine = [&](const std::string& text, fmt::rgb color) {
        int pad = inner - static_cast<int>(text.size());
        if (pad < 0) pad = 0;
        fmt::print(fg(BORDER_COLOR), " *");
        fmt::print(fg(color) | fmt::emphasis::bold, "{}", text);
        fmt::print(fg(BORDER_COLOR), "{}", std::string(pad, ' '));
        fmt::print(fg(BORDER_COLOR), "*\n");
    };

    // Rule line
    auto rule = [&]() {
        fmt::print(fg(BORDER_COLOR), " {}\n", std::string(W, '*'));
    };

    rule();
    starLine("", TEXT_COLOR);
    starLine(centerIn(R"(/\\)"),      BRAND_COLOR);
    starLine(centerIn(R"(/  \\)"),    BRAND_COLOR);
    starLine(centerIn(R"(/    \\)"),  BRAND_COLOR);
    starLine(centerIn(R"(/      \\)"), BRAND_COLOR);
    starLine(centerIn("========="),   BRAND_COLOR);
    starLine(centerIn("deltaFlow  v" + std::string(deltaFlow_VERSION)), BRAND_COLOR);
    starLine("", TEXT_COLOR);
    starLine("  Copyright (c) 2024 Saud Zahir.", TEXT_COLOR);
    starLine("  All rights reserved.", TEXT_COLOR);
    starLine("", TEXT_COLOR);
    starLine("  deltaFlow is free software: you can redistribute it", TEXT_COLOR);
    starLine("  and/or modify it under the terms of the GNU General", TEXT_COLOR);
    starLine("  Public License as published by the Free Software", TEXT_COLOR);
    starLine("  Foundation, either version 3 of the License, or", TEXT_COLOR);
    starLine("  (at your option) any later version.", TEXT_COLOR);
    starLine("", TEXT_COLOR);
    starLine("  deltaFlow is distributed in the hope that it will be useful,", TEXT_COLOR);
    starLine("  but WITHOUT ANY WARRANTY; without even the implied warranty", TEXT_COLOR);
    starLine("  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.", TEXT_COLOR);
    starLine("  See the GNU General Public License for more details.", TEXT_COLOR);
    starLine("", TEXT_COLOR);
    starLine("  <https://www.gnu.org/licenses/>", TEXT_COLOR);
    starLine("", TEXT_COLOR);
    rule();
}

/**
 * @brief Prints the colored banner to terminal.
 */
inline void printTerminalBanner() {
    fmt::print("\n");
    printLicenseNotice();
    fmt::print("\n");
    printProductBox();
    fmt::print("\n");
}

/**
 * @brief Returns a full plain-text banner for output/log files.
 */
inline std::string fileBanner() {
    std::string banner;
    banner += licenseNotice();
    banner += "\n";
    banner += productBox();
    return banner;
}

/**
 * @brief Returns a section header for output files.
 */
inline std::string sectionHeader(const std::string& title) {
    std::string header;
    header += "\n";
    header += separator('*') + "\n";
    header += "\n";
    header += center(title) + "\n";
    header += center(std::string(title.size() + 4, '-')) + "\n";
    header += "\n";
    header += separator('*') + "\n";
    return header;
}

/**
 * @brief Returns a sub-section header for output files.
 */
inline std::string subSectionHeader(const std::string& title) {
    std::string header;
    header += "\n";
    header += "   " + std::string(title.size() + 4, '-') + "\n";
    header += "   " + title + "\n";
    header += "   " + std::string(title.size() + 4, '-') + "\n";
    return header;
}

/**
 * @brief Prints a colored section header to terminal.
 */
inline void printSectionHeader(const std::string& title) {
    fmt::print("\n");
    fmt::print(fg(BRAND_COLOR), "{}\n", separator('*'));
    fmt::print("\n");
    fmt::print(fg(BRAND_COLOR) | fmt::emphasis::bold, "{}\n", center(title));
    fmt::print(fg(BRAND_COLOR), "{}\n",
        center(std::string(title.size() + 4, '-')));
    fmt::print("\n");
    fmt::print(fg(BRAND_COLOR), "{}\n", separator('*'));
    fmt::print("\n");
}

}

#endif
