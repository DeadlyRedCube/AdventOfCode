#pragma once


namespace D16
{
  using Vec = Vec2S32;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();


  struct TraceResult
  {
    Vec ending;
    Vec endFacing;
    s32 cost;
    s32 stepCount;
  };

  std::optional<TraceResult>TracePath(const Array2D<char> &grid, Vec start, Vec dir)
  {
    auto cur = start + dir;

    s32 stepCount = 0;
    s32 cost = 1;
    while (true)
    {
      if (grid[cur] == 'E' || grid[cur] == 'S') // remove S test
      {
        // We actually reached the end so we're done.
        return TraceResult{ cur, dir, cost, stepCount };
      }

      s32 exitCount = 0;
      for (auto testDir : { X, -X, Y, -Y })
        { exitCount += (grid[cur + testDir] != '#') ? 1 : 0; }

      if (exitCount == 1)
        { return std::nullopt; }

      // This is an intersection! Path tracing is done.
      if (exitCount > 2)
        { return TraceResult{ cur, dir, cost, stepCount }; }

      for (auto testDir : { dir, dir.RotateLeft(), dir.RotateRight() })
      {
        auto next = cur + testDir;
        if (grid[next] != '#')
        {
          stepCount++;
          cost += 1 + ((dir == testDir) ? 0 : 1000);
          cur = next;
          dir = testDir;
          break;
        }
      }
    }
  }


