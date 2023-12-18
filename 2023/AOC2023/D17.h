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
    VisitSet(ssz width, ssz height)
      : w(width)
      , h(height)
      , visiteds((w * h * 4 + 7) / 8)
      { visiteds.Fill(0); }


    bool TestAndSet(Vec2S32 pos, Dir d)
    {
      ssz bitIndex = (pos.y * w + pos.x) * 4 + s32(d);
      ssz byteIndex = bitIndex / 8;
      u8 bitMask = u8(1 << (bitIndex & 7));
      return (std::exchange(visiteds[byteIndex], u8(visiteds[byteIndex] | bitMask)) & bitMask) != 0;
    }


  private:
    ssz w;
    ssz h;

    FixedArray<u8> visiteds;
  };


  s64 RunPart(const Array2D<char> &grid, s32 minStraightDistance, s32 maxStraightDistance)
  {
    struct Step
    {
      Vec2S32 pos;
      s64 heatLoss;
      Dir d;

      // These go in a priority queue by distance
      bool operator < (const Step &other) const
        { return heatLoss > other.heatLoss; }
    };

    constexpr Dir leftTurnFromDir[] = { Dir::W, Dir::E, Dir::N, Dir::S };
    constexpr Dir rightTurnFromDir[] = { Dir::E, Dir::W, Dir::S, Dir::N };
    constexpr Vec2S32 dirAdd[] = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };


    // Keep track of which nodes we've visited from
    VisitSet visits { grid.Width(), grid.Height() };


    std::priority_queue<Step> stepQueue;

    auto EnqueueSteps = [&](Vec2S32 pos, s64 loss, Dir d)
    {
      for (s32 i = 1; i <= maxStraightDistance; i++)
      {
        pos += dirAdd[s32(d)];
        if (!grid.PositionInRange(pos))
          { break; }

        loss += grid[pos] - '0';
        if (i >= minStraightDistance)
          { stepQueue.push({ .pos = pos, .heatLoss = loss, .d = d }); }
      }
    };

    for (Dir d : { Dir::E, Dir::S })
      { EnqueueSteps({0, 0}, 0, d); }

    while (!stepQueue.empty())
    {
      auto s = stepQueue.top();
      stepQueue.pop();

      // If we've already been here then whenever we got here last was better than this time (thanks to the priority
      //  queue)
      if (visits.TestAndSet(s.pos, s.d))
        { continue; }

      if (s.pos.x == grid.Width() - 1 && s.pos.y == grid.Height() - 1)
      {
        // If we've reached the exit with the requisite minimum straight distance we're done, otherwise, don't step
        //  more from here, it'll only be worse.
        return s.heatLoss;
      }

      for (auto d : { leftTurnFromDir[s32(s.d)], rightTurnFromDir[s32(s.d)] })
        { EnqueueSteps(s.pos, s.heatLoss, d); }
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