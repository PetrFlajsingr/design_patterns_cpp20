//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_MULTITON_H
#define DESIGN_PATTERNS_MULTITON_H

#include "../concepts.h"
#include <unordered_map>
#include <memory>



template <hashable H, typename T, returning<T, H> Creator>
class multiton {
  friend class std::unique_ptr<multiton>;
 public:
  using key_type = H;
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;

  multiton(multiton &&) = delete;
  multiton &operator=(multiton &&) = delete;
  multiton(const multiton &) = delete;
  multiton &operator=(const multiton &) = delete;

  static multiton &get_instance(key_type h) {
    if (auto iter = instances.find(h); iter != instances.end()) {
      return *iter->second;
    }
    instances[h] = std::unique_ptr<multiton>(new multiton{h});
    return *instances[h];
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
  static inline std::unordered_map<key_type, std::unique_ptr<multiton>> instances;
  static inline std::optional<std::function<value_type(key_type)>> instance_creator = std::nullopt;
  value_type value;

  multiton(key_type h) : value(Creator{}(h)) {}

};

#endif//DESIGN_PATTERNS_MULTITON_H
