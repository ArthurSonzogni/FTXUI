// Copyright 2025 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#ifndef FTXUI_COMPONENT_MULTI_RECEIVER_BUFFER_HPP
#define FTXUI_COMPONENT_MULTI_RECEIVER_BUFFER_HPP

#include <algorithm>  // for std::replace, std::min, std::remove
#include <deque>      // for deque
#include <memory>     // for unique_ptr, make_unique
#include <vector>     // for vector

namespace ftxui {

template <typename T>
class MultiReceiverBuffer {
 public:
  class Receiver {
   public:
    explicit Receiver(MultiReceiverBuffer* buffer)
        : buffer_(buffer), index_(buffer->next_index_) {
      buffer_->receivers_.push_back(this);
    }

    Receiver(MultiReceiverBuffer* buffer, size_t index)
        : buffer_(buffer), index_(index) {
      buffer_->receivers_.push_back(this);
    }

    ~Receiver() {
      if (buffer_) {
        buffer_->RemoveReceiver(this);
      }
    }

    Receiver(const Receiver&) = delete;
    Receiver(Receiver&& other) noexcept
        : buffer_(other.buffer_), index_(other.index_) {
      other.buffer_ = nullptr;
      if (buffer_) {
        std::replace(buffer_->receivers_.begin(), buffer_->receivers_.end(),
                     &other, this);
      }
    }

    Receiver& operator=(const Receiver&) = delete;
    Receiver& operator=(Receiver&& other) noexcept {
      if (this != &other) {
        if (buffer_) {
          buffer_->RemoveReceiver(this);
        }
        buffer_ = other.buffer_;
        index_ = other.index_;
        other.buffer_ = nullptr;
        if (buffer_) {
          std::replace(buffer_->receivers_.begin(), buffer_->receivers_.end(),
                       &other, this);
        }
      }
      return *this;
    }

    bool Has() const { return buffer_ && index_ < buffer_->next_index_; }

    T Pop() {
      if (!Has()) {
        return {};
      }
      T value = buffer_->Get(index_);
      index_++;
      buffer_->Prune();
      return value;
    }

    size_t index() const { return index_; }

   private:
    friend class MultiReceiverBuffer;
    MultiReceiverBuffer* buffer_;
    size_t index_;
  };

  std::unique_ptr<Receiver> CreateReceiver() {
    return std::make_unique<Receiver>(this);
  }

  std::unique_ptr<Receiver> CreateReceiverAt(size_t index) {
    return std::make_unique<Receiver>(this, index);
  }

  void Push(T value) {
    values_.push_back(std::move(value));
    next_index_++;
  }

 private:
  void RemoveReceiver(Receiver* receiver) {
    receivers_.erase(
        std::remove(receivers_.begin(), receivers_.end(), receiver),
        receivers_.end());
    Prune();
  }

  void Prune() {
    if (receivers_.empty()) {
      values_.clear();
      start_index_ = next_index_;
      return;
    }
    size_t min_index = next_index_;
    for (auto* r : receivers_) {
      min_index = std::min(min_index, r->index_);
    }
    while (start_index_ < min_index) {
      values_.pop_front();
      start_index_++;
    }
  }

  T Get(size_t index) const { return values_[index - start_index_]; }

  std::deque<T> values_;
  std::vector<Receiver*> receivers_;
  size_t start_index_ = 0;
  size_t next_index_ = 0;
};

}  // namespace ftxui

#endif /* end of include guard: FTXUI_COMPONENT_MULTI_RECEIVER_BUFFER_HPP */
