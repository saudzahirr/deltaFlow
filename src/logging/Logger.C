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

#include "Display.H"
#include "Logger.H"

Logger& Logger::getLogger() {
    static Logger instance("deltaFlow.log", Level::DEBUG);
    return instance;
}

Logger::Logger(const std::string& name, Level level) : m_FilePath(name), m_Level(level) {
    file.open(m_FilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open log file: " << m_FilePath << std::endl;
    }

    // Write banner to log file
    if (file.is_open()) {
        file << Display::fileBanner();
        auto now = std::time(nullptr);
        auto ts = fmt::format("{:%d-%b-%Y %H:%M:%S}", fmt::localtime(now));
        file << fmt::format("\n   Log started: {}\n", ts);
        file << "   " << Display::separator('-') << "\n\n";
        file.flush();
    }
}

Logger::~Logger() {
    if (file.is_open()) {
        auto now = std::time(nullptr);
        auto ts = fmt::format("{:%d-%b-%Y %H:%M:%S}", fmt::localtime(now));
        file << "\n   " << Display::separator('-') << "\n";
        file << fmt::format("   Log ended: {}\n", ts);
        file.close();
    }
}

void Logger::log(const std::string& msg, const Level& level) {
    std::string levelStr;
    m_Level = level;
    fmt::color levelColor = fmt::color::white;

    switch (m_Level) {
        case Level::DEBUG:
            levelStr = "DEBUG"; levelColor = fmt::color::light_blue; break;
        case Level::INFO:
            levelStr = "INFO"; levelColor = fmt::color::green; break;
        case Level::WARN:
            levelStr = "WARN"; levelColor = fmt::color::yellow; break;
        case Level::ERROR:
            levelStr = "ERROR"; levelColor = fmt::color::orange_red; break;
        case Level::CRITICAL:
            levelStr = "CRITICAL"; levelColor = fmt::color::red; break;
        default:
            levelStr = "LOG"; break;
    }

    auto now = std::time(nullptr);
    auto timestamp = fmt::format("{:%d-%m-%Y %H:%M:%S}", fmt::localtime(now));

    // Plain text to log file
    std::string logMessage = fmt::format(
        "{} :: {:<8} :: {}\n", timestamp, levelStr, msg
    );

    if (file.is_open()) {
        file << logMessage;
        file.flush();
    }

    // Colored output to terminal
    logMessage = fmt::format(
        "{} :: {} :: {}\n",
        timestamp,
        fmt::format(fg(levelColor) | fmt::emphasis::bold, "{:<8}", levelStr),
        msg
    );

    fmt::print("{}", logMessage);
}
