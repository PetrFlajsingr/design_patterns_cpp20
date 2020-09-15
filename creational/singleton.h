//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_SINGLETON_H
#define DESIGN_PATTERNS_SINGLETON_H

template <typename T, returning<T> Creator>
class singleton {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;

  singleton(singleton &&) = delete;
  singleton &operator=(singleton &&) = delete;
  singleton(const singleton &) = delete;
  singleton &operator=(const singleton &) = delete;

  static singleton &get_instance() {
    static singleton instance{};
    return instance;
  }


  pointer operator->() {
    return &value;
  }

  reference operator*() {
    return value;
  }

  const_pointer operator->() const {
    return value;
  }

  const_reference operator*() const {
    return value;
  }

 private:
  singleton() : value(Creator{}()) {}
  T value;

};

#endif//DESIGN_PATTERNS_SINGLETON_H
