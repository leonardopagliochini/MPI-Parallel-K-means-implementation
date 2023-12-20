#ifndef POINT_HPP
#define POINT_HPP

#include <cmath>
#include <vector>

class Point
{

public:
    int id;
    std::vector<double> features;
    int numberOfFeatures;
    int clusterId;
    // double minDist;

    Point(int features_size);
    Point(const int& id, const std::vector<double>& coordinates);
    double distance(const Point& p) const;

    double& getFeature(int index) ;
    void setFeature(int index, double x);
};


#endif // POINT_HPP