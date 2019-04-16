#pragma once

#include <vector>
#include <functional>
#include <variant>

namespace randoms {
template < typename T >
struct is_ref_wrapper : std::false_type {};

template < typename T >
struct is_ref_wrapper< std::reference_wrapper< T > > : std::true_type {};

template < typename T >
struct resolve { using type = T; };

template < typename T >
struct resolve< std::reference_wrapper< T > > { using type = T; };


template < typename T >
inline constexpr bool is_ref_wrapper_v = is_ref_wrapper< T >::value;

template < typename T >
class queryable : protected std::vector< T > {
protected:




    template < typename Iter >
    queryable(Iter begin, Iter end) : base(begin, end) {}



//    template < std::size_t n >
//    friend queryable< wrap_type > as_queryable(const std::array< T, n >&);

public:

    using base = std::vector< T >;
    using base::value_type;
    using base::iterator;

    using decayed_t = std::decay_t< T >;
    using indexer_type = typename resolve< decayed_t >::type;
    using wrap_type = std::reference_wrapper< indexer_type >;
    using const_wrap_type = std::reference_wrapper< const indexer_type >;
    using work_type = indexer_type;

    static constexpr bool wrapped = is_ref_wrapper_v< decayed_t >;

    using reference = indexer_type&;
    using const_reference = const indexer_type&;

    explicit queryable(const std::vector< T >& v) : base(v) {}

    template < typename U, typename V >
    friend queryable< U > as_queryable(const std::vector< V >&);


    reference operator[](unsigned i) {
        return indexer< base, reference >(*this, i);
    }

    const_reference operator[](unsigned i) const {
        return indexer< const base, const_reference >(*this, i);
    }

    queryable< wrap_type > where(std::function< bool(const T&) > func) {
        return _where< wrap_type, queryable >(*this, func);
    }

    queryable< const_wrap_type > where(std::function< bool(const T&) > func) const {
        return _where< const_wrap_type, const queryable>(*this, func);
    }

private:

    template < typename RW, typename Q>
    static auto _where(Q& q, std::function< bool(const T&) > f) {
        std::vector< RW > ret;
        for (auto&& e : q) {
            if (f(e)) {
                ret.push_back(std::ref(e));
            }
        }
        return queryable< RW >(ret);
    }

    template < typename IN, typename OUT >
    static OUT indexer(IN& in, unsigned index) {
        if constexpr (wrapped) {
            return in[index].get();
        } else {
            return in[index];
        }

    }
};

template < template< typename, typename > class C, typename T, typename... Args >
auto transform_to_wrap(const C< T, Args... >& container) {
    using WT = typename queryable< T >::const_wrap_type;
    C< WT, Args... > transformed(container.size());
    std::transform(
            container.begin(),
            container.end(),
            transformed.begin(),
            [&transformed](auto&& elem) {
                return std::ref(elem);
            });
    return transformed;
}

template < typename T >
auto as_queryable(const std::vector< T >& v) {
    if constexpr (!std::is_copy_constructible_v< T >) {
        auto t = transform_to_wrap(v);
        using Type = typename decltype(t)::value_type;
        return queryable< Type >(t);
    } else {
        return queryable< T >(v);
    }

}

//template < typename T, std::size_t n >
//queryable< T > as_queryable(const std::array< T, n >& a) {
//    return queryable< T >(a.begin(), a.end());
//};



}



