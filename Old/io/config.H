#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <regex>
#include "logger.H"

enum class ConfigCardMode {
    IN_FORMAT,
    INCLUDE_CDF_PATH,
    BUS_CSV_PATH,
    BRANCH_CSV_PATH,
    ANALYSIS_SETTINGS,
    OUT_FORMAT,
    SKIP
};

enum class AnalysisType {
    STATIC,
    TRANSIENT,
};

enum class Format {
    CDF,
    CSV,
};

enum class Method {
    GAUSS,
    NEWTON,
};

class Config final {
    public:
        Config(std::string&);
        ~Config();

        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        Format getFormat() const noexcept;
        std::string getIncludeFile() const noexcept;
        std::string getBusDataCsv() const noexcept;
        std::string getBranchDataCsv() const noexcept;
        AnalysisType getAnalysisType() const noexcept;
        Method getSolverMethod() const noexcept;
        double getTolerance() const noexcept;
        int getMaxNumberOfIterations() const noexcept;
        bool writeAscii() const noexcept;
        bool writeBinary() const noexcept;

    private:
        std::ifstream file;
        Format m_format;
        std::string m_includeFile;
        std::string m_busDataCsv;
        std::string m_branchDataCsv;
        AnalysisType m_analysis;
        Method m_solver;
        double m_tolerance;
        int m_maxNumberOfIterations;

        bool ascii = false;
        bool bin = false;

        void parseConfig();
};

#endif
