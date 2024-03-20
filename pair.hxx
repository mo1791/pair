//  ------------------------------------
//      Copyright (C) 2018 MO ELomari
//  ------------------------------------

// The pair class is very similar to std::pair, but if either of the
// template arguments are empty classes, then the "empty base-class
// optimization" is applied to compress the size of the pair.

#ifndef __PAIR_HXX__
#define __PAIR_HXX__

#include <iostream>
#include <tuple>
#include <concepts>


namespace detail {

// type trait to test if a class is a specialization of another class
template <typename, template <typename...> class>
struct is_specialization_of: public std::false_type {};

template <template <typename...> class Primary, typename... ARGS>
struct is_specialization_of<Primary<ARGS...>, Primary>: public std::true_type {};

template <typename T, template<typename...> class Primary>
concept specialization_of = is_specialization_of<typename std::decay<T>::type, Primary>::value;

// unpack a std::tuple into a type trait and use its value.

//  template <template <typename...> class Trait, typename T, typename Tuple>
//  struct unpack_tuple : public std::false_type {};

//  template <template <typename...> class Trait, typename T, typename... U>
//  struct unpack_tuple<Trait, T, std::tuple<U...>> : public Trait<T, U...> {};

//  template <template <typename...> class Trait, typename T, typename... U>
//  struct unpack_tuple<Trait, T, std::tuple<U...>&> : public Trait<T, U&...> {};

//  template <template <typename...> class Trait, typename T, typename... U>
//  struct unpack_tuple<Trait, T, const std::tuple<U...>> : public Trait<T, const U...> {};

//  template <template <typename...> class Trait, typename T, typename... U>
//  struct unpack_tuple<Trait, T, const std::tuple<U...>&> : public Trait<T, const U&...> {};

}  // namespace detail


// main class
template <typename T, typename U>
class pair
{

public:
    using first_type  = T;
    using second_type = U;

public:
    //  Default constructor. Value-initializes both elements of the pair, first and second
    constexpr pair()
        noexcept(std::conjunction<std::is_nothrow_default_constructible<T>,
                                  std::is_nothrow_default_constructible<U>>::value )
        requires(std::conjunction<std::is_default_constructible<T>,
                                  std::is_default_constructible<U>>::value)
        : m_first(), m_second()
    {
    }

    // Initializes m_first with first and m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T> and std::is_copy_constructible<U> are both true
    constexpr pair(const first_type& first, const second_type& second)
        noexcept(std::conjunction<std::is_nothrow_copy_constructible<T>,
                                  std::is_nothrow_copy_constructible<U>>::value )
        requires(std::conjunction<std::is_copy_constructible<T>,
                                  std::is_copy_constructible<U>>::value)
        : m_first(first), m_second(second)
    {
    }

    // Initializes m_first with std::forward<E>(first) and m_second with std::forward<F>(second).
    // This constructor participates in overload resolution if and only if
    // std::is_constructible<T, E> and std::is_constructible<U, F> are both true.
    template<typename E, typename F>
    constexpr pair(E&& first, F&& second)
        noexcept(std::conjunction<std::is_nothrow_constructible<T, E>,
                                  std::is_nothrow_constructible<U, F>>::value )
        requires(std::conjunction<std::is_constructible<T, E>,
                                  std::is_constructible<U, F>>::value)
        : m_first(std::forward_like<E>(first))
        , m_second(std::forward_like<F>(second))
    {
    }

    // Forwards the elements of tuple1 to the constructor of m_first
    // and forwards the elements of tuple2 to the constructor of m_second.
    template <template <typename...> class Tuple1, typename... Ts,
              template <typename...> class Tuple2, typename... Us>
    constexpr pair(std::piecewise_construct_t, const Tuple1<Ts...>& tuple1,
                                               const Tuple2<Us...>& tuple2)
        noexcept(std::conjunction<std::is_nothrow_constructible<T, Ts...>,
                                  std::is_nothrow_constructible<U, Us...>>::value)
        requires(std::conjunction<detail::is_specialization_of<Tuple1<Ts...>, std::tuple>,
                                  detail::is_specialization_of<Tuple2<Us...>, std::tuple>,
                                  std::is_constructible<T, Ts...>,
                                  std::is_constructible<U, Us...>>::value)
        : m_first(std::make_from_tuple<first_type>(tuple1))
        , m_second(std::make_from_tuple<second_type>(tuple2))
    {
    }

