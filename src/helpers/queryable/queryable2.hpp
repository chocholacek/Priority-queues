#pragma once

#include <vector>
#include <functional>

namespace randoms {

template < typename T >
class queryable;

template < typename T >
using wrap = std::reference_wrapper< T >;

template < typename T >
queryable< wrap< T > > as_queryable(std::vector< T >& v) {
    return queryable< wrap< T > >(v);
}

template < typename T >
class queryable : protected std::vector< T > {
protected:
    using base = std::vector< T >;
    using underlying_type = typename T::type;
    using wrapper = std::reference_wrapper< underlying_type  >;

    std::optional< std::vector< underlying_type > > tmp;

    static_assert(std::is_same_v< T, wrapper >);

    explicit queryable() = default;

    explicit queryable(std::vector< underlying_type >& v) {
        for (auto& e : v) {
            this->emplace_back(e);
        }
    }

    friend queryable as_queryable< underlying_type >(std::vector< underlying_type >&);

public:

    using base::begin;
    using base::end;

    queryable where(std::function< bool(const underlying_type&) > predicate) const {
        queryable q;

        for (const auto& w : *this) {
            if (predicate(w.get())) {
                q.emplace_back(w);
            }
        }

        return q;
    }

    template < typename X >
    auto select(std::function< X(const underlying_type&) > func) const {
        queryable< wrap< X > > q;
        std::vector< X > v;

        for (const auto& w :*this) {
            v.emplace_back(func(w.get()));
        }

        q.tmp =
        return (v);
    }

private:
};


}