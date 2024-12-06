#pragma once


namespace D06
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    std::set<Vec2<s32>> visitedPositions;
    Vec2<s32> initialGuardPos;

    std::vector<std::vector<s32>> blockYsPerX;
    std::vector<std::vector<s32>> blockXsPerY;

    blockYsPerX.resize(grid.Width());
    blockXsPerY.resize(grid.Height());

    for (s32 y = 0; y < grid.Height(); y++)
    {
      for (s32 x = 0; x < grid.Width(); x++)
      {
        if (grid[x, y] == '^')
        {
          initialGuardPos = { x, y };
          break;
        }

        if (grid[x, y] == '#')
        {
          blockYsPerX[x].push_back(y);
          blockXsPerY[y].push_back(x);
        }
      }
    }

    visitedPositions.insert(initialGuardPos);
    auto guardPos = initialGuardPos;
    Vec2<s32> dir = { 0, -1 };

    // Use either a row or column of (sorted) block coordinates to find the next block along the direction
    auto FindNextBlock = [](auto &cs, auto c, auto d, auto max)
    {
      auto found = std::ranges::lower_bound(cs, c);
      if (d < 0)
      {
        if (found == cs.begin())
          { return -1; } // No blocks before our element
        else
          { return *(--found); } // the coordinate before the one we found is correct.
      }
      else
      {
        if (found == cs.end())
          { return s32(max); } // No blocks after our element
        else
          { return *found; } // The one we found is next (we know it won't find the input square because it must be free)
      }
    };

    while (true)
    {
      // Find the next block along this direction
      auto nextBlock = (dir.x != 0)
        ? Vec2{ FindNextBlock(blockXsPerY[guardPos.y], guardPos.x, dir.x, grid.Width()), guardPos.y }
        : Vec2{ guardPos.x, FindNextBlock(blockYsPerX[guardPos.x], guardPos.y, dir.y, grid.Height()) };

      // Add all of the positions along the way into the set until our next step is either off the edge or to the block
      while (guardPos + dir != nextBlock)
      {
        guardPos += dir;
        visitedPositions.insert(guardPos);
      }

      if (!grid.PositionInRange(nextBlock))
        { break; }

      // We hit a block so rotate right and carry on
      dir = dir.RotateRight();
    }

    PrintFmt("P1: {}\n", ssize_t(visitedPositions.size()));

    // Don't test the initial position for blocking.
    visitedPositions.erase(initialGuardPos);

    s64 p2 = 0;
    for (auto t : visitedPositions)
    {
      // Temporarily insert this position into the grid
      blockXsPerY[t.y].insert(std::ranges::upper_bound(blockXsPerY[t.y], t.x), t.x);
      blockYsPerX[t.x].insert(std::ranges::upper_bound(blockYsPerX[t.x], t.y), t.y);

      // Start at the beginning again
      guardPos = initialGuardPos;
      dir = { 0, -1 };

      std::set<std::tuple<Vec2<s32>, Vec2<s32>>> blockAndInitialDir;

      while (true)
      {
        // Find the next place we'll hit (or leave)
        auto nextBlock = (dir.x != 0)
          ? Vec2{ FindNextBlock(blockXsPerY[guardPos.y], guardPos.x, dir.x, grid.Width()), guardPos.y }
          : Vec2{ guardPos.x, FindNextBlock(blockYsPerX[guardPos.x], guardPos.y, dir.y, grid.Height()) };

        // If we're leaving this is not a loop.
        if (!grid.PositionInRange(nextBlock))
          { break; }

        // Otherwise we hit a block, add the block and the direction we hit from into the set.
        auto [_, inserted] = blockAndInitialDir.insert({nextBlock, dir});
        if (!inserted)
        {
          // If it was already in the set then we got here again going the same direction and this is officially a loop
          p2++;
          break;
        }

        // Move to the square before the next block and rotate and keep going.
        guardPos = nextBlock - dir;
        dir = dir.RotateRight();
      }

      // Remove our temporary elements.
      blockXsPerY[t.y].erase(std::ranges::lower_bound(blockXsPerY[t.y], t.x));
      blockYsPerX[t.x].erase(std::ranges::lower_bound(blockYsPerX[t.x], t.y));
    }

    PrintFmt("P2: {}\n", p2);
  }
}
