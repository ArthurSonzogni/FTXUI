#ifndef FTXUI_COMPONENTS_CONSUMER_PRODUCER_H_
#define FTXUI_COMPONENTS_CONSUMER_PRODUCER_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace ftxui {

// Usage:
//
// Initialization:
// ---------------
//
// auto consumer = MakeConsumer<std:string>();
// auto producer_1 = consumer.MakeProducer();
// auto producer_2 = consumer.MakeProducer();
//
// Then move one producers elsewhere, potentially in a different thread.
// ----------------------
// [thread 1] producer_1->Send("hello");
// [thread 2] producer_2->Send("world");
//
// On the consumer side:
// ---------------------
// char c;
// while(consumer_->Receive(&c)) // Return true as long as there is a producer.
//   print(c)
//
// Consumer::Receive returns true when the last Producer is released.

// clang-format off
template<class T> class ProducerImpl;
template<class T> class ConsumerImpl;
template<class T> using Producer = std::unique_ptr<ProducerImpl<T>>;
template<class T> using Consumer = std::unique_ptr<ConsumerImpl<T>>;
template<class T> Consumer<T> MakeConsumer();
// clang-format on

// ---- Implementation part ----

template <class T>
class ProducerImpl {
 public:
  void Send(T t) { consumer_->Receive(std::move(t)); }
  ~ProducerImpl() { consumer_->producers_--; }

 private:
  friend class ConsumerImpl<T>;
  ProducerImpl(ConsumerImpl<T>* consumer) : consumer_(consumer) {}
  ConsumerImpl<T>* consumer_;
};

template <class T>
class ConsumerImpl {
 public:
  Producer<T> MakeProducer() {
    producers_++;
    return std::unique_ptr<ProducerImpl<T>>(new ProducerImpl<T>(this));
  }

  bool Receive(T* t) {
    while (producers_) {
      std::unique_lock<std::mutex> lock(mutex_);
      while (queue_.empty())
        notifier_.wait(lock);
      if (queue_.empty())
        continue;
      *t = std::move(queue_.front());
      queue_.pop();
      return true;
    }
    return false;
  }

 private:
  friend class ProducerImpl<T>;

  void Receive(T t) {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(std::move(t));
    notifier_.notify_one();
  }

  std::mutex mutex_;
  std::queue<T> queue_;
  std::condition_variable notifier_;
  std::atomic<int> producers_ = 0;
};

template <class T>
Consumer<T> MakeConsumer() {
  return std::make_unique<ConsumerImpl<T>>();
}

}  // namespace ftxui

#endif  // FTXUI_COMPONENTS_CONSUMER_PRODUCER_H_
