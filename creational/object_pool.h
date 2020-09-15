//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_OBJECT_POOL_H
#define DESIGN_PATTERNS_OBJECT_POOL_H

#include "../concepts.h"
#include <algorithm>
#include <exception>
#include <list>
#include <mutex>

template<typename T, std::size_t PoolSize>
class object_pool {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;

  explicit object_pool(simple_invocable auto &&generator) {
    std::generate_n(std::back_inserter(available), PoolSize, [&generator] { return std::make_unique<T>(generator()); });
  }

  [[nodiscard]] reference lease() {
    std::unique_lock lock{mutex};
    if (available.empty()) {
      throw std::runtime_error{"Pool has no available objects."};
    }
    const auto first_available_iter = available.begin();
    auto &ref = in_use.emplace_front(std::move(*first_available_iter));
    available.erase(first_available_iter);
    return *ref;
  }

  void release(reference object) {
    std::unique_lock lock{mutex};
    const auto ptr = &object;
    if (auto iter = std::find_if(in_use.begin(), in_use.end(), [ptr](const auto &obj) {
          return &*obj == ptr;
        });
        iter != in_use.end()) {
      auto &ref = available.emplace_back(std::move(*iter));
      in_use.erase(iter);
    }
  }

 private:
  std::mutex mutex;
  std::list<std::unique_ptr<T>> in_use;
  std::list<std::unique_ptr<T>> available;
};

#endif//DESIGN_PATTERNS_OBJECT_POOL_H
