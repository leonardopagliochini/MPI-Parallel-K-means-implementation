#include "../include/point.hpp"

Point::Point(int features_size)
{
    this->id = 0;
    this->clusterId = -1;
    this->features = std::vector<double>(features_size, 0.0);
    this->numberOfFeatures = features_size;
}
//Point::Point(const int& id, const double& x, const double& y) : id(id), x(x), y(y), clusterId(-1) /*, minDist(std::numeric_limits<double>::max())*/ {}
Point::Point(const int& id, const std::vector<double>& coordinates)
{
    this->id = id;
    this->features = coordinates;
    this->clusterId = -1;
    this->numberOfFeatures = coordinates.size();
}

double Point::distance(const Point& p) const
{
    double sum = 0.0;
    for (int i = 0; i < features.size(); ++i)
    {
        sum += (features[i] - p.features[i]) * (features[i] - p.features[i]);
    }
    return sqrt(sum);
}

double& Point::getFeature(int index) 
{
    return features[index];
}

void Point::setFeature(int index, double value)
{
    this->features[index] = value;
}