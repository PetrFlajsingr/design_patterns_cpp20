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

template <typename T, typename U, bool IsConst>
concept container_forward_iter = requires(T t, const T const_t) {
  {t.iter_init()};
  {t.iter_next()};
  { const_t.iter_valid() }
  ->std::convertible_to<bool>;
  { t.iter_current() }
  ->std::same_as<typename iterator_types<U, IsConst>::reference>;
};

template <typename T, typename U, bool IsConst>
concept container_bidirectional_iter = container_forward_iter<T, U, IsConst> && requires(T t, const T const_t) {
  {t.iter_previous()};
};




template<typename T, typename U, bool IsConst>
concept input_iter = requires(T t, const T const_t) {
  {t.next()};
  { const_t.valid() }
  ->std::convertible_to<bool>;
  { t.current() }
  ->std::same_as<typename iterator_types<U, IsConst>::reference>;
};

template<typename T, typename U>
concept output_iter = requires(T t, U u) {
  {t.assign(u)};
};

template<typename T, typename U, bool IsConst>
concept forward_iter = requires(T t, const T const_t, const T &t_ref, U u) {
  {t.next()};
  { const_t.equals(t_ref) }
  ->std::convertible_to<bool>;
  {t.next()};
  { t.current() }
  ->std::same_as<typename iterator_types<U, IsConst>::reference>;
};

template<typename T, typename U, bool IsConst>
concept comparable_forward_iter = requires(const T const_t, const T &t_ref) {
  { const_t.equals(t_ref) }
  ->std::convertible_to<bool>;
};

template<typename T, typename U, bool IsConst>
concept bidirectional_iter = forward_iter<T, U, IsConst> &&requires(T t, U u) {
  {t.previous()};
};

template<typename T, typename U, bool IsConst>
concept random_access_iter = bidirectional_iter<T, U, IsConst> &&requires(T t, const T const_t, U u, typename iterator_types<U, false>::difference_type diff) {
  {t.advance(diff)};
  { const_t.distance(t) }
  ->std::convertible_to<typename iterator_types<U, false>::difference_type>;
};

template<typename T, bool IsConst, input_iter<T, IsConst> Derived>
struct input_iterator : iterator_types<T, IsConst>, Derived {
  using iterator_category = std::input_iterator_tag;
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;

  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit input_iterator(Args &&... args) : Derived(std::forward<Args>(args)...) {}

  constexpr input_iterator() : Derived() {}

  constexpr bool operator==(const input_iterator &other) const {
    return !Derived::valid();
  }

  constexpr bool operator!=(const input_iterator &other) const {
    return !(this->operator==(other));
  }

  constexpr reference operator*() {
    return Derived::current();
  }

  constexpr reference operator*() const {
    return Derived::current();
  }

  constexpr pointer operator->() {
    return &Derived::current();
  }

  constexpr pointer operator->() const {
    return &Derived::current();
  }

  constexpr input_iterator &operator++() {
    Derived::next();
    return *this;
  }

  constexpr input_iterator operator++(int) {
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

  constexpr output_iterator &operator*() {
    return *this;
  }

  constexpr output_iterator &operator=(const T &value) {
    Derived::assign(value);
    return *this;
  }

  constexpr output_iterator &operator=(T &&value) {
    Derived::assign(std::move(value));
  }

  constexpr output_iterator &operator++() {
    return *this;
  }

  constexpr output_iterator operator++(int) {
    return *this;
  }
};

template<typename T, bool IsConst, forward_iter<T, IsConst> Derived>
struct forward_iterator : iterator_types<T, IsConst>, Derived {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::forward_iterator_tag;
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit forward_iterator(Args &&... args) : Derived(std::forward<Args>(args)...) {}

  constexpr forward_iterator() : Derived() {}

  constexpr bool operator==(const forward_iterator &other) const {
    return Derived::equals(other);
  }

  constexpr bool operator!=(const forward_iterator &other) const {
    return !(operator==(other));
  }

  constexpr reference operator*() {
    return Derived::current();
  }

  constexpr reference operator*() const {
    return Derived::current();
  }

  constexpr pointer operator->() {
    return &Derived::current();
  }

  constexpr pointer operator->() const {
    return &Derived::current();
  }

  constexpr forward_iterator &operator++() {
    Derived::next();
    return *this;
  }

