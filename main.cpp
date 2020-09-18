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
  constexpr hi(const int *iter) : iter(iter) {
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

  const int *iter;
};

template<typename T>
using fwd = forward_iterator<T, true, hi<T>>;

template<typename T>
struct vec {
  vec(std::vector<T> &v) : v(v) {}
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

template<typename T>
concept printable = requires(T t) {
  {t.print()};
};

template<typename T>
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

struct TTT {
  int a;
};

namespace detail {
template<typename Ignore>
struct anything {
  template<typename T,
           typename = std::enable_if_t<not std::is_same<Ignore, std::decay_t<T>>{}>>
  operator T &&();
};

template<typename U, typename = void, typename... args>
struct test : test<U, void, args..., anything<U>> {};
template<typename U, typename... args>
struct test<U, std::enable_if_t<std::is_constructible<U, args...>{} && sizeof...(args) < 32>, args...>
    : std::integral_constant<std::size_t, sizeof...(args)> {};
template<typename U, typename... args>
struct test<U, std::enable_if_t<sizeof...(args) == 32>, args...>
    : std::integral_constant<std::size_t, (std::size_t) -1> {};
}// namespace detail

template<typename U>
using ctor_arity = detail::test<U, void>;

template<typename... Args>
struct composite : Args... {
  template<typename... CtorArgs>
  explicit composite(CtorArgs &&... args) : Args(ctor_arity<Args>::value)... {}
};

static_assert(std::random_access_iterator<bi<int>>);
constexpr std::array<int, 10> arr{1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

struct A {
  A(int a) : a(a) {}
  void printA() {
    std::cout << a << " A\n";
  }
  int a;
};
struct B {
  B(int b) : b(b) {}
  void printB() {
    std::cout << b << " B\n";
  }
  int b;
};
struct C {
  C(int c) : c(c) {}
  void printC() {
    std::cout << c << " C\n";
  }
  int c;
};

namespace details {
template<class>
struct is_ref_wrapper : std::false_type {};
template<class T>
struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

template<class T>
using not_ref_wrapper = std::negation<is_ref_wrapper<std::decay_t<T>>>;

template<class D, class...>
struct return_type_helper { using type = D; };
template<class... Types>
struct return_type_helper<void, Types...> : std::common_type<Types...> {
  static_assert(std::conjunction_v<not_ref_wrapper<Types>...>,
                "Types cannot contain reference_wrappers when D is void");
};

template<class D, class... Types>
using return_type = std::array<typename return_type_helper<D, Types...>::type,
                               sizeof...(Types)>;
}// namespace details

template<class D = void, class... Types>
constexpr details::return_type<D, Types...> make_array(Types &&... t) {
  return {std::forward<Types>(t)...};
}

int main() {
  auto arr = make_array(1, 2, 3.1, 4, 5, 6, 7);
  return 0;
}
