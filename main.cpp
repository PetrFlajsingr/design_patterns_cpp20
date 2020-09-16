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
#include <numeric>
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

  const T &current() {
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
using range_iter = input_iterator<T, true, hihihi<T>>;

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
  constexpr hi() {}
  constexpr hi(const int* iter) : iter(iter) {
  }
  constexpr void next() {
    ++iter;
  }

  constexpr bool equals(const hi &other) const {
    return iter == other.iter;
  }

  constexpr const T &current() {
    return *iter;
  }

  constexpr void previous() {
    --iter;
  }

  constexpr void advance(int diff) {
    iter += diff;
  }

  constexpr int distance(const hi &other) const {
    return iter - other.iter;
  }


  const int* iter;
};

template<typename T>
using fwd = forward_iterator<T, true, hi<T>>;

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
using bi = random_access_iterator<int, true, hi<int>>;

template <typename T>
concept printable = requires (T t) {
  {t.print()};
};

template <typename T>
struct itera_container_impl {
  itera_container_impl(std::vector<T> vec) : vec(vec) {}

  void iter_init() {
    iter = vec.begin();
  }

  void iter_next() {
    ++iter;
  }

  bool iter_valid() const {
    return iter != vec.end();
  }

  T &iter_current() {
    return *iter;
  }

  void iter_previous() {
    --iter;
  }

  std::vector<T> vec;

  typename std::vector<T>::iterator iter;
};

template<typename T>
using itera_container = bidirectional_iterator_container<T, false, itera_container_impl<T>>;


static_assert(std::random_access_iterator<bi<int>>);
constexpr std::array<int, 10> arr{1,1,1,1,1,1,1,1,1,1};
int main() {
  std::vector<int> a{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  itera_container<int> container{a};

  for (auto a : container) {
    std::cout << a << std::endl;
  }

  constexpr auto beg = bi<int>(arr.begin());
  constexpr auto end = bi<int>(arr.end());
  constexpr auto result = std::accumulate(beg, end, 0);
  std::cout << result;
  return 0;
}
