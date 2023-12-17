#pragma once

namespace D17
{
  enum class Dir
  {
    N,
    S,
    E,
    W
  };


  // Helper to store information about whether we've visited a given position with a given travel direction and
  //  distance. Cuts 1.4 seconds off of the runtime vs. just using a std::set with an equivalent Visit struct.
  class VisitSet
  {
  public:
    VisitSet(ssz width, ssz height, s32 maxStraightDistance)
      : w(width)
      , h(height)
      , msd(maxStraightDistance)
      , visiteds((w * h * 4 * maxStraightDistance + 7) / 8)
      { visiteds.Fill(0); }


    bool TestAndSet(Vec2S32 pos, Dir d, s32 currentStraightLength)
    {
      ssz bitIndex = ((pos.y * w + pos.x) * 4 + s32(d)) * msd + currentStraightLength;
      ssz byteIndex = bitIndex / 8;
      u8 bitMask = u8(1 << (bitIndex & 7));
      return (std::exchange(visiteds[byteIndex], visiteds[byteIndex] | bitMask) & bitMask) != 0;
    }


  private:
    ssz w;
    ssz h;
    s32 msd;

    FixedArray<u8> visiteds;
  };


  s64 RunPart(const Array2D<char> &grid, s32 minStraightDistance, s32 maxStraightDistance)
  {
    struct Step
    {
      Vec2S32 pos;
      s64 heatLoss;
      Dir d;
      s32 currentStraightLength;

      // These go in a priority queue by distance
      bool operator < (const Step &other) const
        { return heatLoss > other.heatLoss; }
    };

    constexpr Dir leftTurnFromDir[] = { Dir::W, Dir::E, Dir::N, Dir::S };
    constexpr Dir rightTurnFromDir[] = { Dir::E, Dir::W, Dir::S, Dir::N };
    constexpr Vec2S32 dirAdd[] = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };


    // Keep track of which nodes we've visited from
    VisitSet visits { grid.Width(), grid.Height(), maxStraightDistance };

    std::priority_queue<Step> stepQueue;
    stepQueue.push({ .pos = { 0, 0 }, .heatLoss = 0, .d = Dir::E, .currentStraightLength = 0});
    stepQueue.push({ .pos = { 0, 0 }, .heatLoss = 0, .d = Dir::S, .currentStraightLength = 0});

    while (!stepQueue.empty())
    {
      auto s = stepQueue.top();
      stepQueue.pop();

      // If we've already been here then whenever we got here last was better than this time (thanks to the priority
      //  queue)
      if (visits.TestAndSet(s.pos, s.d, s.currentStraightLength))
        { continue; }

      if (s.pos.x == grid.Width() - 1 && s.pos.y == grid.Height() - 1)
      {
        // If we've reached the exit with the requisite minimum straight distance we're done, otherwise, don't step
        //  more from here, it'll only be worse.
        if (s.currentStraightLength >= minStraightDistance)
          { return s.heatLoss; }
        continue;
      }

      // Only can move forward if we have enough forward moves
      if (s.currentStraightLength < maxStraightDistance)
      {
        auto d = s.d;
        auto p = s.pos + dirAdd[s32(d)];
        if (grid.PositionInRange(p))
        {
          auto hl = s.heatLoss + (grid[p] - '0');
          stepQueue.push({ .pos = p, .heatLoss = hl, .d = d, .currentStraightLength = s.currentStraightLength + 1 });
        }
      }

      if (s.currentStraightLength >= minStraightDistance)
      {
        // We have moved enough that we are allowed to turn

        {
          auto d = leftTurnFromDir[s32(s.d)];
          auto p = s.pos + dirAdd[s32(d)];
          if (grid.PositionInRange(p))
          {
            auto hl = s.heatLoss + (grid[p] - '0');
            stepQueue.push({ .pos = p, .heatLoss = hl, .d = d, .currentStraightLength = 1 });
          }
        }

        {
          auto d = rightTurnFromDir[s32(s.d)];
          auto p = s.pos + dirAdd[s32(d)];
          if (grid.PositionInRange(p))
          {
            auto hl = s.heatLoss + (grid[p] - '0');
            stepQueue.push({ .pos = p, .heatLoss = hl, .d = d, .currentStraightLength = 1 });
          }
        }
      }
    }

    ASSERT(false);
    return -1;
  }


  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    PrintFmt("Part 1: {}\n", RunPart(grid, 0, 3));
    PrintFmt("Part 2: {}\n", RunPart(grid, 4, 10));
  }
}