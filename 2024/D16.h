#pragma once


namespace D16
{
  using Vec = Vec2S32;
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray2D(path);

    struct Intersection
    {
      Vec pos;
      Vec enterDir; // Direction moving *into* this intersection

      auto operator<=>(const Intersection &) const = default;
    };

    struct Path
    {
      Intersection dest;
      s32 cost;
    };

    struct Paths
    {
      s64 lowestCostTo = std::numeric_limits<s64>::max();
      std::vector<Path> paths;
    };


    std::map<Intersection, Paths> paths;

    constexpr Vec X = { 1, 0 };
    constexpr Vec Y = { 0, 1 };

    Vec s;
    Vec e;
    std::vector<Vec> endDirs;

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

      // We're gonna force S and E into the graph
      if (grid[c] == '.' && entrances < 3)
        { continue; }

      if (grid[c] == '.')
        { grid[c] = 'I'; } // This is an "I"ntersection now.

      if (grid[c] == 'S')
        { grid[c] = 'S'; }
      for (auto d : { X, -X, Y, -Y })
      {
        if (grid[c + d] != '#' || grid[c] == 'S')
        {
          paths[{c, -d}] = {};

          if (grid[c] == 'E')
            { endDirs.push_back(-d); }
        }
      }
    }

    // Now that we've found the intersections trace from them
    for (auto &[i, p] : paths)
    {
      for (auto d : { i.enterDir, i.enterDir.RotateLeft(), i.enterDir.RotateRight() })
      {
        auto c = i.pos + d;
        if (grid[c] == '#')
          { continue; }

        s32 cost = 1 + ((d == i.enterDir) ? 0 : 1000);

        while (true)
        {
          if (grid[c] != '.')
          {
            // Found an intersection (or start/end) so mark it and be done.
            ASSERT(grid[c] != '#');
            ASSERT(paths.contains({c, d}));
            p.paths.push_back({ .dest = { c, d }, .cost = cost });
            break;
          }

          bool foundExit = false;
          for (auto t : { d, d.RotateLeft(), d.RotateRight() })
          {
            if (grid[c + t] != '#')
            {
              // There's only one way out of this spot besides the way we came in so move there.
              cost += 1 + ((t == d) ? 0 : 1000);
              c += t;
              d = t;
              foundExit = true;
              break;
            }
          }

          if (!foundExit)
            { break; }
        }
      }
    }

    struct Walk
    {
      Intersection pos;
      s64 totalCost = 0;
    };

    std::priority_queue<Walk, std::vector<Walk>, decltype([](const Walk &a, const Walk &b) { return a.totalCost > b.totalCost; })> queue;

    queue.push({ .pos = { .pos = s, .enterDir = X } });

    while (!queue.empty())
    {
      auto w = queue.top();
      queue.pop();

      auto &ps = paths[w.pos];

      if (w.totalCost >= ps.lowestCostTo)
        { continue; }

      ps.lowestCostTo = w.totalCost;

      for (auto &p : ps.paths)
      {

        queue.push({ .pos = p.dest, .totalCost = w.totalCost + p.cost });
      }
    }

    p1 = std::numeric_limits<s64>::max();

    for (auto &d : endDirs)
    {
      p1 = std::min(p1, paths[{e, d}].lowestCostTo);
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}