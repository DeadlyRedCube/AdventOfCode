#pragma once


namespace D04
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray2D(path);

    for (s32 y = 0; y < grid.Height(); y++)
    {
      for (s32 x = 0; x < grid.Width(); x++)
      {
        auto st = Vec2{x, y};
        if (grid[x, y] == 'X')
        {
          for (auto dir
            : { Vec2{1, 0}, Vec2{0, 1}, Vec2{-1, 0}, Vec2{0, -1}, Vec2{1, 1}, Vec2{1, -1}, Vec2{-1, 1}, Vec2{-1, -1} })
          {
            // If the end of this test is out of bounds we can skip it.
            if (!grid.PositionInRange(st + dir * 3))
              { continue; }
            if (grid[st + dir] == 'M' && grid[st + dir * 2] == 'A' && grid[st + dir * 3] == 'S')
              { p1++; }
          }
        }

        // Find any 'A's that are in one from the edge.
        if (grid[x, y] == 'A' && x > 0 && y > 0 && x < grid.Width() - 1 && y < grid.Height() - 1)
        {
          s32 masCount = 0;
          for (auto dir : { Vec2{1, 1}, Vec2{1, -1} })
          {
            if ((grid[st - dir] == 'M' && grid[st + dir] == 'S')
              || (grid[st - dir] == 'S' && grid[st + dir] == 'M'))
              { masCount++; }
          }

          ASSERT(masCount <= 2);
          if (masCount == 2)
            { p2++; }
        }
      }
    }

    PrintFmt("P1: {}\n", p1);

    PrintFmt("P2: {}\n", p2);
  }
}
