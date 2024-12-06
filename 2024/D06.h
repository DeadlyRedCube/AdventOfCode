#pragma once


namespace D06
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray(path);

    std::set<Vec2<s32>> visitedPositions;
    Vec2<s32> initialGuardPos;

    for (s32 y = 0; y < grid.Height(); y++)
    {
      bool found = false;
      for (s32 x = 0; x < grid.Width(); x++)
      {
        if (grid[x, y] == '^')
        {
          initialGuardPos = { x, y };
          found = true;
          break;
        }
      }

      if (found)
        { break; }
    }

    visitedPositions.insert(initialGuardPos);
    auto guardPos = initialGuardPos;
    Vec2<s32> dir = { 0, -1 };

    while (true)
    {
      auto n = guardPos + dir;
      if (!grid.PositionInRange(n))
        { break; }

      if (grid[n] == '#')
      {
        dir = dir.RotateRight();
        continue;
      }

      guardPos = n;

      visitedPositions.insert(guardPos);
    }

    p1 = ssize_t(visitedPositions.size());

    for (auto t : visitedPositions)
    {
      if (t == initialGuardPos)
        { continue; }

      std::set<std::tuple<Vec2<s32>, Vec2<s32>>> positionsAndDirs;
      grid[t] = '#';

      guardPos = initialGuardPos;
      dir = { 0, -1};

      while (true)
      {
        auto n = guardPos + dir;
        if (!grid.PositionInRange(n))
          { break; }

        if (grid[n] == '#')
        {
          dir = dir.RotateRight();
          continue;
        }

        guardPos = n;

        auto [_, inserted] = positionsAndDirs.insert({guardPos, dir});
        if (!inserted)
        {
          p2++;
          break;
        }
      }

      grid[t] = '.';
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}
