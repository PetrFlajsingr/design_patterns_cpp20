#include "behavioral/chain_of_responsibility.h"
#include "behavioral/iterator.h"
#include "behavioral/visitor.h"
#include "creational/RAII.h"
#include "creational/abstract_factory.h"
#include "creational/dependency_injection.h"
#include "creational/lazy_init.h"
#include "creational/multiton.h"
#include "creational/object_pool.h"
#include "creational/prototype.h"
#include "creational/singleton.h"
#include <cassert>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <variant>

struct Test {
  int a;
  int b;
  int c;
  int d;
  Test() {}
  Test(int a, int b, int c, int d) : a(a), b(b), c(c), d(d) {}
  Test(Test &&other) = default;
  Test &operator=(Test &&other) = default;
  Test(const Test &) = default;
  Test &operator=(const Test &other) = default;

  void print() const {
    std::cout << a << " " << b << " " << c << " " << d << std::endl;
  }
};

struct Point {

  int a;
  int operator<=>(const Point &rhs) const {
    if (a < rhs.a) {
      return -1;
    } else if (a > rhs.a) {
      return 1;
    }
    return 0;
  }
};

enum class Hi {
  a,
  b,
  c,
  d
};

struct Ja {
  int a;
  virtual void print() {
    std::cout << a << "ja\n";
  }
};

struct Jaa : public Ja {
  void print() override {
    std::cout << a << "jaa\n";
  }
};

struct Jaaa : public Ja {
  void print() override {
    std::cout << a << "jaaa\n";
  }
};

using JaFactory = abstract_factory<Hi, std::unique_ptr<Ja>, int>;

JaFactory create_JaFactory() {
  return JaFactory{{
      {Hi::a, [](int a) {auto res = std::make_unique<Ja>();
                                 res->a = a;
                                 return res; }},
      {Hi::b, [](int a) {auto res = std::make_unique<Jaa>();
                                 res->a = a;
                                 return res; }},
      {Hi::c, [](int a) {auto res = std::make_unique<Jaaa>();
                                 res->a = a;
                                 return res; }},
      {Hi::d, [](int a) {auto res = std::make_unique<Ja>();
                                 res->a = a;
                                 return res; }},
  }};
}

struct Hihi {
  void print() {
    std::cout << "hihihi\n";
  }
};

struct DI {
  dependency<Hihi> d;

  void print() {
    d.get().print();
  }
};

struct Cr {
  int a;

  void print() {
    std::cout << a << std::endl;
  }
};

struct CrBuilder {
  Cr operator()(int a) {
    return Cr{.a = a};
  }
  Cr operator()() {
    return Cr{.a = 10};
  }
};

using CrMultiton = multiton<int, Cr, CrBuilder>;
using CrSingleton = singleton<Cr, CrBuilder>;

using chain = chain_of_responsibility<int, std::string>;

template<typename T>
class hihihi {
 public:
  hihihi(T start, T end) : curr(start), end(end) {}
  void next() {
    ++curr;
  }

  [[nodiscard]] bool valid() const {
    return curr != end;
  }

  T &current() {
    return curr;
  }

  [[nodiscard]] const T &current() const {
    return curr;
  }

 private:
  T curr;
  T end;
};
template<typename T>
using range_iter = input_iterator<T, hihihi<T>, false>;

template<typename T>
struct range {
  range(T start, T fin) : start(start), fin(fin) {}
  range_iter<T> begin() {
    return range_iter<T>{start, fin};
  }
  range_iter<T> end() {
    return range_iter<T>{0, 0};
  }
  const T start;
  const T fin;
};

template<typename T>
struct out_iter {
  void assign(T value) {
    std::cout << value << std::endl;
  }
};

template<typename T>
using out_i = output_iterator<T, out_iter<T>, false>;

template<typename T>
struct hi {
  hi() {}
  hi(typename std::vector<T>::iterator iter) : iter(iter) {
  }
  void next() {
    ++iter;
  }

  bool equals(const hi &other) const {
    return iter == other.iter;
  }

  T &current() {
    return *iter;
  }

  void previous() {
    --iter;
  }

  void advance(int diff) {
    iter += diff;
  }

  int distance(const hi &other) const {
    return iter - other.iter;
  }


  typename std::vector<T>::iterator iter;
};

template<typename T>
using fwd = forward_iterator<int, hi<int>, true>;

template <typename T>
struct vec{
  vec(std::vector<T> &v): v(v) {}
  fwd<T> begin() {
    return fwd<T>{v.begin()};
  }
  fwd<T> end() {
    return fwd<T>{v.end()};
  }

  std::vector<T> &v;
};

template<typename T>
using bi = random_access_iterator<int, hi<int>, true>;


static_assert(std::random_access_iterator<bi<int>>);
int main() {
  std::vector<int> a{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  auto iter1 = bi<int>(a.begin());
  auto iter2 = bi<int>(a.end());
  for (auto i = iter1; i != iter2 - 1; ++i) {
    std::cout << *i << " " << i[1] << std::endl;
  }
  --iter1;
  --iter2;
  for (auto i = iter2; i != iter1; --i) {
    std::cout << *i << std::endl;
  }

  return 0;
}