  void Run(const char *filePath)
  {
    s32 p1 = 0;
    s32 p2 = 0;

    auto grid = ReadFileAsCharArray2D(filePath);
    auto pathGrid = Array2D<s32> { grid.Width(), grid.Height() };

    Vec startPos;
    for (s32 y = s32(grid.Height()) - 1; y >= 0; y--)
    {
      for (s32 x = 0; x < s32(grid.Width()); x++)
      {
        if (grid[x, y] == 'S')
        {
          startPos = {x, y};
          break;
        }
      }
    }

    struct EnterCost
    {
      Vec srcFacing;
      s32 totalCost;
    };

    struct Path
    {
      bool committed = false;
      Vec in;
      Vec inDir;
      Vec out;
      Vec outDir;
      s32 stepCount;
      s32 cost;
      BoundedArray<EnterCost, 4> enterCosts;
    };

    std::vector<Path> paths;
    static constexpr s32 Unpathed = 0;
    paths.emplace_back(); // Put a dummy value at 0 because 0 in the path grid means "not connected yet"

    paths.reserve(grid.Width() * grid.Height() / 8); // Just a guess as to a good initial size to avoid reallocation

    struct WalkEntry
    {
      Vec pos;
      Vec facing;
      Vec prePathFacing;
      s32 postPathCost = 0;
    };

    auto CmpWalk = [](const WalkEntry &a, const WalkEntry &b)
      { return a.postPathCost > b.postPathCost; };
    std::priority_queue<WalkEntry, std::vector<WalkEntry>, decltype(CmpWalk)> traceQueue;

    traceQueue.push({ .pos = startPos, .facing = X });

    Vec endPos;
    p1 = std::numeric_limits<s32>::max();

    while (!traceQueue.empty())
    {
      auto walk = traceQueue.top();
      traceQueue.pop();

      // If we've already walked farther than the shortest path to the exit we've found we can stop now.
      if (walk.postPathCost > p1)
        { continue; }

      if (auto pathIndex = pathGrid[walk.pos - walk.facing]; pathIndex != Unpathed)
      {
        auto &path = paths[pathIndex];

        if (!path.committed)
        {
          // We have to walk the shortest walk along a path before comitting it (otherwise we can commit going
          //  upstream), but we've now gotten here so

          ASSERT(path.enterCosts.IsEmpty());
          path.committed = true;

          // We're going to commit this path now as this is the first time we've actually crossed it to this distance.
          if (path.out != walk.pos)
          {
            // We first saw this from the other direction but we're committing this way
            ASSERT(path.in == walk.pos);
            std::swap(path.in, path.out);
            std::swap(path.inDir, path.outDir);
            path.inDir = -path.inDir;
            path.outDir = -path.outDir;
          }
        }

        if (path.out != walk.pos)
          { continue; } // We've since committed this path the other direction so don't continue.

        // Now that we've walked it, we can commit the enter costs.
        auto found = std::ranges::find(path.enterCosts, walk.prePathFacing, &EnterCost::srcFacing);

        // Only add this facing if there is not already one (which would have a lower cost than this one)
        if (found != path.enterCosts.end())
          { ASSERT(found->totalCost <= walk.postPathCost); continue; }
        else
          { path.enterCosts.Append({ .srcFacing = walk.prePathFacing, .totalCost = walk.postPathCost }); }
      }

      if (grid[walk.pos] == 'E')
      {
        // Reached the end! Since we're doing breadth first we can record this as the shortest cost, we'll never
        //  get here with a shorter path again (but we may get here with a same-length path again)
        p1 = walk.postPathCost;
        endPos = walk.pos;
        continue;
      }

      // Now test every direction except the one we entered from (can't 180)
      for (auto &dir : { walk.facing, walk.facing.RotateLeft(), walk.facing.RotateRight() })
      {
        auto next = walk.pos + dir;

        // If it's not an open space skip it.
        if (grid[next] != '.')
          { continue; }

        if (pathGrid[next] == Unpathed)
        {
          // We haven't traced this path yet so do so now.
          if (auto result = TracePath(grid, walk.pos, dir); result.has_value())
          {
            // The trace was successful, so throw the path into the grid so that we can find it next time.
            auto [target, targetFacing, pathCost, pathStepCount] = *result;

            pathGrid[next] = s32(paths.size());
            pathGrid[target - targetFacing] = s32(paths.size());

            paths.push_back(
              {
                .in = walk.pos,
                .inDir = dir,
                .out = target,
                .outDir = targetFacing,
                .stepCount = pathStepCount,
                .cost = pathCost,
              });
          }
          else
          {
            // We hit a dead end so block it off so any future walks don't even try
            grid[next] = '#';
            continue;
          }
        }

        // Now we definitely have this path available
        auto &path = paths[pathGrid[next]];

        // If it's facing the wrong way and is committed, don't go any further.
        if (path.committed && walk.pos == path.out)
          { continue; }

        // The cost along this path is our current cost, the path's cost, plus 1000 if we had to turn to go down it.
        s32 newCost = walk.postPathCost + path.cost + ((dir == walk.facing) ? 0 : 1000);
        traceQueue.push(
          {
            // An uncommitted path can be facing either direction so get the correct orientation for this travel
            .pos = (walk.pos == path.out) ? path.in : path.out,
            .facing = (walk.pos == path.out) ? -path.inDir : path.outDir,

            .prePathFacing = walk.facing,
            .postPathCost = newCost,
          });
      }
    }

    PrintFmt("P1: {}\n", p1);

    struct Backwalk
    {
      Vec srcPos;
      Vec towardOriginFacing;
    };

    std::queue<Backwalk> backwalkQueue;
    p2 = 2; // Start counting the 'E' and 'S' spaces already.

    // Start the queue with all the paths that came into the end with the minimum cost.
    auto origGrid = ReadFileAsCharArray2D(filePath);

    for (auto d : { -X, X, -Y, Y })
    {
      auto i = pathGrid[endPos + d];
      if (i == 0)
        { continue; }

      for (auto &ent : paths[i].enterCosts)
      {
        if (ent.totalCost != p1)
          { continue; }

        if (grid[endPos + d] != 'P')
        {
          grid[endPos + d] = 'P';
          p2 += paths[i].stepCount;
          backwalkQueue.push({ .srcPos = paths[i].in, .towardOriginFacing = paths[i].inDir });
        }
      }
    }


    // Now trace backwards along the shortest path
    while (!backwalkQueue.empty())
    {
      auto bw = backwalkQueue.front();
      backwalkQueue.pop();

      // If we hit the start we don't have to trace farther.
      if (grid[bw.srcPos] == 'S')
        { continue; }

      // If we haven't counted this space yet, count it now.
      if (grid[bw.srcPos] != 'V')
      {
        // We're marking intersections we reach with 'V' (for visited) as we count them so we don't double-count.
        grid[bw.srcPos] = 'V';
        p2++;
      }

      // Helper lambda to call another lambda for each valid path, with the current cost and the path.
      auto ForEachEntrance =
        [&](auto call)
        {
          for (auto d
            : { bw.towardOriginFacing, bw.towardOriginFacing.RotateRight(), bw.towardOriginFacing.RotateLeft() })
          {
            // If there's no exit here (or if we've already visited it, which we note by marking it with a 'P' for
            //  "path" below), move along.
            if (grid[bw.srcPos - d] != '.')
              { continue; }

            // Get the path itself now and only continue if it ended at our location.
            auto &path = paths[pathGrid[bw.srcPos - d]];
            if (path.out != bw.srcPos)
              { continue; }

            // Now for each entrance calculate the current cost (including turn cost) and then call the lambda.
            for (auto &ent : path.enterCosts)
            {
              s32 cost = ent.totalCost + ((path.outDir != bw.towardOriginFacing) ? 1000 : 0);
              call(cost, path);
            }
          }
        };

      // Use the above lambda to calculate the minimum cost to get into this space from all sides.
      s32 minCost = std::numeric_limits<s32>::max();
      ForEachEntrance([&](auto cost, auto) { minCost = std::min(minCost, cost); });

      // Now that we know the minimum cost we'll visit all of the min-cost ones.
      ForEachEntrance(
        [&](auto cost, auto path)
        {
          if (cost != minCost)
            { return; }

          // Mark this grid space with a 'P' so we won't count it or navigate down it again.
          grid[bw.srcPos - path.outDir] = 'P';
          p2 += path.stepCount;
          backwalkQueue.push({ .srcPos = path.in, .towardOriginFacing = path.inDir });
        });
    }

    PrintFmt("P2: {}\n", p2);
  }
}