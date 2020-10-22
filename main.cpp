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
#include <coroutine>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include <type_traits>
#include <utility>

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

using JaFactory = pf::abstract_factory<Hi, std::unique_ptr<Ja>, int>;

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
  pf::dependency<Hihi> d;

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

using CrMultiton = pf::multiton<int, Cr, CrBuilder>;
using CrSingleton = pf::singleton<Cr, CrBuilder>;

using chain = pf::chain_of_responsibility<int, std::string>;

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
using range_iter = pf::input_iterator<T, true, hihihi<T>>;

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
using out_i = pf::output_iterator<T, out_iter<T>, false>;

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
using fwd = pf::forward_iterator<T, true, hi<T>>;

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
using bi = pf::random_access_iterator<int, true, hi<int>>;

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
using itera_container = pf::bidirectional_iterator_container<T, false, itera_container_impl<T>>;

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

union bit_uint32 {
  uint32_t val;
  struct Bytes {
    uint8_t B0;
    uint8_t B1;
    uint8_t B2;
    uint8_t B3;
    uint8_t &operator[](std::size_t index) {
      switch (index) {
        case 0: return B0;
        case 1: return B1;
        case 2: return B2;
        case 3: return B3;
        default: throw std::runtime_error("Invalid byte index");
      }
    }
  } bytes;
  struct Bits {
    uint8_t b0 : 1;
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
    uint8_t b8 : 1;
    uint8_t b9 : 1;
    uint8_t b10 : 1;
    uint8_t b11 : 1;
    uint8_t b12 : 1;
    uint8_t b13 : 1;
    uint8_t b14 : 1;
    uint8_t b15 : 1;
    uint8_t b16 : 1;
    uint8_t b17 : 1;
    uint8_t b18 : 1;
    uint8_t b19 : 1;
    uint8_t b20 : 1;
    uint8_t b21 : 1;
    uint8_t b22 : 1;
    uint8_t b23 : 1;
    uint8_t b24 : 1;
    uint8_t b25 : 1;
    uint8_t b26 : 1;
    uint8_t b27 : 1;
    uint8_t b28 : 1;
    uint8_t b29 : 1;
    uint8_t b30 : 1;
    uint8_t b31 : 1;
    [[nodiscard]] uint8_t get_bit(std::size_t index) const {
      switch (index) {
        case 0: return b0;
        case 1: return b1;
        case 2: return b2;
        case 3: return b3;
        case 4: return b4;
        case 5: return b5;
        case 6: return b6;
        case 7: return b7;
        case 8: return b8;
        case 9: return b9;
        case 10: return b10;
        case 11: return b11;
        case 12: return b12;
        case 13: return b13;
        case 14: return b14;
        case 15: return b15;
        case 16: return b16;
        case 17: return b17;
        case 18: return b18;
        case 19: return b19;
        case 20: return b20;
        case 21: return b21;
        case 22: return b22;
        case 23: return b23;
        case 24: return b24;
        case 25: return b25;
        case 26: return b26;
        case 27: return b27;
        case 28: return b28;
        case 29: return b29;
        case 30: return b30;
        case 31: return b31;
        default: throw std::runtime_error("Invalid bit index");
      }
    }
    void set_bit(std::size_t index, bool value) {
      const uint8_t new_val = value ? 1 : 0;
      switch (index) {
        case 0: b0 = new_val; break;
        case 1: b1 = new_val; break;
        case 2: b2 = new_val; break;
        case 3: b3 = new_val; break;
        case 4: b4 = new_val; break;
        case 5: b5 = new_val; break;
        case 6: b6 = new_val; break;
        case 7: b7 = new_val; break;
        case 8: b8 = new_val; break;
        case 9: b9 = new_val; break;
        case 10: b10 = new_val; break;
        case 11: b11 = new_val; break;
        case 12: b12 = new_val; break;
        case 13: b13 = new_val; break;
        case 14: b14 = new_val; break;
        case 15: b15 = new_val; break;
        case 16: b16 = new_val; break;
        case 17: b17 = new_val; break;
        case 18: b18 = new_val; break;
        case 19: b19 = new_val; break;
        case 20: b20 = new_val; break;
        case 21: b21 = new_val; break;
        case 22: b22 = new_val; break;
        case 23: b23 = new_val; break;
        case 24: b24 = new_val; break;
        case 25: b25 = new_val; break;
        case 26: b26 = new_val; break;
        case 27: b27 = new_val; break;
        case 28: b28 = new_val; break;
        case 29: b29 = new_val; break;
        case 30: b30 = new_val; break;
        case 31: b31 = new_val; break;
        default: throw std::runtime_error("Invalid bit index");
      }
    }
  } bits;

