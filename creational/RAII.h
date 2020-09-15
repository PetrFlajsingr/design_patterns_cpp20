//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_RAII_H
#define DESIGN_PATTERNS_RAII_H

#include "../concepts.h"

class RAII {
 public:
  template<simple_invocable F>
  explicit RAII(F &&callable) : callable(std::forward<F>(callable)) {}

  ~RAII() {
    callable();
  }

 private:
  std::function<void()> callable;
};

#endif//DESIGN_PATTERNS_RAII_H
