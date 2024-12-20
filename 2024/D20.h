#pragma once


namespace D20
{
  using Vec = Vec2<s32>;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();

  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray2D(filePath);

    Vec startPos, endPos;
    for (auto c : grid.IterCoords<s32>())
    {
      if (grid[c] == 'S')
        { startPos = c; grid[c] = '.'; }
      else if (grid[c] == 'E')
        { endPos = c; grid[c] = '.';  }
    }

    auto distances = Array2D<s32>(grid.Width(), grid.Height());
    distances.Fill(std::numeric_limits<s32>::max());

    std::queue<Vec> queue;
    queue.push({endPos});

    // Do a quick BFS to find all the distances from the end
    distances[endPos] = 0;
    while (!queue.empty())
    {
      auto w = queue.front();
      queue.pop();

      if (w == startPos)
        { break; } // Don't keep scanning past the start

      auto dist = distances[w];
      for (auto d : {X, -X, Y, -Y})
      {
        auto n = w + d;
        if (grid[n] == '#')
          { continue; }

        if (distances[n] < std::numeric_limits<s32>::max())
          { continue; }

        distances[n] = dist + 1;
        queue.push(n);
      }
    }

    for (auto c : grid.IterCoords<s32>())
    {
      if (grid[c] != '.')
        { continue; }

      // Scan for nearest within 20 along any direction
      for (s32 dy = 0; dy <= 20; dy++) // Only scan from the current row downwards (don't double-count up)
      {
        // Scan from left to right, but if we're at y == 0 then start just to the right (don't want to double-count
        //  shortcuts to the left of "c")
        for (s32 dx = (dy == 0 ? 1 : (dy - 20)) ; dx <= (20 - dy); dx++)
        {
          auto n = c + Vec(dx, dy);
          if (grid[OOBZero(n)] != '.')
            { continue; }

          auto manhat = std::abs(dx) + dy;
          auto cheat = std::abs(distances[n] - distances[c]) - manhat;

          if (cheat >= 100)
          {
            if (manhat <= 2)
              { p1++; } // only 2-step cheats count for p1
            p2++;
          }
        }
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}