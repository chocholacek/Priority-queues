#include <ImplicitHeap/implicit_heap.hpp>
#include <ExplicitHeap/explicit_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <ViolationHeap/violation_heap.hpp>
#include <RankPairingHeap/rp_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>

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

template < template < typename > typename T >
std::vector< int > RunImpl(const map_t& map, int iterations) {
    std::cout << "---------------------------------------" << std::endl;
    Graph<T> g;
    std::cout << g.HeapName() << ": " << std::endl;
    LogAndRun("populating graph", [&](){ g.Load(map); });
    std::vector< int > runs;
    LogAndRun("Total", [&]() {
        for (int i = 0; i < iterations; ++i)
        {
            bool succ = false;
            while (!succ) {
                try {
                    auto s = timer.now();
                    g.Dijkstra2(g.RandomVertex(), g.RandomVertex());
                    runs.push_back(duration_cast< milliseconds >(timer.now() - s).count());
                    succ = true;
                } catch (std::logic_error) {}
            }
        }
    });

    auto t = std::accumulate(runs.begin(), runs.end(), 0ul) / runs.size();
    std::cout << "Average: " << t << "ms" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
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

void Run(const std::string& file, int iterations) {
    auto map = GetMap(file);
//    RunImpl<ImplicitHeap>(map, iterations);
    RunImpl<ExplicitHeap>(map, iterations);
    RunImpl<FibonacciHeap>(map, iterations);
    RunImpl<BinomialHeap>(map, iterations);
    RunImpl<ViolationHeap>(map, iterations);
    RunImpl<RankPairingHeap>(map, iterations);
}


int main() {
    Run("maps/maze512-16-9.map", 10);

    return 0;
}
