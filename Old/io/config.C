#include "config.H"

Config::Config(std::string& simFilePath) {
    file.open(simFilePath);
    if (not file.is_open()) {
        ERROR("Failed to open log file: {}", simFilePath);
    }

    this->parseConfig();
}

Config::~Config() {
    if (file.is_open()) {
        file.close();
    }
}

Format Config::getFormat() const noexcept {
    return this->m_format;
}

std::string Config::getIncludeFile() const noexcept {
    return this->m_includeFile;
}

std::string Config::getBusDataCsv() const noexcept {
    return this->m_busDataCsv;
}

std::string Config::getBranchDataCsv() const noexcept {
    return this->m_branchDataCsv;
}

AnalysisType Config::getAnalysisType() const noexcept {
    return this->m_analysis;
}

Method Config::getSolverMethod() const noexcept {
    return this->m_solver;
}

double Config::getTolerance() const noexcept {
    return this->m_tolerance;
}

int Config::getMaxNumberOfIterations() const noexcept {
    return this->m_maxNumberOfIterations;
}

bool Config::writeAscii() const noexcept {
    return this->ascii;
}

bool Config::writeBinary() const noexcept {
    return this->bin;
}

void Config::parseConfig() {
    static const std::regex s_InputFormatPattern("^IN FORMAT");
    static const std::regex s_IncludeDataPattern("^INCLUDE");
    static const std::regex s_BusDataPattern("^BUS CSV");
    static const std::regex s_BranchDataPattern("^BRANCH CSV");
    static const std::regex s_AnalysisPattern("^ANALYSIS");
    static const std::regex s_OutputFormatPattern("^OUT FORMAT");
    static const std::regex s_SkipPattern(R"(^\$.*)");

    std::string line;
    ConfigCardMode mode = ConfigCardMode::SKIP;

    while (getline(file, line)) {
        if (std::regex_search(line, s_InputFormatPattern)) {
            mode = ConfigCardMode::IN_FORMAT;
            continue;
        }
        else if (std::regex_search(line, s_IncludeDataPattern)) {
            mode = ConfigCardMode::INCLUDE_CDF_PATH;
            continue;
        }
        else if (std::regex_search(line, s_BusDataPattern)) {
            mode = ConfigCardMode::BUS_CSV_PATH;
            continue;
        }
        else if (std::regex_search(line, s_BranchDataPattern)) {
            mode = ConfigCardMode::BRANCH_CSV_PATH;
            continue;
        }
        else if (std::regex_search(line, s_AnalysisPattern)) {
            mode = ConfigCardMode::ANALYSIS_SETTINGS;
            continue;
        }
        else if (std::regex_search(line, s_OutputFormatPattern)) {
            mode = ConfigCardMode::OUT_FORMAT;
            continue;
        }
        else if (std::regex_search(line, s_SkipPattern) || line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string token;

        switch (mode) {
            case ConfigCardMode::IN_FORMAT:
                iss >> token;
                if (token == "CDF") m_format = Format::CDF;
                else if (token == "CSV") m_format = Format::CSV;
                break;

            case ConfigCardMode::INCLUDE_CDF_PATH:
                m_includeFile = line;
                break;

            case ConfigCardMode::BUS_CSV_PATH:
                m_busDataCsv = line;
                break;

            case ConfigCardMode::BRANCH_CSV_PATH:
                m_branchDataCsv = line;
                break;

            case ConfigCardMode::ANALYSIS_SETTINGS: {
                std::string typeStr, methodStr, tolStr, iterStr;
                iss >> typeStr >> methodStr >> tolStr >> iterStr;

                if (typeStr == "STATIC") {
                    m_analysis = AnalysisType::STATIC;
                }
                else if (typeStr == "TRANSIENT") {
                    ERROR("TRANSIENT analysis not implemented.");
                    exit(EXIT_FAILURE);
                }
                else {
                    ERROR("Invalid analysis type: {}", typeStr);
                    break;
                }

                if (methodStr == "GAUSS") m_solver = Method::GAUSS;
                else if (methodStr == "NEWTON") m_solver = Method::NEWTON;
                else {
                    ERROR("Invalid method: {}", methodStr);
                    break;
                }

                try {
                    m_tolerance = std::stod(tolStr);
                }
                catch (...) {
                    ERROR("Invalid tolerance value: {}", tolStr);
                    break;
                }

                try {
                    m_maxNumberOfIterations = std::stoi(iterStr);
                }
                catch (...) {
                    ERROR("Invalid iteration count: {}", iterStr);
                    break;
                }

                break;
            }

            case ConfigCardMode::OUT_FORMAT:
                while (iss >> token) {
                    if (token == "ASCII") ascii = true;
                    else if (token == "BIN") bin = true;
                }
                break;

            default:
                break;
        }
    }
}
