export module Util:Containers.FixedArray;

import :Containers.ArrayView;
import :Types;

export template <typename T, ssz C = 0>
class FixedArray;


export template <typename T>
class FixedArray<T, 0> : public ArrayView<T>
{
  using Super = ArrayView<T>;

public:
  FixedArray() = default;
  FixedArray(const FixedArray &) = delete;
  FixedArray(FixedArray &&other) noexcept
    : elements(std::exchange(other.elements, nullptr))
    , count(std::exchange(other.count, 0))
    { }

  FixedArray(ssz c)
    : Super{new T[usz(c)](), c}
    { }

  FixedArray &operator=(const FixedArray &) = delete;
  FixedArray &operator=(FixedArray &&other) noexcept
  {
    delete[] elements;
    elements = std::exchange(other.elements, nullptr);
    count = std::exchange(other.count, 0);
    return *this;
  }

  ~FixedArray()
    { delete[] elements; }

private:
  using Super::elements;
  using Super::count;
};


export template <typename T, ssz C>
class FixedArray : public ArrayView<T>
{
  using Super = ArrayView<T>;


public:
  FixedArray()
    : Super(storage, C)
    { }

  ~FixedArray() = default;

  FixedArray(const FixedArray &) = delete;
  void operator=(const FixedArray &) = delete;

private:
  T storage[C];
  using Super::elements;
  using Super::count;
};