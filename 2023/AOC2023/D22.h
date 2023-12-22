#pragma once

namespace D22
{
  using Interval = ::Interval<s64>;
  void Run(const char *path)
  {
    struct Brick
    {
      Brick() = default;

      Brick(Vec3S64 a, Vec3S64 b)
      {
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
        // Sort by lowest Z first, then by highest Z, then by the other stuff.
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

      // Bricks intersect if all their ranges overlap
      bool Intersects(const Brick &other) const
        { return x.Overlaps(other.x) && y.Overlaps(other.y) && z.Overlaps(other.z); }
    };

    // Read and then sort the bricks
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

    // Fall all the bricks into place from bottom to top.
    s64 maxPrevZ = 1;
    for (s32 i = 0 ; i < bricks.Count(); i++)
    {
      auto &cur = bricks[i];

      // Slam this brick down as far as we can until it's about to overlap in XY planes where other bricks are.
      cur.z = Interval::FromStartAndLength(std::min(cur.z.First(), maxPrevZ + 1), cur.z.Length());
      while (cur.z.First() > 0)
      {
        // Try moving it down one (but save the old Z in case we need to restore it)
        auto oldZ = cur.z;
        cur.z = Interval::FromStartAndLength(cur.z.Start() - 1, cur.z.Length());

        bool overlaps = false;
        for (s32 j = 0; j < i; j++)
        {
          if (cur.Intersects(bricks[j]))
          {
            // These bricks overlap
            overlaps = true;
            break;
          }
        }

        if (overlaps)
        {
          // Was overlapped, so it could not fall anymore - restore the Z before we tried to fall it.
          cur.z = oldZ;
          break;
        }
      }

      // Update our maximum Z (so that we can fast-fall bricks to just above the highest known current Z)
      maxPrevZ = std::max(maxPrevZ, cur.z.Last());
    }

    std::ranges::sort(bricks);

    // Now build a graph of which bricks support which otehr bricks.
    struct BrickGraph
    {
      Brick b;
      UnboundedArray<ssz> supporting;
      UnboundedArray<ssz> supportedBy;
    };

    FixedArray<BrickGraph> brickGraph { bricks.Count() };
    for (ssz i = 0; i < bricks.Count(); i++)
      { brickGraph[i].b = bricks[i]; }

    for (ssz i = 0; i < brickGraph.Count(); i++)
    {
      auto &high = brickGraph[i];

      // If this brick is at the bottom we can't fall any further.
      if (high.b.z.First() == 0)
        { continue; }

      // Try to lower it and see if it intersects any bricks when dropped one.
      auto lowered = high.b;
      lowered.z = Interval::FromStartAndLength(lowered.z.First() - 1, lowered.z.Length());
      for (ssz j = 0; j < i; j++)
      {
        auto &low = brickGraph[j];
        if (low.b.Intersects(lowered))
        {
          // It intersected a brick, which means it is supported by that brick (and that brick is supporting this one)
          low.supporting.Append(i);
          high.supportedBy.Append(j);
        }
      }
    }

    // Do the part 1 count
    s64 disCount = 0;
    for (ssz i = 0; i < brickGraph.Count(); i++)
    {
      auto &b = brickGraph[i];
      bool canRemove = true;
      for (auto &si : b.supporting)
      {
        auto &s =  brickGraph[si];
        if (s.supportedBy.Count() == 1)
        {
          // Can't remove a brick that supports any brick that is only supported by it.
          canRemove = false;
          break;
        }
      }

      if (canRemove)
        { disCount++; }
    }

    PrintFmt("Part 1: {}\n", disCount);

    s64 p2Sum = 0;

    // We'll keep track of which bricks have or have not fallen in this array.
    FixedArray<bool> fell { brickGraph.Count() };

    for (ssz i = 0; i < brickGraph.Count(); i++)
    {
      fell.Fill(false);

      std::queue<ssz> fallQueue;
      fallQueue.push(i);

      while (!fallQueue.empty())
      {
        // Pull our index from the queue and mark it as having fallen.
        auto fi = fallQueue.front();
        fallQueue.pop();
        fell[fi] = true;

        for (auto sup : brickGraph[fi].supporting)
        {
          ASSERT(!fell[sup]);

          // Check to see if any of the supports of this element are still unfallen
          bool unsupported = true;
          for (auto sb : brickGraph[sup].supportedBy)
          {
            if (!fell[sb])
            {
              // Yep, at least one support has not fallen so this one cannot fall (yet).
              unsupported = false;
              break;
            }
          }

          if (unsupported)
          {
            // This one is gonna fall, add it to the queue (to see how it cascades) and add it to the count!
            fallQueue.push(sup);
            p2Sum++;
          }
        }
      }
    }

    PrintFmt("Part 2: {}\n", p2Sum);
  }
}
