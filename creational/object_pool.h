//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_OBJECT_POOL_H
#define DESIGN_PATTERNS_OBJECT_POOL_H

#include "../concepts.h"
#include <algorithm>
#include <concepts>
#include <exception>
#include <iostream>
#include <list>
#include <mutex>
#include <experimental/memory>

namespace pf {

enum class pool_alloc_strategy {
  preallocate,
  increase_by_2x,
  on_demand
};


namespace details {
template <pool_alloc_strategy Strategy>
struct default_pool_allocator_data {};
template <>
struct default_pool_allocator_data<pool_alloc_strategy::increase_by_2x> {
  std::size_t capacity_ = 0;
};
template <>
struct default_pool_allocator_data<pool_alloc_strategy::on_demand> {
  std::size_t capacity_ = 0;
};

template<typename T, std::size_t PoolSize, pool_alloc_strategy Strategy>
struct default_pool_allocator {
  default_pool_allocator_data<Strategy> data;

  explicit default_pool_allocator(std::invocable auto &&generator,
                                  std::list<std::unique_ptr<T>> &available,
                                  std::list<std::unique_ptr<T>> &in_use) : generator(generator) {
    if constexpr (Strategy == pool_alloc_strategy::preallocate) {
      alloc_n(available, in_use, PoolSize);
    }
  }
  void on_lease(std::list<std::unique_ptr<T>> &available, std::list<std::unique_ptr<T>> &in_use) {
    if constexpr (Strategy == pool_alloc_strategy::increase_by_2x) {
      if (data.capacity_ == in_use.size() && data.capacity_ < PoolSize) {
        const auto n_to_alloc = std::max(std::min(data.capacity_ * 2, PoolSize - data.capacity_), 1ull);
        alloc_n(available, in_use,n_to_alloc);
        data.capacity_ += n_to_alloc;
      }
    } else if constexpr (Strategy == pool_alloc_strategy::on_demand) {
      if (data.capacity_ == in_use.size() && data.capacity_ < PoolSize) {
        alloc_n(available, in_use,1);
        ++data.capacity_;
      }
    }
  }
  void on_release(std::list<std::unique_ptr<T>> &available, std::list<std::unique_ptr<T>> &in_use) {
  }

  std::size_t capacity() {
    return data.capacity_;
  }

  void shrink(std::list<std::unique_ptr<T>> &available, std::list<std::unique_ptr<T>> &in_use) {
    const auto remove_cnt = in_use.size();
    in_use.clear();
    data.capacity_ -= remove_cnt;
  }

  void alloc_n(std::list<std::unique_ptr<T>> &available, std::list<std::unique_ptr<T>> &in_use, std::size_t n) {
    std::generate_n(std::back_inserter(available), n, [&] { return std::make_unique<T>(generator()); });
    std::cout << "allocating " << n << std::endl;
  }

  std::function<T()> generator;
};
}// namespace details

template<typename T, std::size_t PoolSize, pool_alloc_strategy Strategy = pool_alloc_strategy::preallocate>
class object_pool {
  using pool_allocator = details::default_pool_allocator<T, PoolSize, Strategy>;
 public:
  using size_type = std::size_t;
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using pointer = std::experimental::observer_ptr<T>;
  using const_pointer = std::experimental::observer_ptr<const T>;

  object_pool() requires std::default_initializable<T> : allocator([] {return T();}, available_, in_use) {}

  explicit object_pool(std::invocable auto &&generator) : allocator(generator, available_, in_use) {
  }

  [[nodiscard]] pointer lease() {
    std::unique_lock lock{mutex};
    allocator.on_lease(available_, in_use);
    if (available_.empty()) {
      throw std::runtime_error{"Pool has no available objects."};
    }
    const auto first_available_iter = available_.begin();
    auto &ref = in_use.emplace_front(std::move(*first_available_iter));
    available_.erase(first_available_iter);
    return std::experimental::make_observer(ref.get());
  }

  void release(pointer object) {
    std::unique_lock lock{mutex};
    if (auto iter = std::find_if(in_use.begin(), in_use.end(), [&object](const auto &obj) {
          return obj.get() == object.get();
        });
        iter != in_use.end()) {
      auto &ref = available_.emplace_back(std::move(*iter));
      in_use.erase(iter);
      allocator.on_release(available_, in_use);
    }
  }

  [[nodiscard]] size_type capacity() const{
    if constexpr (Strategy == pool_alloc_strategy::preallocate) {
      return PoolSize;
    } else {
      return allocator.data.capacity();
    }
  }

  void shrink_to_fit() requires (Strategy != pool_alloc_strategy::preallocate) {
    allocator.shrink(available_, in_use);
  }

  [[nodiscard]] size_type used() const {
    return in_use.size();
  }

  [[nodiscard]] size_type available() const {
    return available_.size();
  }

 private:
  std::mutex mutex;
  std::list<std::unique_ptr<T>> in_use;
  std::list<std::unique_ptr<T>> available_;
  pool_allocator allocator;
};
}// namespace pf
#endif//DESIGN_PATTERNS_OBJECT_POOL_H
