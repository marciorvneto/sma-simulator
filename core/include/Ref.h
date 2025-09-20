#pragma once
#include <utility>
template <typename T> class Ref {
public:
  template <typename... Args> Ref(Args &&...args) {
    m_ptr = new T(std::forward<Args>(args)...);
    m_refCount = new int(1);
  }

  template <typename Derived> Ref(Derived *ptr) {
    static_assert(std::is_base_of<T, Derived>::value,
                  "Ref constructor requires a pointer to a derived class");
    m_ptr = ptr;
    m_refCount = new int(1);
  }

  Ref(const Ref &other) {
    m_ptr = other.m_ptr;
    m_refCount = other.m_refCount;
    if (m_refCount) {
      (*m_refCount)++;
    }
  }

  Ref(Ref &&other) : m_ptr(other.m_ptr), m_refCount(other.m_refCount) {
    other.m_ptr = nullptr;
    other.m_refCount = nullptr;
  }

  Ref &operator=(const Ref &other) {
    if (this == &other) {
      return *this;
    }

    cleanup();

    m_ptr = other.m_ptr;
    m_refCount = other.m_refCount;

    if (m_refCount) {
      (*m_refCount)++;
    }

    return *this;
  }

  Ref &operator=(Ref &&other) {
    if (this == &other) {
      return *this;
    }
    m_ptr = other.m_ptr;
    m_refCount = other.m_refCount;
    other.m_refCount = nullptr;
    other.m_ptr = nullptr;
  }

  ~Ref() { cleanup(); }

  T &operator*() const { return *m_ptr; }
  T *operator->() const { return m_ptr; }

private:
  T *m_ptr = nullptr;
  int *m_refCount = nullptr;

  void cleanup() {
    if (m_refCount) {
      (*m_refCount)--;
      if (*m_refCount == 0) {
        delete m_refCount;
        delete m_ptr;
      }
    }
  }
};