  friend std::ostream &operator<<(std::ostream &stream, bit_uint32 &self) {
    std::stringstream ss;
    ss << "uint32: " << self.val << "\nbytes:";
    for (auto i = 0; i < 4; ++i) {
      ss << " " << uint32_t(self.bytes[i]);
    }
    ss << "\nbits:";
    for (auto i = 0; i < 32; ++i) {
      ss << " " << uint32_t(self.bits.get_bit(i));
    }
    stream << ss.str();
    return stream;
  }
};

template<typename T>
struct Unsigned {

  Unsigned(T value) : value(value) {
    assert(value >= T(0));
  }

  template<typename U>
  Unsigned &operator=(const Unsigned<U> &other) {
    value = other.value;
    return *this;
  }

  template<typename U>
  Unsigned &operator=(U other) {
    assert(other >= 0);
    value = other;
    return *this;
  }

  operator T() {
    return value;
  }

  T value;
};

#include <future>
#include <iostream>
class Package {
 public:
  Package(std::string sender, std::string receiver, float weight = 0.0f, int deliveryCost = 0) : sender(std::move(sender)),
                                                                                                 receiver(std::move(receiver)), deliveryCost(deliveryCost) {
    setWeight(weight);
  }

  inline void setWeight(float weight) {
    Package::weight = std::max(weight, 0.0f);
  }

  inline void replaceReceiverBySender() {
    receiver = sender;
  }

  inline void printInfo() const {
    std::cout << "Sender: " << sender << std::endl;
    std::cout << "Receiver: " << receiver << std::endl;
    std::cout << "Weight: " << weight << std::endl;
    std::cout << "Delivery cost: " << deliveryCost << std::endl;
  }

  inline friend std::ostream &operator<<(std::ostream &os, const Package &aPackage) {
    os << "sender: " << aPackage.sender << " receiver: " << aPackage.receiver << " weight: " << aPackage.weight << " deliveryCost: " << aPackage.deliveryCost;
    return os;
  }

  virtual ~Package() {
    std::cout << "Package deleted!" << std::endl;
  }

 private:
  std::string sender;
  std::string receiver;
  float weight = 0.0f;
  int deliveryCost;
};

class ThreadPool {
 public:
  explicit ThreadPool(uint32_t poolSize) : running(true) {
    for (auto i = 0; i < poolSize; ++i) {
      threads.emplace_back([this] { threadRun(); });
    }
  }

  void enqueue(std::invocable auto &&f) {
    std::unique_lock lck{mtx};
    const auto wakeup = queueTask.empty();
    queueTask.emplace(std::forward<decltype(f)>(f));
    if (wakeup) {
      cv.notify_one();
    }
  }

  void stop() {
    std::unique_lock lck{mtx};
    running = false;
    cv.notify_all();
  }

  void join() {
    for (auto &thread : threads) {
      thread.join();
    }
  }

 private:
  std::optional<std::function<void()>> getTask() {
    std::unique_lock lck{mtx};
    if (queueTask.empty()) {
      cv.wait(lck);
    }
    if (!queueTask.empty()) {
      auto task = std::move(queueTask.front());
      queueTask.pop();
      return std::move(task);
    }
    return std::nullopt;
  }

  void threadRun() {
    while (running) {
      auto task = getTask();
      if (task.has_value()) {
        task.value()();
      }
    }
  }

  std::mutex mtx;
  std::condition_variable cv;

  std::atomic<bool> running;
  std::vector<std::thread> threads;

  using Callable = std::function<void()>;
  std::queue<Callable> queueTask;
};

template<typename EventId = std::string>
class Dispatcher {
  ThreadPool pool{4};

