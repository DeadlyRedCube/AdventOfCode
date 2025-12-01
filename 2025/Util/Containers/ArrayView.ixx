module;

#include <cassert>

export module Util:Containers.ArrayView;

import std;

import :Types;

export enum class ToEnd_t
{
  ToEnd,
};

export using enum ToEnd_t;


export struct FromEnd
{
  ssz index;
};

export struct Wrapped
{
  ssz index;
};


export template <typename T> concept array_view_index = std::integral<T> || std::same_as<T, FromEnd> || std::same_as<T, Wrapped>;

export template <typename T>
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
    { return WrapIndex(i, ssz(count)); }

  T &operator[] (array_view_index auto i) const
  {
    assert(IndexInRange(EvaluateIndex(i)));
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
    assert(e);
    assert(elements);
    ssz i = ssz(e - elements);
    return IndexInRange(i) ? i : -1;
  }

  bool IsMember(const T *e) const
    { return IndexFromElement(e) >= 0; }

  T *GetBuffer(array_view_index auto i, [[maybe_unused]] ssz c)
  {
    assert(InRangeInclusive(EvaluateIndex(i), 0, count));
    assert(InRangeInclusive(EvaluateIndex(i) + c, 0, count));
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