#include "../include/csvReader.hpp"

CSVReader::CSVReader(const std::string &filename) : filename(filename) {}

std::vector<Point> CSVReader::readData()
{
    std::vector<Point> points;

    // Open the CSV file
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening the file." << std::endl;
        return points;
    }

    // Read the file line by line
    std::string line;
    while (getline(file, line))
    {
        // Create a std::stringstream from the line
        std::stringstream ss(line);

        // Parse the CSV values
        std::vector<std::string> values;
        std::string value;
        while (getline(ss, value, ','))
        {
            values.push_back(value);
        }

        // Convert the values to appropriate types
        int id = stoi(values[0]);
        std::vector<double> features;
        for (size_t i = 1; i < values.size(); i++)
        {
            features.push_back(stod(values[i]));
        }

        // Create a point and add it to the std::vector
        points.emplace_back(Point(id - 1, features));
    }

    return points;
}
