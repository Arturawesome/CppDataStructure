#pragma once
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

template <typename Type> class SingleLinkedList {
  struct Node {
    Node() = default;
    Node(const Type &val, Node *next) : value(val), next_node(next) {}
    Type value;
    Node *next_node = nullptr;
  };

  template <typename ValueType> class BasicIterator {
    template <typename> friend class BasicIterator;

  private:
    Node *node_ = nullptr;
    friend class SingleLinkedList;
    explicit BasicIterator(Node *node) : node_(node) {}

  public:
    // Задаёт категорию итератора.
    using iterator_category = std::forward_iterator_tag;
    // Задаёт тип элементов, доступ к которым предоставляет итератор
    using value_type = Type;
    // Задаёт тип для хранения смещения между двумя итераторами.
    using difference_type = std::ptrdiff_t;
    // Задаёт тип указателя, возвращаемого операцией ->
    using pointer = ValueType *;
    // Задаёт тип ссылки, которую возвращает операция разыменования *.
    using reference = ValueType &;

    BasicIterator() = default;
    BasicIterator(const BasicIterator<Type> &other) noexcept
        : node_(other.node_) {}

    BasicIterator &operator=(const BasicIterator &rhs) = default;

    [[nodiscard]] bool
    operator==(const BasicIterator<const Type> &rhs) const noexcept {
      return node_ == rhs.node_;
    }
    [[nodiscard]] bool
    operator!=(const BasicIterator<const Type> &rhs) const noexcept {
      return !(*this == rhs);
    }

    [[nodiscard]] bool
    operator==(const BasicIterator<Type> &rhs) const noexcept {
      return node_ == rhs.node_;
    }
    [[nodiscard]] bool
    operator!=(const BasicIterator<Type> &rhs) const noexcept {
      return !(*this == rhs);
    }
    BasicIterator &operator++() noexcept {
      assert(node_ != nullptr);
      node_ = node_->next_node;
      return *this;
    }

    BasicIterator operator++(int) noexcept {
      assert(node_ != nullptr);
      auto oldValue(*this);
      ++(*this);
      return oldValue;
    }

    [[nodiscard]] reference operator*() const noexcept {
      assert(node_ != nullptr);
      return node_->value;
    }

    [[nodiscard]] pointer operator&() const noexcept { return node_; }
    pointer operator->() const noexcept {
      assert(node_ != nullptr);
      return &node_->value;
    }
  };

  // ===============================================
  // ========ITERATORS==============================
  // ===============================================
public:
  using Iterator = BasicIterator<Type>;
  using ConstIterator = BasicIterator<const Type>;

  [[nodiscard]] Iterator begin() noexcept { return Iterator(head_.next_node); }
  [[nodiscard]] ConstIterator begin() const noexcept {
    return ConstIterator(head_.next_node);
  }
  [[nodiscard]] Iterator before_begin() noexcept { return Iterator(&head_); }
  [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
    return ConstIterator(const_cast<Node *>(&head_));
  }
  [[nodiscard]] ConstIterator before_begin() const noexcept {
    return ConstIterator(&head_);
  }
  [[nodiscard]] Iterator end() noexcept { return Iterator(nullptr); }
  [[nodiscard]] ConstIterator end() const noexcept {
    return ConstIterator(nullptr);
  }
  [[nodiscard]] ConstIterator cbegin() const noexcept { return this->begin(); }
  [[nodiscard]] ConstIterator cend() const noexcept { return this->end(); }

  // ===============================================
  // ========METHODS================================
  // ===============================================
public:
  SingleLinkedList() { size_ = 0; }
  SingleLinkedList(std::initializer_list<Type> values) {
    auto it = values.end();
    while (it != values.begin()) {
      --it;
      PushFront(*it);
    }
  }
  SingleLinkedList(const SingleLinkedList &other) {
    assert(size_ == 0 && head_.next_node == nullptr);
    Node *tail = &head_;

    for (const auto &value : other) {
      tail->next_node = new Node(value, nullptr);
      tail = tail->next_node;
      ++size_;
    };
  }
  void swap(SingleLinkedList &other) noexcept {
    std::swap(head_.next_node, other.head_.next_node);
    std::swap(size_, other.size_);
  }

  SingleLinkedList &operator=(const SingleLinkedList &rhs) {
    SingleLinkedList tmp = SingleLinkedList(rhs);
    (*this).swap(tmp);
    return *this;
  }

  void PushFront(const Type &value) {
    head_.next_node = new Node(value, head_.next_node);
    size_++;
  }
  void Clear() noexcept {
    while (head_.next_node != nullptr) {
      Node *curr = head_.next_node;
      head_.next_node = curr->next_node;
      delete curr;
    }
    size_ = 0;
  }
  Iterator InsertAfter(ConstIterator pos, const Type &value) {
    assert(pos.node_ != nullptr);
    Node *curr = pos.node_;
    Node *newNode = new Node(value, curr->next_node);
    curr->next_node = newNode;
    ++size_;
    return Iterator(newNode);
  }
  void PopFront() noexcept {
    assert(head_.next_node != nullptr);

    Node *newNext = head_.next_node->next_node;
    delete head_.next_node;
    head_.next_node = newNext;
    size_--;
  }
  Iterator EraseAfter(ConstIterator pos) noexcept {
    assert(pos.node_ != nullptr);
    assert(pos.node_->next_node != nullptr);

    Node *delNode = pos.node_->next_node;
    pos.node_->next_node = delNode->next_node;
    delete delNode;
    size_--;
    return Iterator(pos.node_->next_node);
  }
  [[nodiscard]] size_t GetSize() const noexcept { return size_; }
  [[nodiscard]] bool IsEmpty() const noexcept {
    if (head_.next_node == nullptr) {
      return true;
    }
    return false;
  }
  ~SingleLinkedList() { Clear(); }

private:
  Node head_;
  size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type> &lhs, SingleLinkedList<Type> &rhs) noexcept {
  lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
  return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type> &lhs,
               const SingleLinkedList<Type> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
  return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type> &lhs,
               const SingleLinkedList<Type> &rhs) {
  return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type> &lhs,
                const SingleLinkedList<Type> &rhs) {
  return !(lhs < rhs);
}
