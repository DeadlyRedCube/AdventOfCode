#pragma once


namespace D16
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray2D(path);

    using Vec = Vec2S32;

    // A graph node is a position and vacing.
    struct Node
    {
      Vec pos;
      Vec facing;
      bool operator==(const Node &) const = default;
    };

    struct Path
    {
      Node otherEnd;
      s64 cost; // The cost of traveling along this path (from start to end)
      s32 stepCount; // The number of steps along this path (not counting the endpoints)
    };

    struct Connections
    {
      s64 lowestCostTo = std::numeric_limits<s64>::max();
      std::vector<Path> entrances;
      std::vector<Path> exits;
    };

    auto HashNode = [](const auto &v) { return std::hash<Vec>{}(v.pos) ^ std::hash<Vec>{}(v.facing); };
    std::unordered_map<Node, Connections, decltype(HashNode)> graph;

    constexpr Vec X = Vec::XAxis();
    constexpr Vec Y = Vec::YAxis();

    Vec s;
    Vec e;
    std::vector<Vec> endDirs;

    // Start by scanning through the id for three things:
    //  - start position
    //  - end position (and the directions leading into the end)
    //  - any position in the graph that has
    for (auto c : grid.IterCoords<s32>())
    {
      if (grid[c] == '#')
        { continue; }

      if (grid[c] == 'S')
        { s = c; }
      else if (grid[c] == 'E')
        { e = c; }

      s32 entrances = 0;
      for (auto d : { X, -X, Y, -Y })
      {
        if (grid[c + d] != '#')
          { entrances++; }
      }

      // If it's an empty space (i.e. not 'S'tart or 'E'nd) and there are only two ways in (or less?!?) skip it, it's
      //  not interesting at this time.
      if (grid[c] == '.' && entrances < 3)
        { continue; }

      if (grid[c] == '.')
        { grid[c] = 'I'; } // This is an "I"ntersection now (to make scanning paths later easier)

      if (grid[c] == 'S')
      {
        // We don't count the real entrances to the start position, only "entering" facing east counts.
        grid[c] = 'S';
        graph[{c, X}] = {};
        continue;
      }

      // Check every direction for an exit and build a graph node for coming in from that direction
      for (auto d : { X, -X, Y, -Y })
      {
        if (grid[c + d] == '#')
          { continue; }

        graph[{c, -d}] = {};

        // Track the directions into the end to make it easier to find the best exits for part 2.
        if (grid[c] == 'E')
          { endDirs.push_back(-d); }
      }
    }

    // Now that we've found the intersections trace from them to build the whole graph of connections
    for (auto &[node, conns] : graph)
    {
      // We can't go back so test all of the three forward directions.
      for (auto dir : { node.facing, node.facing.RotateLeft(), node.facing.RotateRight() })
      {
        auto c = node.pos + dir;
        if (grid[c] == '#')
          { continue; }

        s64 cost = 1 + ((dir == node.facing) ? 0 : 1000); // One square to step in plus 1000 if we turn

        // Start the step count as 0 here instead of 1 because we want to not count the endpoints, and we'll count the
        //  destination in the loop below, so rather than subtracting 1 at the end to counter that, just don't count
        //  this first step.
        s32 stepCount = 0;

        while (true)
        {
          if (grid[c] != '.')
          {
            // Found an intersection (or start/end) so mark it and be done.
            if (grid[c] == 'S')
              { break; } // Don't actually bother tracing back to the entrance, we don't care.

            auto otherEnd = Node{c, dir};
            ASSERT(grid[c] != '#');
            ASSERT(graph.contains(otherEnd));

            // Link both directions - exits for part 1 and entrances for part 2.
            conns.exits.push_back({ .otherEnd = otherEnd, .cost = cost, .stepCount = stepCount });
            graph[otherEnd].entrances.push_back(
              { .otherEnd = node, .cost = cost, .stepCount = stepCount });
            break;
          }

          bool foundExit = false;
          for (auto t : { dir, dir.RotateLeft(), dir.RotateRight() })
          {
            if (grid[c + t] != '#')
            {
              // There's only one way out of this spot besides the way we came in so move there.
              cost += 1 + ((t == dir) ? 0 : 1000); // Once again, 1000 cost if we turn
              stepCount++;
              c += t;
              dir = t;
              foundExit = true;
              break;
            }
          }

          if (!foundExit)
            { break; }
        }
      }
    }

    // Scope in part 1 so that part 2 can live separately
    {
      struct Walk
      {
        Node node;
        s64 totalCost = 0;
      };

      // Use a priority queue to do a breadth-first search
      auto CmpWalk = [](const Walk &a, const Walk &b) { return a.totalCost > b.totalCost; };
      std::priority_queue<Walk, std::vector<Walk>, decltype(CmpWalk)> queue;

      // Start at the start, facing east.
      queue.push({ .node = { .pos = s, .facing = X } });

      p1 = std::numeric_limits<s64>::max();
      while (!queue.empty())
      {
        auto w = queue.top();
        queue.pop();

        auto &data = graph[w.node];

        if (w.totalCost > data.lowestCostTo || w.totalCost > p1)
          { continue; }

        data.lowestCostTo = w.totalCost;

        if (w.node.pos == e)
          { p1 = data.lowestCostTo; } // We'll only set this on short paths

        for (auto &ex : data.exits)
          { queue.push({ .node = ex.otherEnd, .totalCost = w.totalCost + ex.cost }); }
      }
    }

    p2 = 1; // Start by counting the end.

    struct Q
    {
      Node origin;
      const Path *p;
    };

    std::queue<Q> queue;

    // Figure out which entrances into the exit cost exactly our lowest exit cost and queue them - that's where we'll
    //  start tracing.
    for (auto &d : endDirs)
    {
      auto &data = graph[{e, d}];
      for (auto &ent : data.entrances)
      {
        if (graph[ent.otherEnd].lowestCostTo + ent.cost == p1)
          { queue.push({ .origin = {e, d}, .p = &ent });  }
      }
    }

    while (!queue.empty())
    {
      auto q = queue.front();
      queue.pop();

      // We'll use a "T" on the grid at the entrance to the origin to mark whether or not we've already counted this
      //  path's steps or not.
      if (grid[q.origin.pos - q.origin.facing] != 'T')
      {
        grid[q.origin.pos - q.origin.facing] = 'T';
        p2 += q.p->stepCount;
      }

      // We're going to use 0 through 15 as a set of four flags denoting which directions a grid space has been visited
      //  from.
      if (grid[q.p->otherEnd.pos] >= 15)
      {
        // We haven't visited this space yet so count it as a space along the best path and clear its value (no flags).
        p2++;
        grid[q.p->otherEnd.pos] = 0;
      }

      // Build a flag value out of our direction (the values don't matter as long as each cardinal direction is unique)
      auto otherFacing = q.p->otherEnd.facing;
      u32 dirFlag = (otherFacing.x != 0)
        ? ((otherFacing.x < 0) ? 1 : 2)
        : ((otherFacing.y < 0) ? 4 : 8);

      if ((grid[q.p->otherEnd.pos] & dirFlag) != 0)
        { continue; } // We've already gone into that space in this direction so we don't need to do it again.
      else
        { grid[q.p->otherEnd.pos] |= char(dirFlag); } // Add our flag into the grid.

      // Now we're going to scan the other end's entrances and any that cost the lowest cost we'll trace along.
      auto &otherEnd = graph[q.p->otherEnd];
      for (auto &ent : otherEnd.entrances)
      {
        if (graph[ent.otherEnd].lowestCostTo + ent.cost == otherEnd.lowestCostTo)
          { queue.push({ .origin = q.p->otherEnd, .p = &ent}); }
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}