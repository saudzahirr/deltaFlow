#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct CSVData {
    std::vector<int> ID;
    std::vector<std::string> Name;
    std::vector<std::string> Type;
    std::vector<float> V;
    std::vector<float> delta;
    std::vector<float> Pg;
    std::vector<float> Qg;
    std::vector<float> Pl;
    std::vector<float> Ql;
    std::vector<float> Qgmax;
    std::vector<float> Qgmin;
    std::vector<float> Gs;
    std::vector<float> Bs;
};

// Function to read CSV and store data in a struct with vectors
CSVData readCSV(const std::string& filename) {
    CSVData data;  // Instantiate the struct to hold the columns as vectors
    std::ifstream file(filename);
    std::string line;

    // Read the header (first line) to understand the column names
    std::getline(file, line);
    std::stringstream headerStream(line);
    std::string column;

    // Parse the headers (column names) into a vector of column names
    std::vector<std::string> headers;
    while (std::getline(headerStream, column, ',')) {
        headers.push_back(column);
    }

    // Read the rest of the file line by line and parse the values
    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::string value;

        size_t columnIndex = 0;

        // Parse each column's value in the current row
        while (std::getline(lineStream, value, ',')) {
            if (columnIndex < headers.size()) {
                // Assign values to the appropriate vector based on the column name
                if (headers[columnIndex] == "ID") {
                    data.ID.push_back(std::stoi(value));  // Convert to integer
                } else if (headers[columnIndex] == "Name") {
                    data.Name.push_back(value);
                } else if (headers[columnIndex] == "Type") {
                    data.Type.push_back(value);
                } else if (headers[columnIndex] == "V") {
                    data.V.push_back(std::stof(value));  // Convert to float
                } else if (headers[columnIndex] == "delta") {
                    data.delta.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Pg") {
                    data.Pg.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Qg") {
                    data.Qg.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Pl") {
                    data.Pl.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Ql") {
                    data.Ql.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Qgmax") {
                    data.Qgmax.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Qgmin") {
                    data.Qgmin.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Gs") {
                    data.Gs.push_back(std::stof(value));
                } else if (headers[columnIndex] == "Bs") {
                    data.Bs.push_back(std::stof(value));
                }
            }
            columnIndex++;
        }
    }

    return data;
}
