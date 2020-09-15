//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_DEPENDENCY_INJECTION_H
#define DESIGN_PATTERNS_DEPENDENCY_INJECTION_H

#include <optional>

template<typename T>
class dependency {
 public:
  dependency() {}
  dependency(const dependency &other) = delete;
  dependency(dependency &&other) = delete;
  dependency &operator=(const dependency &other) = delete;
  dependency &operator=(dependency &&other) = delete;

  dependency &operator=(T &&value) {
    set(std::forward<T>(value));
    return *this;
  }

  [[nodiscard]] T &get() {
    if (!value.has_value()) {
      throw std::bad_optional_access{};
    }
    return value.value();
  }

  void install(T &&value) {
    this->value = value;
  }

  [[nodiscard]] bool is_set() const {
    return value.has_value();
  }

 private:
  std::optional<T> value = std::nullopt;
};

#endif//DESIGN_PATTERNS_DEPENDENCY_INJECTION_H
