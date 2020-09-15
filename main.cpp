#include "creational/RAII.h"
#include "creational/abstract_factory.h"
#include "creational/dependency_injection.h"
#include "creational/multiton.h"
#include "creational/object_pool.h"
#include "creational/prototype.h"
#include "creational/singleton.h"
#include "creational/lazy_init.h"
#include <cassert>
#include <iostream>
#include <type_traits>

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

int main() {
  lazy_init<int> lazy{[] {std::cout << "init\n"; return 10;}};
  std::cout << "hihihi\n";
  std::cout << *lazy << std::endl;
  std::cout << "hihihi\n";

  return 0;
  int cnt = 0;
  object_pool<std::string, 10> pool{[&cnt] {++cnt;return std::to_string(cnt); }};
  std::vector<std::reference_wrapper<std::string>> vec1;
  for (std::size_t i = 0; i < 5; ++i) {
    vec1.emplace_back(pool.lease());
  }
  std::vector<std::reference_wrapper<std::string>> vec2;
  for (std::size_t i = 0; i < 5; ++i) {
    vec2.emplace_back(pool.lease());
  }


  for (const auto &ref : vec2) {
    std::cout << ref.get() << std::endl;
    pool.release(ref.get());
  }
  for (const auto &ref : vec1) {
    std::cout << ref.get() << std::endl;
    pool.release(ref.get());
  }

  std::vector<std::reference_wrapper<std::string>> vec3;
  for (std::size_t i = 0; i < 10; ++i) {
    vec3.emplace_back(pool.lease());
  }
  for (const auto &ref : vec3) {
    std::cout << ref.get() << std::endl;
    pool.release(ref.get());
  }

  return 0;
  auto &e = CrMultiton::get_instance(1);
  auto &e1 = CrMultiton::get_instance(1);
  auto &e2 = CrMultiton::get_instance(2);
  assert(&e == &e1);
  assert(&e != &e2);

  return 0;

  DI d;
  d.d = Hihi{};
  d.print();
  return 0;
  auto factory = create_JaFactory();
  factory.create(Hi::a, 1)->print();
  factory.create(Hi::b, 2)->print();
  factory.create(Hi::c, 3)->print();
  factory.create(Hi::d, 4)->print();

  Point a{.a = 1};
  Point b{.a = 10};
  std::cout << std::boolalpha << (a > b) << std::endl;
  return 0;
  prototype<Test> prototype{Test{1, 2, 3, 4}};

  {
    Test t = prototype.clone();
    t.print();
    t.a = 100;
    t.print();
    auto d = prototype.clone();
    d.print();
    prototype.get().print();
  }
  return 0;
}
