#include <cassert>
#include <cstdlib>
#include <iostream>

template <typename Type> class ArrayPtr {
  // constructors and destructor
public:
  ArrayPtr() = default;

  explicit ArrayPtr(size_t size) {
    if (size > 0) {
      raw_ptr_ = new Type[size]{};
    }
  }
  explicit ArrayPtr(Type *raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

  // move constructor
  ArrayPtr(ArrayPtr &&other) noexcept : raw_ptr_(other.Release()) {}

  ArrayPtr(const ArrayPtr &) = delete;

  ~ArrayPtr() { delete[] raw_ptr_; }

  // operators
public:
  ArrayPtr &operator=(const ArrayPtr &) = delete;

  ArrayPtr &operator=(ArrayPtr &&rhs) noexcept {
    if (this != &rhs) {
      delete[] raw_ptr_;
      raw_ptr_ = rhs.Release();
    }
    return *this;
  }

  Type &operator[](size_t index) noexcept { return raw_ptr_[index]; }

  const Type &operator[](size_t index) const noexcept {
    return raw_ptr_[index];
  }

  explicit operator bool() const { return static_cast<bool>(raw_ptr_); }

  // Methods
public:
  [[nodiscard]] Type *Release() noexcept {
    Type *tmp = raw_ptr_;
    raw_ptr_ = nullptr;
    return tmp;
  }

  Type *Get() const noexcept { return raw_ptr_; }

  void swap(ArrayPtr &other) noexcept {
    Type *tmp = other.raw_ptr_;
    other.raw_ptr_ = raw_ptr_;
    raw_ptr_ = tmp;
  }

private:
  Type *raw_ptr_ = nullptr;
};
