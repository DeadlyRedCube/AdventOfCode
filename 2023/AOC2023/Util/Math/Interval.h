#pragma once

template <typename T>
class Interval
{
public:
  Interval() = default;

  static Interval FromStartAndLength(T start, T length)
    { return { start, start + length }; }

  static Interval FromStartAndEnd(T start, T end)
    { return { start, end }; }

  static Interval FromFirstAndLast(T first, T last)
    { return { first, last + 1 }; }

  T Start() const
    { return start; }

  T End() const
    { return end; }

  T Length() const
    { return end - start; }

  T First() const
    { return start; }

  T Last() const
    { return end - 1; }

  bool Contains(T v) const
    { return v >= start && v < end; }

  bool Overlaps(Interval b) const
    { return end > b.start && start < b.end; }

  std::optional<Interval> PartBefore(Interval b) const
  {
    if (start >= b.start)
      { return std::nullopt; }

    return Interval::FromStartAndEnd(start, std::min(end, b.start));
  }

  std::optional<Interval> PartAfter(Interval b) const
  {
    if (end <= b.end)
      { return std::nullopt; }

    return Interval::FromStartAndEnd(std::max(start, b.end), end);
  }

  std::optional<Interval> Intersection(Interval b) const
  {
    if (end <= b.start || start >= b.end)
      { return std::nullopt; }

    return Interval::FromStartAndEnd(std::max(start, b.start), std::min(end, b.end));
  }

  // Only gives a union if they actually touch or overlap
  std::optional<Interval> Union(Interval b) const
  {
    if (end >= b.start && start <= b.end)
      { return Interval::FromStartAndEnd(std::min(start, b.start), std::max(end, b.end)); }

    return std::nullopt;
  }

  bool operator==(const Interval &) const = default;
  auto operator<=>(const Interval &) const = default;
protected:
  Interval(T s, T e)
    : start(s)
    , end(e)
    { }


  T start = 0;
  T end = 0;
};