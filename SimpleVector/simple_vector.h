#pragma once
#include "array_ptr.h"
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>
struct ReserveProxyObj {
  explicit ReserveProxyObj(size_t capacity) : capacity(capacity) {}

  size_t capacity;
};

ReserveProxyObj Reserve(size_t capacity) { return ReserveProxyObj(capacity); }

template <typename Type> class SimpleVector {
private:
  ArrayPtr<Type> items_;
  size_t size_ = 0;
  size_t capacity_ = 0;

  // CONSTRUCTORS
public:
  using Iterator = Type *;
  using ConstIterator = const Type *;

  SimpleVector() noexcept = default;

  explicit SimpleVector(size_t size)
      : items_(size), size_(size), capacity_(size) {}

  SimpleVector(ReserveProxyObj reserve)
      : items_(reserve.capacity), size_(0), capacity_(reserve.capacity) {}

  SimpleVector(size_t size, const Type &value)
      : items_(size), size_(size), capacity_(size) {
    std::fill(items_.Get(), (items_.Get() + size_), value);
  }

  SimpleVector(std::initializer_list<Type> init)
      : items_(init.size()), size_(init.size()), capacity_(init.size()) {

    std::copy(init.begin(), init.end(), items_.Get());
  }

  SimpleVector(const SimpleVector &other)
      : items_(other.size_), size_(other.size_), capacity_(other.size_) {
    if (size_ > 0) {
      std::copy(other.begin(), other.end(), items_.Get());
    }
  }

  SimpleVector(SimpleVector &&other)
      : items_(std::move(other.items_)), size_(std::exchange(other.size_, 0)),
        capacity_(std::exchange(other.capacity_, 0)) {};

  // OPERATORS
public:
  SimpleVector &operator=(const SimpleVector &rhs) {
    if (this == &rhs) {
      return *this;
    }
    SimpleVector tmp(rhs);
    this->swap(tmp);
    return *this;
  }

  SimpleVector &operator=(SimpleVector &&rhs) {
    if (this != &rhs) {
      items_ = std::move(rhs.items_);
      size_ = std::exchange(rhs.size_, 0);
      capacity_ = std::exchange(rhs.capacity_, 0);
    }
    return *this;
  }

  Type &operator[](size_t index) noexcept { return items_[index]; }

  const Type &operator[](size_t index) const noexcept { return items_[index]; }

  // METHODS
public:
  void swap(SimpleVector &other) noexcept {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    items_.swap(other.items_);
  }

  void Reserve(size_t new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    ArrayPtr<Type> tmp(new_capacity);

    // move_if_noexcept: Перемещение, если перемещающий конструктор
    // помечен noexcept или тип нельзя копировать.
    // Копирование в остальных случаях
    for (size_t i = 0; i < size_; ++i) {
      tmp[i] = std::move_if_noexcept(items_[i]);
    }
    items_.swap(tmp);
    capacity_ = new_capacity;
  }
  void PushBack(const Type &item) {
    if (size_ < capacity_) {
      items_[size_++] = item;
      return;
    }
    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    ArrayPtr<Type> tmp(new_capacity);

    std::copy(items_.Get(), items_.Get() + size_, tmp.Get());
    tmp[size_++] = item;
    items_.swap(tmp);
    capacity_ = new_capacity;
  }

  void PushBack(Type &&item) {
    if (size_ < capacity_) {
      items_[size_++] = std::move(item);
      return;
    }
    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    ArrayPtr<Type> tmp(new_capacity);

    std::move(items_.Get(), items_.Get() + size_, tmp.Get());
    tmp[size_++] = std::move(item);
    items_.swap(tmp);
    capacity_ = new_capacity;
  }

  Iterator Insert(ConstIterator pos, const Type &value) {
    size_t index = pos - cbegin();
    if (size_ < capacity_) {
      std::move_backward(items_.Get() + index, items_.Get() + size_,
                         items_.Get() + size_ + 1);
      items_[index] = value;
      size_++;
      return begin() + index;
    }
    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    ArrayPtr<Type> tmp(new_capacity);
    for (size_t i = 0; i < index; ++i) {
      tmp[i] = items_[i];
    }

    tmp[index] = value;

    for (size_t i = index; i < size_; ++i) {
      tmp[i + 1] = std::move_if_noexcept(items_[i]);
    }
    items_.swap(tmp);
    capacity_ = new_capacity;
    size_++;
    return begin() + index;
  }

  Iterator Insert(ConstIterator pos, Type &&value) {
    size_t index = pos - cbegin();
    if (size_ < capacity_) {
      std::move_backward(items_.Get() + index, items_.Get() + size_,
                         items_.Get() + size_ + 1);
      items_[index] = std::move(value);
      size_++;
      return begin() + index;
    }
    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    ArrayPtr<Type> tmp(new_capacity);
    for (size_t i = 0; i < index; ++i) {
      tmp[i] = std::move(items_[i]);
    }

    tmp[index] = std::move(value);

    for (size_t i = index; i < size_; ++i) {
      tmp[i + 1] = std::move_if_noexcept(items_[i]);
    }
    items_.swap(tmp);
    capacity_ = new_capacity;
    size_++;
    return begin() + index;
  }

  void PopBack() noexcept {
    if (size_ > 0) {
      size_--;
    }
  }
  Iterator Erase(ConstIterator pos) { 
    size_t index = pos - cbegin();
    std::move(items_.Get() + index + 1, items_.Get() + size_,
              items_.Get() + index);
    --size_;
    return begin() + index;
  }

  size_t GetSize() const noexcept { return size_; }

  size_t GetCapacity() const noexcept { return capacity_; }

  bool IsEmpty() const noexcept { return size_ == 0; }

  Type &At(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("std::out_of_range");
    }
    return items_[index];
  }

  const Type &At(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("std::out_of_range");
    }
    return items_[index];
  }

  void Clear() noexcept { size_ = 0; }

  void Resize(size_t new_size) {
    if (new_size <= size_) {
      size_ = new_size;
      return;
    }

    if (new_size <= capacity_) {
      for (size_t i = size_; i < new_size; ++i) {
        items_[i] = Type{};
      }
      size_ = new_size;
      return;
    }

    ArrayPtr<Type> tmp(new_size);
    for (size_t i = 0; i < size_; ++i) {
      tmp[i] = std::move_if_noexcept(items_[i]);
    }
    for (size_t i = size_; i < new_size; ++i) {
      tmp[i] = Type{};
    }

    items_.swap(tmp);
    capacity_ = new_size;
    size_ = new_size;
  }

  // ITERATORS
public:
  Iterator begin() noexcept { return items_.Get(); }
  Iterator end() noexcept { return items_.Get() + size_; }
  ConstIterator begin() const noexcept { return items_.Get(); }
  ConstIterator end() const noexcept { return items_.Get() + size_; }
  ConstIterator cbegin() const noexcept { return items_.Get(); }
  ConstIterator cend() const noexcept { return items_.Get() + size_; }
};

template <typename Type>
inline bool operator<(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs) {
  // Заглушка. Напишите тело самостоятельно
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename Type>
bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
  return lhs.GetSize() == rhs.GetSize() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  // Заглушка. Напишите тело самостоятельно
  return !(lhs == rhs);
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {

  return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs) {

  return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  // Заглушка. Напишите тело самостоятельно
  return !(lhs < rhs);
}