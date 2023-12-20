#pragma once


enum class ToEnd_t
{
  ToEnd,
};

using enum ToEnd_t;


struct FromEnd
{
  ssz index;
};

struct Wrapped
{
  ssz index;
};


template <typename T> concept array_view_index = std::integral<T> || std::same_as<T, FromEnd> || std::same_as<T, Wrapped>;

template <typename T>
class ArrayView
{
public:
  ArrayView() = default;
  ArrayView(T *p, ssz c)
    : elements(p)
    , count(c)
    { }

  ArrayView(ArrayView v, ssz initialIndex, ssz c)
    : elements(v.GetBuffer(initialIndex, c))
    , count(c)
    { }

  ArrayView(ArrayView v, ssz initialIndex, ToEnd_t)
    : elements(v.GetBuffer(initialIndex, v.Count() - initialIndex))
    , count(v.Count() - initialIndex)
    { }

  ArrayView(const ArrayView &) = default;
  ArrayView &operator=(const ArrayView &) = default;

  T *Elements() const
    { return elements; }

  ssz Count() const
    { return count; }

  bool IsEmpty() const
    { return count == 0; }

  ssz WrapIndex(ssz i) const
  {
    // equivalent to: return (index < 0) ? (m_count - 1 + ((index + 1) % m_count)) : index % m_count,
    constexpr size_t k_shift = std::numeric_limits<size_t>::digits - 1;
    ssz index = ssz(i);
    return ((count - 1) & (index >> k_shift)) + ((index + ssz(usz(index) >> k_shift)) % count);
  }

  T &operator[] (array_view_index auto i) const
  {
    ASSERT(IndexInRange(EvaluateIndex(i)));
    return elements[EvaluateIndex(i)];
  }

  bool IndexInRange(ssz i) const
    { return i >= 0 && i < count; }

  T *begin() const
    { return &elements[0]; }

  T *end() const
    { return &elements[count]; }

  void Fill(const T &v)
  {
    for (auto &d : *this)
      { d = v; }
  }

  ssz IndexFromElement(const T *e) const
  {
    ASSERT(e);
    ASSERT(elements);
    ssz i = ssz(e - elements);
    return IndexInRange(i) ? i : -1;
  }

  bool IsMember(const T *e) const
    { return IndexFromElement(e) >= 0; }

  T *GetBuffer(array_view_index auto i, ssz c)
  {
    ASSERT(InRangeInclusive(EvaluateIndex(i), 0, count));
    ASSERT(InRangeInclusive(EvaluateIndex(i) + c, 0, count));
    return &elements[EvaluateIndex(i)];
  }

  ssz FindFirst(const T &v) const
  {
    for (ssz i = 0; i < count; i++)
    {
      if (elements[i] == v)
        { return i; }
    }
    return -1;
  }

  template <callable_as<bool(const T &)> PredicateType>
  ssz FindFirst(PredicateType &&pred) const
  {
    for (ssz i = 0; i < count; i++)
    {
      if (pred(elements[i]))
        { return i; }
    }
    return -1;
  }

  template <typename MemberType, typename ClassType, typename ValueType>
    requires std::is_same_v<ClassType, T>
      && std::is_class_v<ClassType>
      && std::equality_comparable_with<MemberType, ValueType>
  ssz FindFirst(MemberType ClassType::*member, const ValueType &value) const
  {
    for (ssz i = 0; i < count; i++)
    {
      if (elements[i].*member == value)
        { return i; }
    }

    return -1;
  }

  ssz FindLast(const T &v) const
  {
    for (ssz i = count - 1; i >= 0; i--)
    {
      if (elements[i] == v)
        { return i; }
    }

    return -1;
  }

  template <callable_as<bool(const T &)> PredicateType>
  ssz FindLast(PredicateType &&pred) const
  {
    for (ssz i = count - 1; i >= 0; i--)
    {
      if (pred(elements[i]))
        { return i; }
    }

    return -1;
  }

  template <typename MemberType, typename ClassType, typename ValueType>
    requires std::is_same_v<ClassType, T>
      && std::is_class_v<ClassType>
      && std::equality_comparable_with<MemberType, ValueType>
  ssz FindLast(MemberType ClassType::*member, const MemberType &value) const
  {
    for (ssz i = count - 1; i >= 0; i--)
    {
      if (elements[i].*member == value)
        { return i; }
    }

    return -1;
  }


  bool Contains(const T &v) const
    { return FindFirst(v) >= 0; }

  template <callable_as<bool(const T &)> PredicateType>
  bool Contains(PredicateType &&pred) const
    { return FindFirst(pred) >= 0; }

  void Reverse()
  {
    for (ssz i = 0; i < count / 2; i++)
      { std::swap(elements[i], elements[count - 1 - i]); }
  }


  template <array_view_index I>
  ssz EvaluateIndex(I i) const
  {
    if constexpr (std::is_same_v<I, FromEnd>)
      { return count + i.index; }
    else if constexpr (std::is_same_v<I, Wrapped>)
      { return WrapIndex(i.index); }
    else
      { return ssz(i); }
  }

protected:
  T *elements = nullptr;
  ssz count = 0;
};