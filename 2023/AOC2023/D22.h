#pragma once

namespace D22
{
  using Interval = ::Interval<s64>;
  void Run(const char *path)
  {
    struct Brick
    {
      Brick (Vec3S64 a, Vec3S64 b)
      {
        ASSERT((a.x == b.x && a.y == b.y)
          || (a.x == b.x && a.z == b.z)
          || (a.y == b.y && a.z == b.z));
        x = Interval::FromFirstAndLast(std::min(a.x, b.x), std::max(a.x, b.x));
        y = Interval::FromFirstAndLast(std::min(a.y, b.y), std::max(a.y, b.y));
        z = Interval::FromFirstAndLast(std::min(a.z, b.z), std::max(a.z, b.z));
      }

      Interval x;
      Interval y;
      Interval z;

      bool operator== (const Brick &other) const = default;

      auto operator <=> (const Brick &other) const
      {
        if (auto r = z.First() <=> other.z.First(); r != std::strong_ordering::equal)
          { return r; }

        if (auto r = z.Last() <=> other.z.Last(); r != std::strong_ordering::equal)
          { return r; }

        if (auto r = x.First() <=> other.x.First(); r != std::strong_ordering::equal)
          { return r; }

        if (auto r = y.First() <=> other.y.First(); r != std::strong_ordering::equal)
          { return r; }

        if (auto r = x.Last()  <=> other.x.Last(); r != std::strong_ordering::equal)
          { return r; }

        return y.Last() <=> other.y.Last();
      }

      bool Intersects(const Brick &other)
        { return x.Overlaps(other.x) && y.Overlaps(other.y) && z.Overlaps(other.z); }
    };

    UnboundedArray<Brick> bricks;
    for (auto line : ReadFileLines(path))
    {
      auto splits = Split(line, ",~ ", KeepEmpty::No);
      bricks.Append(
        {
          { StrToNum(splits[0]), StrToNum(splits[1]), StrToNum(splits[2]) },
          { StrToNum(splits[3]), StrToNum(splits[4]), StrToNum(splits[5]) },
        });
    }

    std::ranges::sort(bricks);

    for (ssz i = 0; i < bricks.Count(); i++)
    {
      for (ssz j = i + 1; j < bricks.Count(); j++)
      {
        ASSERT(!bricks[i].Intersects(bricks[j]));
      }
    }

    for (s32 i = 0 ; i < bricks.Count(); i++)
    {
      auto &cur = bricks[i];

      while (cur.z.First() > 0)
      {
        auto oldZ = cur.z;
        cur.z = Interval::FromStartAndLength(cur.z.Start() - 1, cur.z.Length());

        bool overlaps = false;
        for (s32 j = 0; j < i; j++)
        {
          if (cur.Intersects(bricks[j]))
          {
            overlaps = true;
            break;
          }
        }

        if (overlaps)
        {
          cur.z = oldZ;
          break;
        }
      }
    }

    std::ranges::sort(bricks);

    ssz  disCount = 0;
    for (s32 i = 0; i < bricks.Count(); i++)
    {
      // Check if this can be disintigrated
      bool anyCouldFall = false;
      for (s32 j = i + 1; j < bricks.Count(); j++)
      {
        if (bricks[j].z.First() == 0)
          { continue; }

        if (bricks[j].z.First() > bricks[i].z.Last() + 1)
          { break; }

        Brick b = bricks[j];
        b.z = Interval::FromStartAndLength(b.z.First() - 1, b.z.Length());
        bool canFall = true;
        for (s32 k = 0; k < j; k++)
        {
          if (k == i)
            { continue; }
          if (b.Intersects(bricks[k]))
          {
            canFall = false;
            break;
          }
        }

        if (canFall)
        {
          anyCouldFall = true;
          break;
        }
      }

      if (!anyCouldFall)
      {
        disCount++;
      }
    }

    PrintFmt("Part 1: {}\n", disCount);
  }
}