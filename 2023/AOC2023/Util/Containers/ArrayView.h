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
    { return ((i % count) + count) % count; }

  T &operator[] (ssz i) const
  {
    ASSERT(IndexInRange(i));
    return elements[i];
  }

  T &operator[] (FromEnd f) const
    { return (*this)[count + f.index]; }

  T &operator[] (Wrapped w) const
    { return (*this)[WrapIndex(w.index)]; }

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

  T *GetBuffer(ssz i, ssz c)
  {
    ASSERT(InRangeInclusive(i, 0, count));
    ASSERT(InRangeInclusive(i + c, 0, count));
    return &elements[i];
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

protected:
  T *elements = nullptr;
  ssz count = 0;
};