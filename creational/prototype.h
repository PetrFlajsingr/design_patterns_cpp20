//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_PROTOTYPE_H
#define DESIGN_PATTERNS_PROTOTYPE_H

#include "../concepts.h"

template<copyable T>
class prototype {
 private:
  T prototype_instance;

 public:
  explicit prototype(const T &prototypeInstance) requires(copy_constructible<T>) : prototype_instance(prototypeInstance) {
  }

  explicit prototype(const T &prototypeInstance) requires(!copy_constructible<T> && copy_assignable<T>) {
    this->prototype_instance = prototypeInstance;
  }

  explicit prototype(T &&prototypeInstance) requires(move_constructible<T>) : prototype_instance(prototypeInstance) {
  }

  explicit prototype(T &&prototypeInstance) requires(!move_constructible<T> && move_assignable<T>) {
    this->prototype_instance = std::move(prototypeInstance);
  }

  prototype(prototype &&) = delete;
  prototype &operator=(prototype &&) = delete;

  prototype(const prototype &) = delete;
  prototype &operator=(const prototype &) = delete;

  T clone() {
    if constexpr (copy_constructible<T>) {
      return {prototype_instance};
    } else {
      auto result = T{};
      result = prototype_instance;
      return std::move(result);
    }
  }

  const T &get() const noexcept {
    return prototype_instance;
  }
};

#endif//DESIGN_PATTERNS_PROTOTYPE_H