  constexpr forward_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
};

template<typename T, bool IsConst, bidirectional_iter<T, IsConst> Derived>
struct bidirectional_iterator : forward_iterator<T, IsConst, Derived> {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::bidirectional_iterator_tag;

  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit bidirectional_iterator(Args &&... args) : forward_iterator<T, IsConst, Derived>(std::forward<Args>(args)...) {}

  constexpr bidirectional_iterator() : forward_iterator<T, IsConst, Derived>() {}

  constexpr bidirectional_iterator &operator++() {
    Derived::next();
    return *this;
  }

  constexpr bidirectional_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
  constexpr bidirectional_iterator &operator--() {
    Derived::previous();
    return *this;
  }

  constexpr bidirectional_iterator operator--(int) {
    auto result = *this;
    Derived::previous();
    return result;
  }
};

template<typename T, bool IsConst, random_access_iter<T, IsConst> Derived>
struct random_access_iterator : forward_iterator<T, IsConst, Derived> {
  using difference_type = typename iterator_types<T, IsConst>::difference_type;
  using value_type = typename iterator_types<T, IsConst>::value_type;
  using reference = typename iterator_types<T, IsConst>::reference;
  using pointer = typename iterator_types<T, IsConst>::pointer;
  using iterator_category = std::random_access_iterator_tag;
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit random_access_iterator(Args &&... args) : forward_iterator<T, IsConst, Derived>(std::forward<Args>(args)...) {}


  constexpr random_access_iterator &operator++() {
    Derived::next();
    return *this;
  }

  constexpr random_access_iterator operator++(int) {
    auto result = *this;
    Derived::next();
    return result;
  }
  constexpr random_access_iterator &operator--() {
    Derived::previous();
    return *this;
  }

  constexpr random_access_iterator operator--(int) {
    auto result = *this;
    Derived::previous();
    return result;
  }

  constexpr random_access_iterator &operator+=(difference_type difference) {
    Derived::advance(difference);
    return *this;
  }

  constexpr random_access_iterator &operator-=(difference_type difference) {
    Derived::advance(-difference);
    return *this;
  }

  constexpr random_access_iterator operator+(difference_type difference) const {
    auto result = *this;
    result += difference;
    return result;
  }

  constexpr friend random_access_iterator operator+(difference_type difference, const random_access_iterator &iter) {
    return iter + difference;
  }

  constexpr random_access_iterator operator-(difference_type difference) const {
    auto result = *this;
    result -= difference;
    return result;
  }

  constexpr difference_type operator-(const random_access_iterator &other) const {
    return Derived::distance(other);
  }

  constexpr int operator<=>(const random_access_iterator &rhs) const {
    return Derived::distance(rhs);
  }

  constexpr reference operator[](difference_type offset) const {
    return *(*this + offset);
  }
};



template<typename T, bool IsConst, container_forward_iter<T, IsConst> Derived>
struct forward_iterator_container : Derived {
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit forward_iterator_container(Args &&... args) : Derived(std::forward<Args>(args)...) {}

  constexpr forward_iterator_container() : Derived() {}

  struct iter {
    explicit iter(forward_iterator_container &container) : container(container) {
      container.iter_init();
    }
    void next() {
      container.iter_next();
    }

    bool equals(const iter &other) const {
      return !container.iter_valid();
    }

    T &current() const {
      return container.iter_current();
    }

    forward_iterator_container &container;
  };

  using iterator = forward_iterator<T, IsConst, iter>;

  iterator begin() {
    return iterator{*this};
  }
  iterator end() {
    return iterator{*this};
  }
};

template<typename T, bool IsConst, container_forward_iter<T, IsConst> Derived>
struct bidirectional_iterator_container : forward_iterator_container<T, IsConst, Derived>  {
  template<typename... Args>
  requires std::is_constructible_v<Derived, Args...> constexpr explicit bidirectional_iterator_container(Args &&... args) : forward_iterator_container<T, IsConst, Derived>(std::forward<Args>(args)...) {}

  constexpr bidirectional_iterator_container() : Derived() {}

  struct iter {
    explicit iter(bidirectional_iterator_container &container) : container(container) {
      container.iter_init();
    }
    void next() {
      container.iter_next();
    }
    void previous() {
      container.iter_previous();
    }

    bool equals(const iter &other) const {
      return !container.iter_valid();
    }

    T &current() const {
      return container.iter_current();
    }

    bidirectional_iterator_container &container;
  };

  using iterator = bidirectional_iterator<T, IsConst, iter>;
};

#endif//DESIGN_PATTERNS_ITERATOR_H
