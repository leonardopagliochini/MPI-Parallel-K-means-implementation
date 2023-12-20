#include "../include/kMeans.hpp"

    KMeans::KMeans(const int& k, const std::vector<Point> points) : k(k), points(points), gp("gnuplot -persist")
    {
        int size = points.size();
        std::random_device rd;                            // Initialize a random device
        std::mt19937 gen(23456789);                       // Initialize a Mersenne Twister random number generator with the random device
        std::uniform_int_distribution<> dis(0, size - 1); // Create a uniform distribution between 0 and size - 1

        for (int i = 0; i < k; ++i)
        {
            centroids.emplace_back(points[dis(gen)]); // Generate a random index and use it to select a point
        }
    }

    void KMeans::run(const int& rank, const int& world_size)
    {
        bool change = true;
        int iter = 0;
        int iter_max = 1000;
        std::vector<int> counts(k, 0);
        while (change && iter < iter_max)
        {
            if (rank == 0)
            change = false;
            // --------------------------------------------
            for (Point &p : points)
            {
                int points_per_cluster = points.size() / world_size;
                int start = rank * points_per_cluster;
                int end = (rank == world_size - 1) ? points.size() : (rank + 1) * points_per_cluster;
                if (p.id >= start && p.id < end)
                {
                    double minDist = std::numeric_limits<double>::max();
                    int nearest = 0;
                    for (int j = 0; j < k; ++j)
                    {
                        double tempDist = p.distance(centroids[j]);
                        if (tempDist < minDist)
                        {
                            minDist = tempDist;
                            nearest = j;
                        }
                    }
                    if (p.clusterId != nearest)
                    {
                        p.clusterId = nearest;
                        // p.minDist = minDist;
                        change = true;
                    }
                    if (rank != 0)
                    {
                        MPI_Send(&p.id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                        // MPI_Send(&p.minDist, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
                        MPI_Send(&p.clusterId, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);

                        //std::cout << "---------------inviati dati del punto " << p.id << std::endl;
                    }
                }
                if (rank == 0)
                {
                    int whosend = std::min(p.id / points_per_cluster, world_size - 1);
                    if (whosend != 0)
                    {
                        //std::cout << "who send: " << whosend << std::endl;
                        //std::cout << "p.id: " << p.id << std::endl;
                        int id;
                        // int minDist;
                        int newCluster;
                        MPI_Recv(&id, 1, MPI_INT, whosend, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        // MPI_Recv(&minDist, 1, MPI_INT, whosend, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        MPI_Recv(&newCluster, 1, MPI_INT, whosend, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        //std::cout << "---------------ricevuti dati da " << whosend << std::endl;

                        if (p.clusterId != newCluster)
                        {
                            p.clusterId = newCluster;
                            // p.minDist = minDist;
                            change = true;
                        }
                    }
                }
            }

            if (rank == 0)
            {
                //std::cout << " bonaaa " << std::endl;
                counts = std::vector<int>(k, 0);
                centroids = std::vector<Point>(k, Point(points[0].numberOfFeatures));
                for (Point p : points)
                {
                    for (int i = 0; i < p.numberOfFeatures; ++i)
                    {
                        centroids[p.clusterId].setFeature(i, centroids[p.clusterId].getFeature(i) + p.getFeature(i));
                    }
                    counts[p.clusterId] += 1;
                }

                for (int i = 0; i < k; ++i)
                {
                    for (int j = 0; j < centroids[i].numberOfFeatures; ++j)
                    {
                        centroids[i].setFeature(j, centroids[i].getFeature(j) / counts[i]);
                    }
                }
            }
            for (int i = 0; i < k; ++i)
            {
                for (int j = 0; j < centroids[i].numberOfFeatures; ++j)
                {
                    MPI_Bcast(&centroids[i].features[j], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                }
            }
            MPI_Bcast(&change, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
            iter++;
        }
        numberOfIterationForConvergence = iter;
    }
    void KMeans::printClusters()  const
    {
        for (int i = 0; i < k; ++i)
        {
            std::cout << "Cluster " << i + 1 << ":\n";
            for (Point p : points)
            {
                if (p.clusterId == i)
                {
                    std::cout << "Point " << p.id << ": (";
                    for (int j = 0; j < p.numberOfFeatures; ++j)
                    {
                        std::cout << p.getFeature(j);
                        if (j < p.numberOfFeatures - 1)
                            std::cout << ", ";
                    }
                    std::cout << ")"<< std::endl;
                }
            }
        }
    }

    void KMeans::plotClusters()
    {
        if(centroids[0].numberOfFeatures != 2)
        {
            std::cout << "Cannot plot clusters with more than 2 features" << std::endl;
            return;
        }
        gp << "set xrange [20:70]\nset yrange [0:30]\n";
        gp << "set key outside\n"; // Add this line to place the legend outside the plot
        std::vector<std::string> colors = {"red", "blue", "green", "brown", "purple", "orange", "cyan", "violet"};
        gp << "plot ";
        for (int i = 0; i < k; ++i)
        {
            std::vector<std::pair<double, double>> pts;
            for (Point p : points)
            {
                if (p.clusterId == i)
                {
                    pts.emplace_back(std::make_pair(p.getFeature(0), p.getFeature(1)));
                }
            }
            gp << "'-' with points pointtype 7 pointsize 1 lc rgb '" << colors[i % colors.size()] << "' title 'Cluster " << i + 1 << "'"; // Add a title to each plot command
            if (i < k - 1)
            {
                gp << ", ";
            }
        }
        gp << ", '-' with points pointtype 7 pointsize 2 lc rgb 'black' title 'Centroids'"; // Add a title to the centroids \ command
        gp << "\n";
        for (int i = 0; i < k; ++i)
        {
            std::vector<std::pair<double, double>> pts;
            for (Point p : points)
            {
                if (p.clusterId == i)
                {
                    pts.push_back(std::make_pair(p.getFeature(0), p.getFeature(1)));
                }
            }
            gp.send1d(pts);
        }
        std::vector<std::pair<double, double>> centroid_pts;
        for (Point c : centroids)
        {
            centroid_pts.push_back(std::make_pair(c.getFeature(0), c.getFeature(1)));
        }
        gp.send1d(centroid_pts);
    }

    int KMeans::getNumberOfIterationForConvergence()
    {
        return numberOfIterationForConvergence;
    }