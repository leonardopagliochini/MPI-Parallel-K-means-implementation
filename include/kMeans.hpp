#ifndef KMEANS_HPP
#define KMEANS_HPP

#include <random>
#include <iostream>
#include <mpi.h>
#include <thread>
#include "gnuplot-iostream.h"
#include "point.hpp"



class KMeans
{
public:
    KMeans(const int& k, const std::vector<Point> points);

    void run(const int& rank, const int& world_size);
    void printClusters() const;
    void plotClusters();
    int getNumberOfIterationForConvergence();
private:
    int numberOfIterationForConvergence;
    int k;
    std::vector<Point> points;
    std::vector<Point> centroids;
    Gnuplot gp;
};


#endif // KMEANS_HPP