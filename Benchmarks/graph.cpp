#include <ImplicitHeap/implicit_heap.hpp>
#include <ExplicitHeap/explicit_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <ViolationHeap/violation_heap.hpp>
#include <RankPairingHeap/rp_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>
#include <RankPairingHeap/rp_heap_t2.hpp>

#include <cctype>

#include "graph.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

high_resolution_clock timer;

using namespace MC;

void LogAndRun(const std::string& text, std::function< void()> func) {
    std::cout << text << ": ";
    std::cout.flush();
    auto s = timer.now();
    func();
    std::cout << duration_cast< milliseconds >(timer.now() - s).count() / 1000.0 << "s" << std::endl;
}

template < typename Ret>
Ret LogAndRun(const std::string& text, std::function< Ret()> func) {
    std::cout << text << ": ";
    std::cout.flush();
    auto s = timer.now();
    auto ret = func();
    std::cout << duration_cast< milliseconds >(timer.now() - s).count() / 1000.0 << "s" << std::endl;
    return ret;
}

std::map< std::string, double > averages;

template < template < typename > typename T >
std::vector< int > RunImpl(const map_t& map, int iterations, int dij) {
    std::cout << "---------------------------------------" << std::endl;
    Graph<T> g;
    auto n = g.HeapName();
    std::cout << n << ": " << std::endl;
    LogAndRun("populating graph", [&](){ g.Load(map); });
    std::vector< int > runs;
    LogAndRun("Total", [&]() {
        for (int i = 0; i < iterations; ++i)
        {
            bool succ = false;
            while (!succ) {
                try {
                    auto& from = g.FirstVertex();
                    auto& to = g.LastVertex();
                    g.ResetDistances();
                    auto s = timer.now();
                    if (dij == 1)
                        g.Dijkstra(from, to);
                    else
                        g.Dijkstra2(from, to);
                    runs.push_back(duration_cast< milliseconds >(timer.now() - s).count());
                    succ = true;
                } catch (std::logic_error) { }
            }
        }
    });

    auto t = std::accumulate(runs.begin(), runs.end(), 0ul) / static_cast< double >(runs.size());
    std::cout << "Average: " << t << "ms" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::transform(n.begin(), n.end(), n.begin(), [](char c) { return std::tolower(c); });
    averages[n] = t;
    return runs;
}

auto GetMap(const std::string& filename) {
//    map_t g = LogAndRun("loading '" + filename + "'",
//                       [&filename](){
//                           return Graph< ImplicitHeap >::LoadFile(filename);
//                       });
    std::cout << "loading '" << filename << "': ";
    std::flush(std::cout);
    auto s = timer.now();
    auto g =  Graph< ImplicitHeap >::LoadFile(filename);
    std::cout << duration_cast< milliseconds >(timer.now() - s).count() / 1000.0 << "s" << std::endl;
    return g;
}

void Run(const std::string& file, int iterations, int dij) {
    auto map = GetMap(file);
    RunImpl<ImplicitHeap>(map, iterations, dij);
    RunImpl<ExplicitHeap>(map, iterations, dij);
    RunImpl<FibonacciHeap>(map, iterations, dij);
    RunImpl<BinomialHeap>(map, iterations, dij);
    RunImpl<ViolationHeap>(map, iterations, dij);
    RunImpl<RankPairingHeap>(map, iterations, dij);
    RunImpl<RankPairingHeap2>(map, iterations, dij);
}


void NormalizeResults() {
    std::cout << "Normalized results: " << std::endl;
    auto min = std::min_element(
            averages.begin(),
            averages.end(),
            [](auto it1, auto it2) {
                return it1.second < it2.second;
            })->second;
    for (auto it : averages) {
        std::cout << it.first << ": " << it.second / min << std::endl;
    }
}

const char* GetCmdOption(const char** begin, const char** end, const std::string& option) {
    const char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return nullptr;
}

bool CmdOptionExists(const char** begin, const char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, const char** argv) {
    if (argc != 6) {
        std::cerr << "Invalid number of arguments. Example: \"./graph -d2 -f ../maps/maze512-2-0.map -i 1\"" << std::endl;
        return 1;
    }

    int dij;
    std::string file;
    if (!CmdOptionExists(argv, argv + argc, "-d1") && !CmdOptionExists(argv, argv + argc, "-d2")) {
        std::cerr << "-d1 for Dijkstra1 or -d2 for Dijkstra2 must be specified";
        return 1;
    }

    if (CmdOptionExists(argv, argv + argc, "-d1")) {
        dij = 1;
    } else {
        dij = 2;
    }

    if (CmdOptionExists(argv, argv + argc, "-f")) {
        file = GetCmdOption(argv, argv + argc, "-f");
    } else {
        std::cerr << "file must be specified with -f option";
        return 1;
    }

    int iterations = 0;

    if (CmdOptionExists(argv, argv + argc, "-i")) {
        iterations = std::stoi(GetCmdOption(argv, argv + argc, "-i"));
    } else {
        std::cerr << "iteration count must be specified with -i option";
        return 1;
    }


    Run(file, iterations, dij);

    NormalizeResults();
    return 0;
}
