#pragma once

template <typename T>
class ResizeableArray : public ArrayView<T>
{
  using Super = ArrayView<T>;

public:
  virtual ~ResizeableArray() = default;

  void SetCount(ssz c)
  {
    if (c > count)
    {
      EnsureCapacity(c, true);
      for (ssz i = count; i < c; i++)
        { new (&elements[i]) T(); }
    }
    else
    {
      for (ssz i = c; i < count; i++)
        { elements[i].~T(); }
    }

    count = c;
  }

  void Append(const T &v)
  {
    EnsureCapacity(count + 1, false);
    new (&elements[count]) T(v);
    count++;
  }

  void Append(T &&v)
  {
    EnsureCapacity(count + 1, false);
    new (&elements[count]) T(std::move(v));
    count++;
  }

  T &AppendNew()
  {
    EnsureCapacity(count + 1, false);
    auto res = new (&elements[count]) T();
    count++;
    return *res;
  }

  void AppendMultiple(ArrayView<T> v)
  {
    EnsureCapacity(count + v.Count(), false);
    for (auto &&e : v)
    {
      new (&elements[count]) T(e);
      count++;
    }
  }

  void AppendMultiple(const std::initializer_list<T> &v)
    { AppendMultiple(ArrayView{const_cast<T *>(v.begin()), ssz(v.size())}); }

  void Insert(ssz index, const T &v)
  {
    ASSERT(InRangeInclusive(index, 0, count));
    if (index == count)
    {
      Append(v);
      return;
    }

    EnsureCapacity(count + 1, false);
    for (ssz i = count; i > index; i--)
    {
      new(&elements[i]) T(std::move(elements[i - 1]));
      elements[i - 1].~T();
    }

    new (&elements[index]) T(std::forward<T>(v));
    count++;
  }

  void Insert(ssz index, T &&v)
  {
    ASSERT(InRangeInclusive(index, 0, count));
    if (index == count)
    {
      Append(std::move(v));
      return;
    }

    EnsureCapacity(count + 1, false);
    for (ssz i = count; i > index; i--)
    {
      new(&elements[i]) T(std::move(elements[i - 1]));
      elements[i - 1].~T();
    }

    new (&elements[index]) T(std::forward<T>(v));
    count++;
  }

  void RemoveLast(ssz c = 1)
    { RemoveAt(count - c, c); }

  void RemoveAt(ssz index, ssz c = 1)
  {
    ASSERT(c >= 0);

    ASSERT(this->IndexInRange(index));
    ASSERT(InRangeInclusive(index + c, 0, count));

    for (ssz i = 0; i < c; i++)
      { elements[index + i].~T(); }

    index += c;
    for (; index < count; index++)
    {
      new (&elements[index - c]) T(std::move(elements[index]));
      elements[index].~T();
    }

    count -= c;
  }

  void RemoveFirst(const T &v)
  {
    if (auto idx = this->FindFirst(v); idx >= 0)
      { RemoveAt(idx); }
  }

  template <callable_as<bool(const T &)> PredicateType>
  void RemoveFirst(PredicateType &&pred)
  {
    if (auto idx = this->FindFirst(pred); idx >= 0)
      { RemoveAt(idx); }
  }

  void RemoveLast(const T &v)
  {
    if (auto idx = this->FindLast(v); idx >= 0)
      { RemoveAt(idx); }
  }

  template <callable_as<bool(const T &)> PredicateType>
  void RemoveLast(PredicateType &&pred)
  {
    if (auto idx = this->FindLast(pred); idx >= 0)
      { RemoveAt(idx); }
  }

  void RemoveAll(const T &v)
    { RemoveAll([&v](auto e) { return e == v; }); }

  template <callable_as<bool(const T &)> PredicateType>
  void RemoveAll(PredicateType &&pred)
  {
    ssz start = 0;
    while (true)
    {
      auto view = ArrayView { *this, start, ToEnd };
      auto index = view.FindFirst(pred);
      if (index < 0)
        { return; }

      index += start;
      RemoveAt(index);
      start = index;
    }
  }

protected:
  ResizeableArray() = default;
  ResizeableArray(T *elements, ssz count)
    : Super(elements, count)
    { }

  virtual void EnsureCapacity(ssz cap, bool preferExactSize) = 0;

  using Super::elements;
  using Super::count;
};