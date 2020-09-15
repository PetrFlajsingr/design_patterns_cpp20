//
// Created by Petr on 15.09.2020.
//

#ifndef DESIGN_PATTERNS_ABSTRACT_FACTORY_H
#define DESIGN_PATTERNS_ABSTRACT_FACTORY_H
#include "../concepts.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <exception>


template<enum_type E, typename ResultType, typename ...Args>
class abstract_factory {
 public:
  using constructor_map = std::unordered_map<E, std::function<ResultType(Args...)>>;

  explicit abstract_factory(constructor_map &&constructors) : constructors(constructors) {}

  ResultType create(E type, Args &&...args) {
    if (const auto iter = constructors.find(type); iter != constructors.end()) {
      return iter->second(std::forward<Args>(args)...);
    }
    throw std::invalid_argument("Invalid enum type in abstract_factory.");
  }

 private:
  constructor_map constructors;

};

#endif//DESIGN_PATTERNS_ABSTRACT_FACTORY_H
