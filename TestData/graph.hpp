//
// Created by marzi on 2.3.19.
//

#ifndef PRIORITY_QUEUES_GRAPH_HPP
#define PRIORITY_QUEUES_GRAPH_HPP

#include <vector>
#include <sstream>
#include <set>
#include <map>

struct edge {
    int from;
    int to;
    int distance;
};

struct vertex {
    int id;
    unsigned distance = -1;
    int prevId = -1;

    vertex(int id) : id(id) {}

    std::map< int, int > neighbors;
//    std::vector< int > neighborIds;
//    std::vector< edge > edges;
};

;


class graph {

public:
    std::map< int, vertex > vertices;
    std::vector< edge > edges;

    void parseGraph(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;


        while (std::getline(file, line)) {
            std::stringstream ss(line);

            std::string from, to, dist;

            std::getline(ss, from, ':');
            std::getline(ss, to, ':');
            std::getline(ss, dist, ':');

            int f = std::stoi(from), t = std::stoi(to), d = std::stoi(dist);

            vertices[f].neighbors[t] = d;
            //vertices[t].neighbors[f] = d;
        }
    }
};


#endif //PRIORITY_QUEUES_GRAPH_HPP
