#pragma once

#include <limits>
#include <stdexcept>

namespace MC {

class HeapBase {
public:
    constexpr static int Infinity = std::numeric_limits< int >::max();

    const std::string Name;

    HeapBase(const std::string& name) : Name(name) {}

    void EmptyException() const {
        throw std::logic_error("empty heap");
    }

    void InvalidKeyException() const {
        throw std::logic_error("provided key is higher than stored key");
    }
};

}