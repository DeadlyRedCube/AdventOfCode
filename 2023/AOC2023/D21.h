#pragma once

namespace D21
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    Vec2S64 sPosition;
    bool foundS = false;
    for (ssz y = 0; y < grid.Height(); y++)
    {
      for (ssz x = 0; x < grid.Height(); x++)
      {
        if (grid[{x, y}] == 'S')
        {
          sPosition = {x, y};
          foundS = true;
          break;
        }
      }

      if (foundS)
        { break; }
    }

    // Now trace the distances through the graph
    enum class SpaceType
    {
      Unreached,
      Even,
      Odd,
    };

    struct Step
    {
      Vec2S64 pos;
      s64 step = 0;

      bool operator < (const Step &b) const
        { return step > b.step; }
    };

    Vec2S64 dirs[] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

    struct Found
    {
      Vec2S64 pos;
      SpaceType type;

      bool operator==(const Found &) const = default;
      auto operator<=>(const Found &) const = default;
    };


    {
      std::queue<Step> stepQueue;
      stepQueue.push({ .pos = sPosition, .step = 0 });

      struct Found
      {
        Vec2S64 pos;
        SpaceType type;

        bool operator==(const Found &) const = default;
        auto operator<=>(const Found &) const = default;
      };

      std::set<Found> foundSteps;

      while (!stepQueue.empty())
      {
        auto step = stepQueue.front();
        stepQueue.pop();

        auto type = (step.step & 1) ? SpaceType::Odd : SpaceType::Even;

        if (!foundSteps.insert({ .pos = step.pos, .type = type }).second)
          { continue; }

        if (step.step >= 64)
          { continue; }

        for (auto dir : dirs)
        {
          if (grid.PositionInRange(step.pos + dir) && grid[step.pos + dir] != '#')
            { stepQueue.push({ step.pos + dir, step.step + 1 }); }
        }
      }

      // Count the evens
      s64 count = 0;
      for (auto &&f : foundSteps)
        { if (f.type == SpaceType::Even) { count++; } }

      PrintFmt("Part 1: {}\n", count);
    }

    const s64 targetStepCount = 26501365;

    // Alright this all sucks but it got the right answer.
    // I discovered that out past a certain point (a 3x3 grid in the real data, but I needed more in the sample),
    //  the distance to any given grid square increases by the manhattan distance to that tile (likely due to the
    //  straightline channels around each grid square).
    s32 gridSizeMultiplier = 3;

    // For the sample I needed a larger grid to get the edges right
    while (gridSizeMultiplier * grid.Width() < 77)
      { gridSizeMultiplier += 2; }

    ssz halfGridSizeMultiplier = gridSizeMultiplier / 2;
    Vec2S64 gridCenterOffset = { grid.Width() * halfGridSizeMultiplier, grid.Height() * halfGridSizeMultiplier };

    Array2D<s64> distances = { grid.Width() * gridSizeMultiplier, grid.Height() * gridSizeMultiplier };
    distances.Fill(std::numeric_limits<s64>::max());

    // Generate the distances for our embiggened grid
    {
      auto sBigPosition = sPosition + gridCenterOffset;
      std::priority_queue<Step> stepQueue;
      stepQueue.push({ sBigPosition, 0 });

      while(!stepQueue.empty())
      {
        auto step = stepQueue.top();
        stepQueue.pop();

        if (distances[step.pos] != std::numeric_limits<s64>::max())
          { continue; }

        distances[step.pos] = step.step;

        for (auto dir : dirs)
        {
          if (distances.PositionInRange(step.pos + dir) &&  grid[grid.Wrap(step.pos + dir)] != '#')
            { stepQueue.push({ step.pos + dir, step.step + 1 }); }
        }
      }
    }

    // Now we want to get a list of the (reachable!) evens/odds within a square (so we can add entire tile counts
    //  wholesale through the middle where we know all the distances are in range)
    s64 centerOnParityCount = 0;
    s64 centerOffParityCount = 0;
    for (ssz y = 0; y < grid.Height(); y++)
    {
      for (ssz x = 0; x < grid.Width(); x++)
      {
        auto dist = distances[{x + halfGridSizeMultiplier * grid.Width(), y + halfGridSizeMultiplier * grid.Height() }];
        if (dist == std::numeric_limits<s64>::max())
          { continue; }

        if ((dist & 1) == (targetStepCount & 1))
          { centerOnParityCount++; }
        else
          { centerOffParityCount++; }
      }
    }

    s64 count = 0;

    // Below a tile-index manhattan distance of this distance we know that the entire grid is in range and we can
    //  skip doing an actual check against it.
    ssz minTileExtent = targetStepCount / grid.Width() - 1;

    // Above this distance, the entire tile is guaranteed to be out of range.
    ssz maxTileExtent = (targetStepCount + grid.Width() - 1) / grid.Width();

    for (ssz ty = -maxTileExtent; ty <= maxTileExtent; ty++)
    {
      // Figure out the x extents to that we need to test (Basically testing a diamond around the whole thing)
      ssz xMinExtent = std::max(ssz(0), minTileExtent - std::abs(ty));
      ssz xMaxExtent = maxTileExtent - std::abs(ty);

      if (xMinExtent > 0)
      {
        // Inside of this extent it's full squares only!
        count += (xMinExtent - 1) * (centerOffParityCount + centerOnParityCount);
        count += centerOnParityCount;
      }

      // Time to tally up the ones we actually need to examine closely
      for (ssz absTx = xMinExtent; absTx <= xMaxExtent; absTx++)
      {
        // Test both positive and negative versions of our x (only do one if we're at 0)
        ssz xArray[] = { -absTx, absTx };
        ArrayView<ssz> xList = (absTx == 0) ? ArrayView<ssz>(xArray, 1) : ArrayView<ssz>(xArray, 2);

        for (ssz tx : xList)
        {
          // Get the x, y coordinate of the tile we're going to actually investigate
          ssz tileX = std::clamp(tx + halfGridSizeMultiplier, ssz(0), ssz(gridSizeMultiplier - 1));
          ssz tileY = std::clamp(ty + halfGridSizeMultiplier, ssz(0), ssz(gridSizeMultiplier - 1));

          // The full manhattan distance to this tile from the tile we're testing against  (the baseline distance
          //  we're going to use)
          ssz tileDistance = std::abs(tx + halfGridSizeMultiplier - tileX) * grid.Width()
            + std::abs(ty + halfGridSizeMultiplier - tileY) * grid.Height();

          for (ssz y = 0; y < grid.Height(); y++)
          {
            for (ssz x = 0; x < grid.Width(); x++)
            {
              auto dist = distances[{x + tileX * grid.Width(), y + tileY * grid.Height() }];

              // ignore unreachable spaces
              if (dist == std::numeric_limits<s64>::max())
                { continue; }

              dist += tileDistance;
              if (dist <= targetStepCount && (dist & 1) == (targetStepCount & 1))
                { count++; }
            }
          }
        }
      }
    }

    PrintFmt("Part 2:{}\n", count);
  }
}