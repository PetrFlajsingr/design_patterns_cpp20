//
// Created by Petr on 15.09.2020.
//
#ifndef DESIGN_PATTERNS_CONCEPTS_H
#define DESIGN_PATTERNS_CONCEPTS_H

#include <functional>
#include <istream>
#include <ostream>
#include <type_traits>

namespace pf {

template<typename T>
concept destructible = std::is_destructible_v<T>;

template<typename T>
concept enum_type = std::is_enum_v<T>;

template<typename F, typename T, typename... Args>
concept invocable_returning = std::same_as<std::invoke_result_t<F, Args...>, T>;

template<typename T>
concept hashable = requires(T t) {
  { std::hash<T>{}(t) }
  ->std::convertible_to<std::size_t>;
};

template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;

template<typename T>
concept copy_assign_constructible = copy_assignable<T> &&std::default_initializable<T>;

template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;

template<typename T>
concept move_assign_constructible = move_assignable<T> &&std::default_initializable<T>;

template<typename T>
concept copyable = std::copy_constructible<T> || copy_assign_constructible<T>;

template<typename T>
concept noexcept_copy_constructible = std::copy_constructible<T> &&noexcept(T(std::declval<const T &>()));

template<typename T>
concept noexcept_copy_assignable = copy_assignable<T> &&noexcept(T::operator==(std::declval<const T &>()));

template<typename T>
concept noexcept_copyable = copyable<T> && (noexcept_copy_constructible<T> || noexcept_copy_assignable<T>);

template<typename T>
concept movable = std::move_constructible<T> || move_assign_constructible<T>;

template<typename T>
concept stream_outputable = requires(const T &t, std::ostream &o) {
  { o << t }
  ->std::same_as<std::ostream>;
};

template<typename T>
concept stream_inputable = requires(T &t, std::istream &i) {
  { i >> t }
  ->std::same_as<std::istream>;
};

template<typename T>
concept streamable = stream_inputable<T> &&stream_outputable<T>;

template<typename T>
concept iterable = requires(T t) {
  { std::begin(t) }
  ->std::input_iterator;
  { std::end(t) }
  ->std::sentinel_for<decltype(std::begin(t))>;
};

template<typename T>
concept reverse_iterable = requires(T t) {
  { std::rbegin(t) }
  ->std::input_iterator;
  { std::rend(t) }
  ->std::sentinel_for<decltype(std::rbegin(t))>;
};

template<typename T, typename U>
concept reverse_iterable_of = reverse_iterable<T> &&requires(T t, decltype(std::rbegin(t)) it) {
  { *it }
  ->std::same_as<U>;
};

template<typename T>
concept const_reverse_iterable = requires(T t) {
  { std::crbegin(t) }
  ->std::input_iterator;
  { std::crend(t) }
  ->std::sentinel_for<decltype(std::crbegin(t))>;
};

template<typename T, typename U>
concept const_reverse_iterable_of = const_reverse_iterable<T> &&requires(T t, decltype(std::crbegin(t)) it) {
  { *it }
  ->std::same_as<const U>;
};

template<typename T, typename U>
concept iterable_of = iterable<T>
    &&requires(T t, decltype(std::begin(t)) it) {
  { *it }
  ->std::same_as<U>;
};

template<typename T>
concept const_iterable = requires(T t) {
  { std::cbegin(t) }
  ->std::input_iterator;
  { std::cend(t) }
  ->std::sentinel_for<decltype(std::cbegin(t))>;
};

template<typename T, typename U>
concept const_iterable_of = const_iterable<T>
    &&requires(T t, decltype(std::cbegin(t)) it) {
  { *it }
  ->std::same_as<const U>;
};

template<typename T>
concept container = std::is_default_constructible_v<T>
    &&std::copy_constructible<T>
        &&std::move_constructible<T>
            &&copy_assignable<T>
                &&move_assignable<T>
                    &&iterable<T>
                        &&const_iterable<T>
                            &&std::equality_comparable<T>
                                &&std::swappable<T>
                                    &&destructible<T>
                                        &&requires(T t) {
  typename T::value_type;
  typename T::reference;
  typename T::const_reference;
  typename T::iterator;
  typename T::const_iterator;
  typename T::difference_type;
  typename T::size_type;
  { t.size() }
  ->std::same_as<typename T::size_type>;
  { t.max_size() }
  ->std::same_as<typename T::size_type>;
  { t.empty() }
  ->std::convertible_to<bool>;
};

template<typename T>
concept reversible_container = container<T> &&reverse_iterable<T> &&const_reverse_iterable<T> &&requires(T t) {
  typename T::reverse_iterator;
  typename T::const_reverse_iterator;
};

template<typename T>
concept allocator_aware_container = container<T> &&requires(T t, const T &const_t, T &&move_t) {
  typename T::allocator_type;
  { t.get_allocator() }
  ->std::same_as<typename T::allocator_type>;
  { t = const_t }
  ->std::same_as<T &>;
  { t = move_t }
  ->std::same_as<T &>;
}
&&std::constructible_from<T, typename T::allocator_type>
    &&std::constructible_from<T, const T &, typename T::allocator_type>
        &&std::constructible_from<T, T &&, typename T::allocator_type>;

}// namespace pf
#endif//DESIGN_PATTERNS_CONCEPTS_H
