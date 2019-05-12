#pragma once

#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>
#include <chrono>
#include <map>
#include <random>
#include <unordered_map>

namespace MC {

using map_t = std::vector< std::vector< char > >;


template < template < typename > typename Heap >
class Graph {
public:

    struct Indices {
        int row = 0;
        int col = 0;

        bool operator==(Indices o) const { return row == o.row && col == o.col; }
    };

    struct Vertex;
    using HeapType = Heap< Vertex* >;
    using Handle = const typename HeapType::NodeType*;

    struct Vertex {
        std::vector< Vertex* > neighbors;
        Indices indices;

        Vertex(Indices i) : indices(i) {}

        Handle handle;

        int dist = HeapType::Infinity;

        Vertex* prev = nullptr;

        std::string ToString() const {
            return "[" + std::to_string(indices.row) + "," + std::to_string(indices.col) + "]";
        }
    };

private:


    struct MapParams {
        int width;
        int height;
    };


    static void SetParams(const std::string& line, MapParams& p ) {
        std::stringstream ss(line);
        std::string what, value;
        std::getline(ss, what, ' ');
        std::getline(ss, value);

        int v = std::stoi(value);

        if (what[0] == 'w')
            p.width = v;
        else
            p.height = v;
    }

    static map_t GetMetadata(std::ifstream& f) {
        std::string line;
        MapParams p;
        std::getline(f, line); // map type
        std::getline(f, line); // param
        SetParams(line , p);
        std::getline(f, line); // 2nd param
        SetParams(line, p);

        std::getline(f, line); // last

        std::vector< char > inner(p.height);

        return map_t(p.width, inner);
    }

    static auto Left(Indices indices) { --indices.col; return indices; }
    static auto Up(Indices indices) { --indices.row; return indices; }
    static auto Right(Indices indices) { ++indices.col; return indices; }
    static auto Down(Indices indices) { ++indices.row; return indices; }

    static auto UpLeft(Indices indices) { return Up(Left(indices)); }
    static auto UpRight(Indices indices) { return Up(Right(indices)); }
    static auto DownRight(Indices indices) { return Down(Right(indices)); }
    static auto DownLeft(Indices indices) { return Down(Left(indices)); }

    void SetNeighbours() {
        auto find = [this](Indices i) -> Vertex* {
            if (i.row >= vertices.size())
                return nullptr;
            auto& v = vertices[i.row];
            auto it = std::find_if(v.begin(), v.end(), [i](const auto& x) { return x.indices == i; });
            if (it != v.end())
                return &*it;
            return nullptr;
        };

        auto setNeighbour = [&find](auto& v, auto f) {
            auto i = f(v.indices);
            if (auto x = find(i); x != nullptr)
                v.neighbors.push_back(x);
        };

        for (auto& vector :vertices) {
            for (auto& v : vector) {
                setNeighbour(v, Left);
                setNeighbour(v, UpLeft);
                setNeighbour(v, Up);
                setNeighbour(v, UpRight);
                setNeighbour(v, Right);
                setNeighbour(v, DownRight);
                setNeighbour(v, Down);
                setNeighbour(v, DownLeft);
            }
        }
    }

public:

    std::string HeapName() const { return HeapType().Name; }

    Graph() = default;

    Graph(const std::string& filename) {
        Load(LoadFile(filename));
    }

    Graph(const map_t& map) {
        Load(map);
    }

    static map_t LoadFile(const std::string& filename) {
        std::ifstream map(filename);
        if (!map.is_open())
            throw std::logic_error("file '" + filename + "' not found");
        std::string line;

        int row = 0;
        auto v = GetMetadata(map);

        while (std::getline(map, line)) {
            int col = 0;
            for (char c : line) {
                v[row][col] = c;
                ++col;
            }
            ++row;
        }

        return v;
    }

    const Vertex& RandomVertex() const {
        std::random_device r;
        std::mt19937_64 rng(r());
        std::uniform_int_distribution< int > uid;

        if (vertices.size() == 0)
            throw std::logic_error("empty");

        int row = uid(rng) % vertices.size();

        if (vertices[row].size() == 0)
            throw std::logic_error("empty");

        int col = uid(rng) % vertices[row].size();

        return vertices[row][col];
    }


    void Load(const map_t& map) {
        int row = 0;
        vertices = decltype(vertices)(map.size());
        for (auto& v :  map) {
            int col = 0;
            for (char c : v) {
                if (c == '.') {
                    vertices[row].push_back(Vertex({ row, col }));
                }
                ++col;
            }
            ++row;
        }

        SetNeighbours();
    }

    std::vector< std::vector< Vertex >> vertices;

    void Dijkstra(const Vertex& from, const Vertex& to) {
        auto f = from.indices;
        auto t = to.indices;
        HeapType h;

        for (auto& vector : vertices) {
            for (auto& v : vector) {
                if (v.indices == f)
                    v.dist = 0;
                auto node = h.Insert(v.dist, &v);
                node->item->handle = node;
            }
        }

        while (!h.Empty()) {
            auto min = h.ExtractMin();
            auto u = min->item;

            if (u->indices == t) {
                break;
            }

            for (auto v : u->neighbors) {
                int alt = u->dist+ 1;
                if (alt < v->dist) {
                    v->dist = alt;
                    v->prev = u;
                    h.DecreaseKey(v->handle, alt);
                }
            }
        }
    }

    void Dijkstra2(const Vertex& from, const Vertex& to) {
        auto f = from.indices;
        auto t = to.indices;
        HeapType h;

        const_cast< Vertex& >(from).dist = 0;

        std::unordered_map< Handle, Handle > handles;

        h.Insert(0, const_cast< Vertex* >(&from));

        while (!h.Empty()) {
            auto min = h.ExtractMin();
            auto u = min->item;

            if (u->indices == t)
                break;

            for (auto v : u->neighbors) {
                int alt = u->dist + 1;
                if (alt < v->dist) {
                    v->dist = alt;
                    v->prev = u;
                    if (auto it = handles.find(v->handle); it != handles.end()) {
                        h.DecreaseKey(v->handle, alt);
                    } else {
                        auto handle = h.Insert(alt, v);
                        handles[handle] = handle;
                    }
                }
            }
        }
    }

};

}