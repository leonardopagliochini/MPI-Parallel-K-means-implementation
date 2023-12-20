#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <random>
#include "gnuplot-iostream.h"
#include <thread>
#include <random>
#include <mpi.h>

#include "point.hpp"
#include "kMeans.hpp"
#include "csvReader.hpp"


int main()
{
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //std::cout << "Rank: " << rank << std::endl;

    CSVReader reader("./Files/profiling.csv");
    std::vector<Point> points = reader.readData();
    int k;
    char printcluster = 'n';
    if (rank == 0)
    {
        std::cout << "Enter the number of clusters --> ";
        std::cin >> k;
        while(k<1 || k > points.size())
        {
            std::cout << "Please enter a number between " << 1 << " and " << points.size() << " --> ";
            std::cin >> k;
        }
        if (k > 8)
        {
            printcluster = 'y';
        }
        else
        {
            std::cout << "Do you want to print the clusters on the terminal? (y/n) --> ";
        std::cin >> printcluster;
        while (printcluster != 'y' && printcluster != 'n')
        {
            std::cout << "Please insert y or n --> ";
            std::cin >> printcluster;
        }
        }
        
    }

    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
    KMeans kmeans(k, points);
    kmeans.run(rank, world_size);
    if (rank == 0)
    {
        if (printcluster == 'y')
            kmeans.printClusters();
        if (k <= 8)
            kmeans.plotClusters();
        std::cout << "Number of iterations for convergence: " << kmeans.getNumberOfIterationForConvergence() << std::endl;

    }
    MPI_Finalize();
}