 public:
  template<typename T = void>
  std::future<T> enqueue(std::invocable auto &&callable) {
    std::unique_lock lck(q_mtx);
    const auto wakeUp = isSleeping();
    auto promisePtr = new std::promise<T>();
    auto future = promisePtr->get_future();
    auto qCallable = [promisePtr, callable]() mutable {
      auto promise = std::unique_ptr<std::promise<T>>(promisePtr);
      if constexpr (std::same_as<T, void>) {
        callable();
        promise->set_value();
      } else {
        promise->set_value(callable());
      }
    };
    queueTask.emplace(std::move(qCallable));
    if (wakeUp) {
      main_cv.notify_one();
    }
    return future;
  }

  void delayed(std::chrono::milliseconds delay, std::invocable auto &&callable) {
    std::unique_lock lck(q_mtx);
    const auto wakeUp = isSleeping();
    const auto execTime = std::chrono::steady_clock::now() + delay;
    delayedTasks.emplace(execTime, callable);
    if (wakeUp) {
      main_cv.notify_one();
    }
  }

  void start() {
    running = true;
    mainThread = std::thread([this] {
      run();
    });
  }

  void setPaused(bool paused) {
    Dispatcher::paused = paused;
  }

  void notify(EventId event) {
    std::unique_lock lck(q_mtx);
    const auto wakeUp = isSleeping();
    queuedEvent.emplace(event);
    if (wakeUp) {
      main_cv.notify_one();
    }
  }

  class Canceler {
   public:
    explicit Canceler(std::invocable auto &&f) : fnc(f) {}

    void unsubscribe() {
      fnc();
    }

   private:
    std::function<void()> fnc;
  };

  Canceler observe(EventId event, std::invocable auto &&observer) {
    std::unique_lock lck(q_mtx);
    if (!eventListeners.contains(event)) {
      eventListeners.emplace();
    }
    const auto id = generateId();
    eventListeners[event].emplace_back(id, std::forward<decltype(observer)>(observer));
    return Canceler([this, event, id] {
      removeObserver(event, id);
    });
  }

  Canceler periodic(std::chrono::milliseconds period, std::invocable auto &&callable) {
    std::unique_lock lck(q_mtx);
    const auto wakeUp = isSleeping();
    const auto execTime = std::chrono::steady_clock::now() + period;
    periodicTasks.emplace(execTime, callable);
    if (wakeUp) {
      main_cv.notify_one();
    }
  }

  void join() {
    mainThread.join();
    pool.join();
  }

  void stop() {
    running = false;
    pool.stop();
  }

 private:
  bool isSleeping() const {
    return queueTask.empty() && delayedTasks.empty() && queuedEvent.empty() && periodicTasks.empty();
  }

  struct TimedCallable {
    std::chrono::steady_clock::time_point execTime;
    std::function<void()> fnc;
    TimedCallable(std::chrono::steady_clock::time_point execTime, std::invocable auto &&f) : execTime(execTime), fnc(f) {}

    void operator()() const {
      fnc();
    }

    bool operator<(const TimedCallable &rhs) const {
      return execTime < rhs.execTime;
    }
    bool operator>(const TimedCallable &rhs) const {
      return rhs < *this;
    }
    bool operator<=(const TimedCallable &rhs) const {
      return !(rhs < *this);
    }
    bool operator>=(const TimedCallable &rhs) const {
      return !(*this < rhs);
    }
  };

  void removeObserver(EventId event, uint32_t observerId) {
    if (auto iter = eventListeners.find(event); iter != eventListeners.end()) {
      if (auto i = std::find_if(iter->second.begin(), iter->second.end(), [observerId](const auto &pair) {
            return pair.first == observerId;
          });
          i != iter->second.end()) {
        iter->second.erase(i);
      }
    }
  }

  void run() {
    while (running) {
      if (queueTask.empty() && delayedTasks.empty() && queuedEvent.empty()) {
        std::unique_lock lck(main_mtx);
        main_cv.wait(lck);
      }
      if (!paused) {
        std::unique_lock lck(q_mtx);
        runDelayed();
        enqueueTaskToPool();
        notifyEvents();
      }
    }
  }

