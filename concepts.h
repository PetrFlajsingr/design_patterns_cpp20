//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_CONCEPTS_H
#define DESIGN_PATTERNS_CONCEPTS_H


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

#endif//DESIGN_PATTERNS_CONCEPTS_H
