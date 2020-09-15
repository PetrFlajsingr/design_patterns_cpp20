//
// Created by xflajs00 on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_ITERATOR_H
#define DESIGN_PATTERNS_ITERATOR_H

#include <concepts>
#include <iterator>


template<typename T, bool IsConst>
struct iterator_types {
  using difference_type = int64_t;
  using value_type = T;
  using pointer = std::conditional_t<IsConst, const T *, T *>;
  using reference = std::conditional_t<IsConst, const T &, T &>;
};

template<typename T, typename U>
concept input_iter = requires(T t, const T const_t) {
  {t.next()};
  { const_t.valid() }
  ->std::convertible_to<bool>;
  { t.current() }
  ->std::same_as<typename iterator_types<U, false>::reference>;
};

template<typename T, typename U>
concept output_iter = requires(T t, U u) {
  {t.assign(u)};
};

template<typename T, typename U>
concept forward_iter = requires(T t, const T const_t, const T &t_ref, U u) {
  { const_t.equals(t_ref) }
  ->std::convertible_to<bool>;
  {t.next()};
  { t.current() }
  ->std::same_as<typename iterator_types<U, false>::reference>;
};

template<typename T, typename U>
concept bidirectional_iter = forward_iter<T, U> && requires(T t, U u) {
  {t.previous()};
};

template<typename T, typename U>
concept random_access_iter = bidirectional_iter<T, U> && requires(T t, const T const_t, U u, typename iterator_types<U, false>::difference_type diff) {
  {t.advance(diff)};
  {const_t.distance(t)} -> std::convertible_to<typename iterator_types<U, false>::difference_type>;
};

template<typename T, input_iter<T> Derived, bool IsConst>
struct input_iterator : iterator_types<T, IsConst>, Derived {
  using iterator_category = std::input_iterator_tag;
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;

  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> explicit input_iterator(Args &&... args) : Derived(std::forward<Args>(args)...) {}

  input_iterator() : Derived() {}

  bool operator==(const Derived &other) const {
    return !Derived::valid();
  }

  bool operator!=(const input_iterator &other) const {
    return !(this->operator==(other));
  }

  reference operator*() {
    return Derived::current();
  }

  reference operator*() const {
    return Derived::current();
  }

  pointer operator->() {
    return &Derived::current();
  }

  pointer operator->() const {
    return &Derived::current();
  }

  input_iterator &operator++() {
    Derived::next();
    return *this;
  }

  input_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
};

template<typename T, output_iter<T> Derived, bool IsConst>
struct output_iterator : iterator_types<T, IsConst>, Derived {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::output_iterator_tag;

  output_iterator &operator*() {
    return *this;
  }

  output_iterator &operator=(const T &value) {
    Derived::assign(value);
    return *this;
  }

  output_iterator &operator=(T &&value) {
    Derived::assign(std::move(value));
  }

  output_iterator &operator++() {
    return *this;
  }

  output_iterator operator++(int) {
    return *this;
  }
};

template<typename T, forward_iter<T> Derived, bool IsConst>
struct forward_iterator : iterator_types<T, IsConst>, Derived {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::forward_iterator_tag;
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> explicit forward_iterator(Args &&... args) : Derived(std::forward<Args>(args)...) {}

  forward_iterator() : Derived() {}


  bool operator==(const forward_iterator &other) const {
    return Derived::equals(other);
  }

  bool operator!=(const forward_iterator &other) const {
    return !(operator==(other));
  }

  reference operator*() {
    return Derived::current();
  }

  reference operator*() const {
    return Derived::current();
  }

  pointer operator->() {
    return &Derived::current();
  }

  pointer operator->() const {
    return &Derived::current();
  }

  forward_iterator &operator++() {
    Derived::next();
    return *this;
  }

  forward_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
};

template<typename T, bidirectional_iter<T> Derived, bool IsConst>
struct bidirectional_iterator : forward_iterator<T, Derived, IsConst> {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::bidirectional_iterator_tag;

  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> explicit bidirectional_iterator(Args &&... args) : forward_iterator<T, Derived, IsConst>(std::forward<Args>(args)...) {}

  bidirectional_iterator() : forward_iterator<T, Derived, IsConst>() {}

  bidirectional_iterator &operator++() {
    Derived::next();
    return *this;
  }

  bidirectional_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
  bidirectional_iterator &operator--() {
    Derived::previous();
    return *this;
  }

  bidirectional_iterator operator--(int) {
    auto result = *this;
    Derived::previous();
    return result;
  }
};


template<typename T, random_access_iter<T> Derived, bool IsConst>
struct random_access_iterator : forward_iterator<T, Derived, IsConst> {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::random_access_iterator_tag;
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> explicit random_access_iterator(Args &&... args) : forward_iterator<T, Derived, IsConst>(std::forward<Args>(args)...) {}

  random_access_iterator() : forward_iterator<T, Derived, IsConst>() {}

  random_access_iterator &operator++() {
    Derived::next();
    return *this;
  }

  random_access_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
  random_access_iterator &operator--() {
    Derived::previous();
    return *this;
  }

  random_access_iterator operator--(int) {
    auto result = *this;
    Derived::previous();
    return result;
  }

  random_access_iterator &operator+=(difference_type difference) {
    Derived::advance(difference);
    return *this;
  }

  random_access_iterator &operator-=(difference_type difference) {
    Derived::advance(-difference);
    return *this;
  }

  random_access_iterator operator+(difference_type difference) const {
    auto result = *this;
    result += difference;
    return result;
  }

  friend random_access_iterator operator+(difference_type difference, const random_access_iterator &iter) {
    return iter + difference;
  }

  random_access_iterator operator-(difference_type difference) const {
    auto result = *this;
    result -= difference;
    return result;
  }

  difference_type operator-(const random_access_iterator &other) const {
    return Derived::distance(other);
  }

  int operator<=>(const random_access_iterator &rhs) const {
    return Derived::distance(rhs);
  }

  reference operator[](difference_type offset) const {
    return *(*this + offset);
  }
};

#endif//DESIGN_PATTERNS_ITERATOR_H
