#include "creational/abstract_factory.h"
#include "creational/prototype.h"
#include "creational/dependency_injection.h"
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

int main() {
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
