//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_DEPENDENCY_INJECTION_H
#define DESIGN_PATTERNS_DEPENDENCY_INJECTION_H

#include <optional>

template<typename T>
class dependency {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  dependency() {}
  dependency(const dependency &other) = delete;
  dependency(dependency &&other) = delete;
  dependency &operator=(const dependency &other) = delete;
  dependency &operator=(dependency &&other) = delete;

  dependency &operator=(value_type &&value) {
    install(std::forward<value_type>(value));
    return *this;
  }

  [[nodiscard]] reference get() {
    return *value;
  }

  [[nodiscard]] const_reference get() const {
    return *value;
  }

  void install(value_type &&value) {
    this->value = value;
  }

  [[nodiscard]] bool is_set() const {
    return value.has_value();
  }

  pointer operator->() {
    return &*value;
  }

  reference operator*() {
    return *value;
  }

  const_pointer operator->() const {
    return &*value;
  }

  const_reference operator*() const {
    return *value;
  }

 private:
  std::optional<value_type> value = std::nullopt;
};

#endif//DESIGN_PATTERNS_DEPENDENCY_INJECTION_H
