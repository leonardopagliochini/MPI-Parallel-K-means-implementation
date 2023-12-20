#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "point.hpp"
class CSVReader
{
public:
    CSVReader(const std::string &filename);
    std::vector<Point> readData();

private:
    std::string filename;
};

#endif // CSVREADER_HPP