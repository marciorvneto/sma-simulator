#pragma once
#include <cassert>
#include <utility>

template <typename T> class Ref {
private:
  T *m_ptr;
  int *m_refCount;

  void cleanup() {
    if (m_refCount && *m_refCount > 0) {
      --(*m_refCount);
      if (*m_refCount == 0) {
        delete m_ptr;
        delete m_refCount;
      }
    }
    m_ptr = nullptr;
    m_refCount = nullptr;
  }

  void acquire(T *ptr, int *refCount) {
    m_ptr = ptr;
    m_refCount = refCount;
    if (m_refCount) {
      ++(*m_refCount);
    }
  }

public:
  // Default constructor - creates null reference
  Ref() : m_ptr(nullptr), m_refCount(nullptr) {}

  // Constructor from raw pointer
  template <typename Derived>
  explicit Ref(Derived *ptr) : m_ptr(nullptr), m_refCount(nullptr) {
    static_assert(
        std::is_base_of_v<T, Derived> || std::is_same_v<T, Derived>,
        "Ref can only be constructed from T or classes derived from T");
    if (ptr) {
      m_ptr = ptr;
      m_refCount = new int(1);
    }
  }

  // Constructor with perfect forwarding for object creation
  template <typename... Args>
  explicit Ref(Args &&...args) : m_ptr(nullptr), m_refCount(nullptr) {
    m_ptr = new T(std::forward<Args>(args)...);
    m_refCount = new int(1);
  }

  // Copy constructor
  Ref(const Ref &other) : m_ptr(nullptr), m_refCount(nullptr) {
    acquire(other.m_ptr, other.m_refCount);
  }

  // Move constructor
  Ref(Ref &&other) noexcept : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
    other.m_ptr = nullptr;
    other.m_refCount = nullptr;
  }

  // Copy assignment
  Ref &operator=(const Ref &other) {
    if (this != &other) {
      cleanup();
      acquire(other.m_ptr, other.m_refCount);
    }
    return *this;
  }

  // Move assignment
  Ref &operator=(Ref &&other) noexcept {
    if (this != &other) {
      cleanup();
      m_ptr = other.m_ptr;
      m_refCount = other.m_refCount;
      other.m_ptr = nullptr;
      other.m_refCount = nullptr;
    }
    return *this;
  }

  // Destructor
  ~Ref() { cleanup(); }

  // Dereference operators
  T &operator*() const {
    assert(m_ptr != nullptr && "Attempting to dereference null Ref");
    return *m_ptr;
  }

  T *operator->() const {
    assert(m_ptr != nullptr && "Attempting to access null Ref");
    return m_ptr;
  }

  // Null check
  bool IsNull() const { return m_ptr == nullptr; }

  // Explicit bool conversion
  explicit operator bool() const { return m_ptr != nullptr; }

  // Get raw pointer (use with caution)
  T *get() const { return m_ptr; }

  // Get reference count (for debugging)
  int use_count() const { return m_refCount ? *m_refCount : 0; }

  // Reset to null
  void reset() { cleanup(); }

  // Comparison operators
  bool operator==(const Ref &other) const { return m_ptr == other.m_ptr; }

  bool operator!=(const Ref &other) const { return m_ptr != other.m_ptr; }

  bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }

  bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }
};
