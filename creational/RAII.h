//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_RAII_H
#define DESIGN_PATTERNS_RAII_H

#include <functional>

namespace pf {
class RAII {
 public:
  template<std::invocable F>
  explicit RAII(F &&callable) : callable(std::forward<F>(callable)) {}

  ~RAII() {
    std::invoke(callable);
  }

 private:
  std::function<void()> callable;
};
}// namespace pf
#endif//DESIGN_PATTERNS_RAII_H
