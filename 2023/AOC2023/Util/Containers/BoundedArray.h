#pragma once

#include <cstdlib>

template <typename T, ssz Cap = 0>
class BoundedArray;


template <typename T>
class BoundedArray<T, 0> : public ResizeableArray<T>
{
  using Super = ResizeableArray<T>;

public:
  BoundedArray() = default;
  BoundedArray(const BoundedArray &) = delete;
  BoundedArray(BoundedArray &&other) noexcept
    : Super(std::exchange(other.elements, nullptr), std::exchange(other.count, 0))
    , capacity(std::exchange(other.capacity, 0))
    { }

  BoundedArray(ssz c)
    : Super(reinterpret_cast<T *>(_aligned_malloc(c * sizeof(T), alignof(T))), 0)
    , capacity(c)
    { }

  BoundedArray &operator=(const BoundedArray &) = delete;
  BoundedArray &operator=(BoundedArray &&other) noexcept
  {
    if (elements != nullptr)
      { _aligned_free(elements); }

    elements = std::exchange(other.elements, nullptr);
    count = std::exchange(other.count, 0);
    capacity = std::exchange(other.capacity, 0);
    return *this;
  }

  ~BoundedArray()
  {
    for (auto &e : *this)
      { e.~T(); }

    if (elements != nullptr)
      { _aligned_free(elements); }
  }

  ssz Capacity() const
    { return capacity; }

protected:
  void EnsureCapacity(ssz cap, [[maybe_unused]] bool preferExactSize) override
  {
    ASSERT(cap <= capacity);
  }

  using Super::elements;
  using Super::count;
  ssz capacity = 0;
};



template <typename T, ssz Cap>
class BoundedArray : public ResizeableArray<T>
{
  using Super = ResizeableArray<T>;

public:
  BoundedArray()
    : Super(reinterpret_cast<T *>(storage), 0)
    { }

  BoundedArray(const BoundedArray &) = delete;
  BoundedArray &operator=(const BoundedArray &) = delete;

  constexpr ssz Capacity() const
    { return Cap; }

  ~BoundedArray()
  {
    for (auto &e : *this)
      { e.~T(); }
  }

protected:
  void EnsureCapacity([[maybe_unused]] ssz cap, [[maybe_unused]] bool preferExactSize) override
  {
    ASSERT(cap <= Cap);
  }

  using Super::elements;
  using Super::count;
  alignas(alignof(T)) u8 storage[sizeof(T) * Cap];
};