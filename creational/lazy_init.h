//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_LAZY_INIT_H
#define DESIGN_PATTERNS_LAZY_INIT_H

#include<functional>
#include "../concepts.h"

template <typename T>
class lazy_init {
 public:
  using calc_fnc = std::function<T()>;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;

  explicit lazy_init(simple_invocable auto calc) : calc(calc) {}

  pointer operator->() {
    calculate();
    return &*value;
  }

  reference operator*() {
    calculate();
    return *value;
  }

  const_pointer operator->() const {
    calculate();
    return *&value;
  }

  const_reference operator*() const {
    calculate();
    return *value;
  }

 private:
  void calculate() {
    if (value == nullptr) {
      value = std::make_unique<T>(calc());
    }
  }
  calc_fnc calc;
  std::unique_ptr<T> value;

};

#endif//DESIGN_PATTERNS_LAZY_INIT_H
