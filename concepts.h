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

}// namespace pf
#endif//DESIGN_PATTERNS_CONCEPTS_H
