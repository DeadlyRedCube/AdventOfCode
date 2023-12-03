#pragma once


template <typename T>
class UnboundedArray : public ResizeableArray<T>
{
  using Super = ResizeableArray<T>;

public:
  UnboundedArray() = default;
  UnboundedArray(const UnboundedArray &other)
    { this->AppendMultiple(other); }
  UnboundedArray &operator=(const UnboundedArray &other)
  {
    this->SetCount(0);
    this->AppendMultiple(other);
    return *this;
  }

  UnboundedArray(UnboundedArray &&other) noexcept
    : Super(std::exchange(other.elements, nullptr), std::exchange(other.count, 0))
    , capacity(std::exchange(other.capacity, 0))
    { }

  UnboundedArray &operator=(UnboundedArray &&other)
  {
    for (auto &e : *this)
      { e.~T(); }

    if (elements != nullptr)
      { _aligned_free(elements); }

    elements = std::exchange(other.elements, nullptr);
    count = std::exchange(other.count, 0);
    capacity = std::exchange(other.capacity, 0);
    return *this;
  }

  ~UnboundedArray()
  {
    for (auto &e : *this)
      { e.~T(); }

    if (elements != nullptr)
      { _aligned_free(elements); }
  }

protected:
  void EnsureCapacity(ssz cap, [[maybe_unused]] bool preferExactSize) override
  {
    if (cap <= capacity)
      { return; }

    ssz newCap = preferExactSize ? cap : std::max((capacity * 2), cap);

    T *newElements = reinterpret_cast<T *>(_aligned_malloc(sizeof(T) * newCap, alignof(T)));
    if (elements != nullptr)
    {
      for (ssz i = 0; i < count; i++)
      {
        new (&newElements[i]) T(std::move(elements[i]));
        elements[i].~T();
      }

      _aligned_free(elements);
    }

    elements = newElements;
    capacity = newCap;
  }

  using Super::elements;
  using Super::count;
  ssz capacity = 0;
};