  void runDelayed() {
    const auto currentTime = std::chrono::steady_clock::now();
    while (!delayedTasks.empty() && delayedTasks.top().execTime <= currentTime) {
      pool.enqueue(std::move(delayedTasks.top()));
      delayedTasks.pop();
    }
  }

  void enqueueTaskToPool() {
    while (!queueTask.empty()) {
      pool.enqueue(std::move(queueTask.front()));
      queueTask.pop();
    }
  }

  void notifyEvents() {
    while (!queuedEvent.empty()) {
      if (auto iter = eventListeners.find(queuedEvent.front()); iter != eventListeners.end()) {
        pool.enqueue([iter] {
          for (auto &[id, listener] : iter->second) {
            listener();
          }
        });
      }
      queuedEvent.pop();
    }
  }

  std::mutex main_mtx;
  std::mutex q_mtx;
  std::condition_variable main_cv;

  std::thread mainThread;
  using Callable = std::function<void()>;
  std::queue<Callable> queueTask;

  std::priority_queue<TimedCallable, std::vector<TimedCallable>, std::greater<>> delayedTasks;
  std::priority_queue<TimedCallable, std::vector<TimedCallable>, std::greater<>> periodicTasks;
  std::atomic<bool> running;
  std::atomic<bool> paused;

  std::queue<EventId> queuedEvent;
  using EventListenerFnc = std::function<void()>;
  using EventListenerId = uint32_t;
  using EventListener = std::pair<EventListenerId, EventListenerFnc>;
  EventListenerId generateId() {
    static EventListenerId current = 0;
    return current++;
  }

  std::unordered_map<EventId, std::vector<EventListener>> eventListeners;
};

template <typename I, typename S>
concept SizeDiff = requires(I i, S s) {
  {s - i} -> std::convertible_to<std::size_t>;
};

auto advanceAndReturn(auto iter, auto cnt) {
  std::advance(iter, cnt);
  return iter;
}

template<std::forward_iterator Iterator, std::sentinel_for<Iterator> Sentinel = Iterator>
class IteratorSpan {
 public:
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using const_reference = const reference;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using const_pointer = const pointer;
  using iterator = Iterator;
  using sentinel = Sentinel;
  IteratorSpan(Iterator iter, Sentinel sent) : iter(iter), sent(sent) {}
  IteratorSpan(Iterator iter, std::size_t cnt) : iter(iter), sent(advanceAndReturn(iter, cnt)) {}

  reference operator[](std::size_t idx) requires std::random_access_iterator<Iterator> {
    return iter[idx];
  }

  const_reference operator[](std::size_t idx) const requires std::random_access_iterator<Iterator>  {
    return iter[idx];
  }

  Iterator begin() {
    return iter;
  }

  Sentinel end() {
    return sent;
  }

  [[nodiscard]] bool empty() const {
    return iter == sent;
  }

  [[nodiscard]] std::size_t size() const requires SizeDiff<Iterator, Sentinel> {
    return sent - iter;
  }

 private:
  Iterator iter;
  Sentinel sent;
};

int main() {
  auto pool = pf::object_pool<int, 10>();
  auto asdas = pool.lease();
  using namespace std::chrono_literals;
  auto dispatcher = Dispatcher();
  dispatcher.start();
  dispatcher.delayed(500ms, [] { std::cout << "delayed 1" << std::endl; });
  dispatcher.delayed(200ms, [] { std::cout << "delayed 2" << std::endl; });
  dispatcher.delayed(1000ms, [] { std::cout << "delayed 3" << std::endl; });
  dispatcher.enqueue([] {
    std::cout << "nani" << std::endl;
  });

  dispatcher.observe("dick", [] {
    std::cout << "dick1" << std::endl;
  });

  auto a = dispatcher.observe("dick", [] {
    std::cout << "dick2" << std::endl;
  });

  dispatcher.notify("dick");
  std::this_thread::sleep_for(2s);

  dispatcher.setPaused(true);
  a.unsubscribe();
  dispatcher.setPaused(false);

  dispatcher.notify("dick");

  dispatcher.stop();
  dispatcher.join();
  return 0;
}
/*
int main() {
  Unsigned<double> d{0};
  d = 100;
  d = -100;
  return 0;
}
*/