#pragma once

#define PAIRING2

#include "rp_heap.hpp"

namespace MC {
template < typename T >
class RankPairingHeap2 : public RankPairingHeap< T > {
public:
    RankPairingHeap2() : RankPairingHeap< T >("rank-pairing heap t2") {}
};
}
