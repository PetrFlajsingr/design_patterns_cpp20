//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_CONCEPTS_H
#define DESIGN_PATTERNS_CONCEPTS_H

#include <type_traits>
#include <functional>

template<typename T>
concept copy_constructible = std::is_copy_constructible_v<T>;

template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;

template<typename T>
concept move_constructible = std::is_move_constructible_v<T>;

template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;

template<typename T>
concept copy_assign_movable = copy_assignable<T> &&std::is_constructible_v<T> &&move_constructible<T>;

template<typename T>
concept copyable = copy_constructible<T> || copy_assign_movable<T>;

template<typename T>
concept enum_type = std::is_enum_v<T>;

template<typename T, typename U>
concept same = std::is_same_v<T, U>;

template<typename From, typename To>
concept convertible_to = std::is_convertible_v<From, To>;

template<typename F, typename... Args>
concept invocable = std::is_invocable_v<F, Args...>;

template<typename F>
concept simple_invocable = std::is_invocable_v<F>;

template<typename F, typename R, typename... Args>
concept returning = requires(F f, Args... args) {
  { f(args...) }
  ->convertible_to<R>;
} && invocable<F, Args...>;

template<typename T>
concept hashable = requires(T t) {
  { std::hash<T>{}(t)}
  ->convertible_to<std::size_t>;
};


#endif//DESIGN_PATTERNS_CONCEPTS_H