    //    template <typename Tuple1, typename Tuple2>
    //    constexpr compressed_pair_impl(
    //        std::piecewise_construct_t, Tuple1&& tuple1,
    //        Tuple2&&
    //            tuple2) noexcept(std::
    //                                 conjunction<
    //                                     unpack_tuple<std::is_nothrow_constructible,
    //                                                  T1, Tuple1>,
    //                                     unpack_tuple<std::is_nothrow_constructible,
    //                                                  T2, Tuple2>>::value)
    //        requires(std::conjunction<
    //                 detail::is_specialization_of<Tuple1, std::tuple>,
    //                 detail::is_specialization_of<Tuple2, std::tuple>,
    //                 unpack_tuple<std::is_constructible, T1, Tuple1>,
    //                 unpack_tuple<std::is_constructible, T2, Tuple2>>::value)
    //        : m_first( std::make_from_tuple<first_type>(tuple1) )
    //         , m_second( std::make_from_tuple<second_type>(tuple2) )
    //    {
    //    }

public:
    // Copy constructor 
    constexpr pair(const pair& other)
        noexcept(std::conjunction<std::is_nothrow_copy_constructible<T>,
                                  std::is_nothrow_copy_constructible<U>>::value)
        requires(std::conjunction<std::is_copy_constructible<T>,
                                  std::is_copy_constructible<U>>::value)
        : m_first(other.first())
        , m_second(other.second())
    {
    }

    // Move constructor
    constexpr pair(pair&& other)
        noexcept(std::conjunction<std::is_nothrow_move_constructible<T>,
                                  std::is_nothrow_move_constructible<U>>::value)
        requires(std::conjunction<std::is_move_constructible<T>,
                                  std::is_move_constructible<U>>::value)
        : m_first(std::move(other.first()))
        , m_second(std::move(other.second()))
    {
    }

    // Copy assignment operator
    constexpr auto operator=(const pair& rhs)
        noexcept(std::conjunction<std::is_nothrow_copy_assignable<T>,
                                  std::is_nothrow_copy_assignable<U>>::value) -> pair&
        requires(std::conjunction<std::is_copy_assignable<T>,
                                  std::is_copy_assignable<U>>::value)
    {
        m_first  = rhs.first();
        m_second = rhs.second();

        return *this;
    }

    // Move assignment operator
    constexpr auto operator=(pair&& rhs)
        noexcept(std::conjunction<std::is_nothrow_move_assignable<T>,
                                  std::is_nothrow_move_assignable<U>>::value) -> pair&
        requires(std::conjunction<std::is_move_assignable<T>,
                                  std::is_move_assignable<U>>::value)
    {
        m_first  = std::move( rhs.first() );
        m_second = std::move( rhs.second() );

        return *this;
    }                     


public:
    // access first element of a pair
    template <typename Self>
    constexpr auto first(this Self&& self) noexcept { return std::forward_like<Self>(self).m_first; }


    // access second element of a pair
    template <typename Self>
    constexpr auto second(this Self&& self) noexcept { return std::forward_like<Self>(self).m_second; }
 

private:
    [[no_unique_address]] first_type  m_first;
    [[no_unique_address]] second_type m_second;
};



/***  lexicographically compares the values in the pair ( Equality and Ordering Operators )  ***/
template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto lexicographical_compare(const pair<T, U>& lhs,
                                       const pair<T, U>& rhs) noexcept -> bool
{
    return std::tie(lhs.first(), lhs.second()) <
           std::tie(rhs.first(), rhs.second());
}

template <std::equality_comparable T, std::equality_comparable_with<T> U>
constexpr auto operator==(const pair<T, U>& lhs,
                          const pair<T, U>& rhs) noexcept -> bool
{
    return std::tie(lhs.first(), lhs.second()) ==
           std::tie(rhs.first(), rhs.second());
}

template <std::equality_comparable T, std::equality_comparable_with<T> U>
constexpr auto operator!=(const pair<T, U>& lhs,
                          const pair<T, U>& rhs) noexcept -> bool
{
    return not(lhs == rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator>(const pair<T, U>& lhs,
                         const pair<T, U>& rhs) noexcept -> bool
{
    return lexicographical_compare(rhs, lhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator>=(const pair<T, U>& lhs,
                          const pair<T, U>& rhs) noexcept -> bool
{
    return not(lhs < rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator<(const pair<T, U>& lhs,
                         const pair<T, U>& rhs) noexcept -> bool
{
    return lexicographical_compare(lhs, rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator<=(const pair<T, U>& lhs,
                          const pair<T, U>& rhs) noexcept -> bool
{
    return not(rhs < lhs);
}
/****** END ********/




/****** C++ structured binding support *******/
namespace std {

    template <typename T, typename U>
    struct tuple_size<::pair<T,U>> : public integral_constant<size_t, 2> {};

    template <size_t Index, typename T, typename U>
    struct tuple_element<Index, ::pair<T,U>>: public tuple_element<Index, tuple<T,U>> {}; 
}


template <std::size_t Index, detail::specialization_of<::pair> Pair>
constexpr auto get(Pair&& pair) noexcept  -> decltype(auto)
{
    if constexpr ( Index == 0 ) return std::forward_like<Pair>(pair).first();
    if constexpr ( Index == 1 ) return std::forward_like<Pair>(pair).second();
}

/****** END *******/

#endif

