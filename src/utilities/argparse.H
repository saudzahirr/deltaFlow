#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <iostream>
#include "enums.H"
#include "utils.H"
#include "exception.H"

#ifndef VERSION
#define VERSION "1.1.5"
#endif

class ArgumentParser final {
    public:
        /**
         * Constructor
         */
        ArgumentParser(int argc, char* argv[]);

        /**
         * Destructor
         */
        ~ArgumentParser() = default;

        /**
         * Copy Constructor
         */
        ArgumentParser(const ArgumentParser&) = delete;

        /**
         * Copy Assignment Operator
         */
        ArgumentParser& operator=(const ArgumentParser&) = delete;

        /**
         * Move Constructor
         */
        ArgumentParser(const ArgumentParser&&) = delete;

        /**
         * Move Assignment Operator
         */
        ArgumentParser& operator=(ArgumentParser&&) = delete;

        std::string getBusDataCsv() const noexcept;
        std::string getBranchDataCsv() const noexcept;
        Solver getSolverType() const noexcept;
        double getTolerance() const noexcept;
        unsigned long int getMaxIterations() const noexcept;
        bool writeAscii() const noexcept;
        bool guiEnabled() const noexcept;

    private:
        std::string busDataCsv;
        std::string branchDataCsv;
        Solver solver = Solver::GAUSS;
        double tolerance = 1E-8;
        unsigned long int maxIterations = 128;
        bool ascii = false;
        bool gui = false;

        void parse_args(int argc, char* argv[]);
        void help() const noexcept;
        void version() const noexcept;
};

#endif
