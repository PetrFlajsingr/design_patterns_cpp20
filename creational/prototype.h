//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_PROTOTYPE_H
#define DESIGN_PATTERNS_PROTOTYPE_H

#include "../concepts.h"

namespace pf {
template<copyable T>
class prototype {
  static constexpr inline bool copy_constructing = std::copy_constructible<T>;
  static constexpr inline bool copy_assigning = !std::copy_constructible<T> && copy_assign_constructible<T>;
  static constexpr inline bool move_constructing = std::move_constructible<T>;
  static constexpr inline bool move_assigning = !std::move_constructible<T> && move_assign_constructible<T>;
  static constexpr inline bool is_noexcept_copyable = noexcept_copyable<T>;

 private:
  T prototype_instance;

 public:
  explicit prototype(const T &prototypeInstance) noexcept(is_noexcept_copyable) requires(copy_constructing) : prototype_instance(prototypeInstance) {
  }

  explicit prototype(const T &prototypeInstance) noexcept(is_noexcept_copyable) requires(copy_assigning) {
    this->prototype_instance = prototypeInstance;
  }

  explicit prototype(T &&prototypeInstance) noexcept requires(move_constructing) : prototype_instance(prototypeInstance) {
  }

  explicit prototype(T &&prototypeInstance) noexcept requires(move_assigning) {
    this->prototype_instance = std::move(prototypeInstance);
  }

  prototype(prototype &&) = delete;
  prototype &operator=(prototype &&) = delete;
  prototype(const prototype &) = delete;
  prototype &operator=(const prototype &) = delete;

  T clone() const noexcept(is_noexcept_copyable) {
    if constexpr (std::copy_constructible<T>) {
      return {prototype_instance};
    } else {
      auto result = T{};
      result = prototype_instance;
      return std::move(result);
    }
  }

  std::unique_ptr<T> cloneUnique()const noexcept(is_noexcept_copyable) {
    if constexpr (std::copy_constructible<T>) {
      return std::make_unique<T>(prototype_instance);
    } else {
      auto result = std::make_unique<T>();
      *result = prototype_instance;
      return std::move(result);
    }
  }

  std::shared_ptr<T> cloneShared()const noexcept(is_noexcept_copyable) {
    if constexpr (std::copy_constructible<T>) {
      return std::make_shared<T>(prototype_instance);
    } else {
      auto result = std::make_shared<T>();
      *result = prototype_instance;
      return std::move(result);
    }
  }

  const T &get() const noexcept {
    return prototype_instance;
  }
};

template<copyable T>
prototype<T> make_prototype(T &&instance) {
  return prototype(std::forward<T>(instance));
}

}// namespace pf
#endif//DESIGN_PATTERNS_PROTOTYPE_H
