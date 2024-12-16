#pragma once


namespace D16
{
  void Run(const char *path)
  {
    s32 p1 = 0;
    s32 p2 = 0;

    auto grid = ReadFileAsCharArray2D(path);

    using Vec = Vec2S32;
    constexpr auto X = Vec::XAxis();
    constexpr auto Y = Vec::YAxis();
    std::vector<Vec> endDirs;

    // Start by finding the start and ending positions.
    Vec s, e;
    for (auto c : grid.IterCoords<s32>())
    {
      if (grid[c] == 'S')
      {
        s = c;
        if (e != Vec{})
          { break; }
      }

      if (grid[c] == 'E')
      {
        e = c;

        for (auto d : { X, -X, Y, -Y })
        {
          // Along with the end positions, it's helpful for part 2 to keep a list of which directions can enter the
          //  end.
          if (grid[c + d] != '#')
            { endDirs.push_back(-d); }
        }
        if (s != Vec{})
          { break; }
      }
    }

    struct NodeKey
    {
      Vec pos;
      Vec facing;
      bool operator==(const NodeKey &) const = default;
    };

    auto HashNodeKey = [](const auto &v) { return std::hash<Vec>{}(v.pos) ^ std::hash<Vec>{}(v.facing); };

    struct NodePath
    {
      NodeKey src;
      NodeKey dst;
      s32 cost;
      s32 stepCount;
    };

    struct NodeVal
    {
      s32 lowestCostTo = std::numeric_limits<s32>::max();
      std::vector<NodePath> entrancePaths;
      std::vector<NodePath> exitPaths;
    };

    std::unordered_map<NodeKey, NodeVal, decltype(HashNodeKey)> graph;

    struct PathKey
    {
      Vec src;
      Vec exitDir;
      bool operator==(const PathKey &) const = default;
    };

    auto HashPathKey = [](const auto &v) { return std::hash<Vec>{}(v.src) ^ std::hash<Vec>{}(v.exitDir); };

    enum class ExitType
    {
      Untraced,
      DeadEnd,
      Traced,
    };

    struct PathVal
    {
      ExitType type = ExitType::Untraced;
      NodeKey dst;
      s32 cost;      // The cost of traveling along this path (from start to end)
      s32 stepCount; // The number of steps along this path (not counting the endpoints)
    };


    std::unordered_map<PathKey, PathVal, decltype(HashPathKey)> paths;

    struct Walk
    {
      NodeKey nodeKey;
      s32 totalCost = 0;
    };

    auto CmpWalk = [](const Walk &a, const Walk &b) { return a.totalCost > b.totalCost; };
    std::priority_queue<Walk, std::vector<Walk>, decltype(CmpWalk)> traceQueue;

    // Start at the start, facing east.
    traceQueue.push({ .nodeKey = { .pos = s, .facing = X } });

    p1 = std::numeric_limits<s32>::max();
    while (!traceQueue.empty())
    {
      auto walk = traceQueue.top();
      traceQueue.pop();

      auto &nodeVal = graph[walk.nodeKey];

      // If we've walked farther than the last time it took to get to this node (or farther than the shortest distance
      //  to the exit) we can stop early.
      if (walk.totalCost > nodeVal.lowestCostTo || walk.totalCost > p1)
        { continue; }

      nodeVal.lowestCostTo = walk.totalCost;

      if (walk.nodeKey.pos == e)
      {
        // We'll only set this on shortest paths, and we don't have to keep tracing out from here.
        p1 = nodeVal.lowestCostTo;
        continue;
      }

      if (nodeVal.exitPaths.empty())
      {
        // Now we need to trace the exits
        for (auto exitDir : { walk.nodeKey.facing, walk.nodeKey.facing.RotateLeft(), walk.nodeKey.facing.RotateRight() })
        {
          auto target = walk.nodeKey.pos + exitDir;

          if (grid[target] == '#')
            { continue; } // No exit this way

          auto pathKey = PathKey{ .src = walk.nodeKey.pos, .exitDir = exitDir };
          auto &pathVal = paths[pathKey];
          if (pathVal.type == ExitType::DeadEnd)
            { continue; } // We already checked and this was a dead end so we can move on.

          if (pathVal.type == ExitType::Untraced)
          {
            // We haven't traced this path yet so do so now.
            auto facing = exitDir;

            s32 cost = 1; // Don't count the initial turn since this path info is global.

            // Start the step count as 0 here instead of 1 because we want to not count the endpoints, and we'll count the
            //  destination in the loop below, so rather than subtracting 1 at the end to counter that, just don't count
            //  this first step.
            s32 stepCount = 0;

            while (true)
            {
              if (grid[target] == '.')
              {
                // See how many exits this space has
                s32 exitCount = 0;
                Vec outDir;
                for (auto d : { X, -X, Y, -Y })
                {
                  if (grid[target + d] != '#')
                  {
                    if (d != -facing)
                      { outDir = d; }
                    exitCount++;
                  }
                }

                if (exitCount == 1)
                {
                  // Dead end. Mark it so we don't try to trace it again.
                  pathVal.type = ExitType::DeadEnd;
                  break;
                }

                if (exitCount == 2)
                {
                  // This is just a path so move along.
                  target += outDir;
                  cost += 1 + ((facing == outDir) ? 0 : 1000);
                  facing = outDir;
                  stepCount++;
                  continue;
                }

                if (exitCount > 2)
                {
                  // This is an intersection so mark it with an I so future traces to here are easier
                  grid[target] = 'I';
                }
              }

              // Found an intersection or the end. Update this path to do the right thing, then connect up to our node
              pathVal =
                {
                  .type = ExitType::Traced,
                  .dst = { .pos = target, .facing = facing },
                  .cost = cost,
                  .stepCount = stepCount,
                };
              break;
            }
          }

          if (pathVal.type == ExitType::Traced)
          {
            // This reached an ending so add it to the list (applying the cost of turning if it is necessary)
            NodePath nodePath =
              {
                .src = walk.nodeKey,
                .dst = pathVal.dst,
                .cost = pathVal.cost + ((pathKey.exitDir == walk.nodeKey.facing) ? 0 : 1000),
                .stepCount = pathVal.stepCount,
              };

            // Link in both directions.
            nodeVal.exitPaths.push_back(nodePath);
            graph[pathVal.dst].entrancePaths.push_back(nodePath);
          }
        }
      }

      // One way or another we have our path list now, so move along the paths.
      for (auto &exit : nodeVal.exitPaths)
        { traceQueue.push({ .nodeKey = exit.dst , .totalCost = walk.totalCost + exit.cost }); }
    }

    PrintFmt("P1: {}\n", p1);

    // Now time to do part 2.

    p2 = 1; // Start by counting the end.

    // Figure out which entrance(s) into the exit cost exactly our lowest exit cost and queue them - that's where we'll
    //  start tracing.
    std::queue<NodePath> queue;
    for (auto &d : endDirs)
    {
      auto &data = graph[{e, d}];
      for (auto &ent : data.entrancePaths)
      {
        if (graph[ent.src].lowestCostTo + ent.cost == p1)
          { queue.push(ent);  }
      }
    }

    while (!queue.empty())
    {
      auto q = queue.front();
      queue.pop();

      // We'll use a "T" on the grid at the entrance to the origin to mark whether or not we've already counted this
      //  path's steps or not.
      if (grid[q.dst.pos - q.dst.facing] != 'T')
      {
        grid[q.dst.pos - q.dst.facing] = 'T';
        p2 += q.stepCount;
      }

      // We're going to use 0 through 15 as a set of four flags denoting which directions a grid space has been visited
      //  from.
      if (grid[q.src.pos] >= 15)
      {
        // We haven't visited this space yet so count it as a space along the best path and clear its value (no flags).
        p2++;
        grid[q.src.pos] = 0;
      }

      // Build a flag value out of our direction (the values don't matter as long as each cardinal direction is unique)
      u32 dirFlag = (q.src.facing.x != 0)
        ? ((q.src.facing.x < 0) ? 1 : 2)
        : ((q.src.facing.y < 0) ? 4 : 8);

      if ((grid[q.src.pos] & dirFlag) != 0)
        { continue; } // We've already gone into that space in this direction so we don't need to do it again.
      else
        { grid[q.src.pos] |= char(dirFlag); } // Add our flag into the grid.

      // Now we're going to scan the other end's entrances and any that cost the lowest cost we'll trace along.
      auto &otherEnd = graph[q.src];
      for (auto &ent : otherEnd.entrancePaths)
      {
        if (graph[ent.src].lowestCostTo + ent.cost == otherEnd.lowestCostTo)
          { queue.push(ent); }
      }
    }

    PrintFmt("P2: {}\n", p2);
  }
}