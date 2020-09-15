#include <iostream>
#include <type_traits>
#include "creational/prototype.h"

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


int main() {

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
