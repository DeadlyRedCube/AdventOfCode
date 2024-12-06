#pragma once


namespace D06
{
  enum class Dir
  {
    N,
    E,
    S,
    W,
  };

  enum class DirFlag
  {
    N = 0x1,
    E = 0x8,
    S = 0x2,
    W = 0x4,

    None = 0,
    All = 0xf,
  };

  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray(path);

    Vec2<s32> initialGuardPos;

    // Start by finding the initial guard pos.
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

    // Make a bool array that's the same size as our grid initially filled with false (We'll use this to track what's
    //  visited)
    auto visits = Array2D<bool>{grid.Width(), grid.Height()};
    visits.Fill(false);

    // Mark the initial position as visited (and increment our visit counter by 1)
    visits[initialGuardPos] = true;
    p1++;

    auto guardPos = initialGuardPos;

    for (auto dirV = Vec2{0, -1};;)
    {
      auto n = guardPos + dirV;
      if (!grid.PositionInRange(n))
        { break; } // Out of range so we're done.

      if (grid[n] == '#')
      {
        // Next spot is a wall so rotate right and carry on.
        dirV = dirV.RotateRight();
        continue;
      }

      // Move and update the visit grid and count if we hadn't already been here.
      guardPos = n;
      if (!visits[guardPos])
      {
        p1++;
        visits[guardPos] = true;
      }
    }

    auto visitDirs = Array2D<DirFlag>{grid.Width(), grid.Height()};
    constexpr Vec2<s32> dirVecs[] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    // We don't want to consider the initial guard post in this.
    visits[initialGuardPos] = false;

    // I tried a few ways and iterating the visits list and checking for "true"s was the fastest
    for (auto vy = 0; vy < visits.Height(); vy++)
    {
      for (auto vx = 0; vx < visits.Width(); vx++)
      {
        auto t = Vec2{vx, vy};
        if (!visits[t])
          { continue; } // This isn't a location we visited so there's no point in trying a wall here.

        grid[t] = '#';

        // Start at the beginning
        guardPos = initialGuardPos;
        auto dir = Dir::N;
        auto dirV = dirVecs[s32(dir)];

        visitDirs.Fill(DirFlag::None);

        while (true)
        {
          auto n = guardPos + dirV;

          // Move until we go out of range or reach a wall.
          while (grid.PositionInRange(n) && grid[n] != '#')
          {
            guardPos = n;
            n += dirV;
          }

          // We're done if we went out of range, this isn't a loop.
          if (!grid.PositionInRange(n))
            { break; }

          auto flag = DirFlag(1 << s32(dir));
          if ((visitDirs[guardPos] & flag) == flag)
          {
            // We already reached this block going this direction so this is a loop!
            p2++;
            break;
          }

          // Add our direction to the loop and rotate right.
          visitDirs[guardPos] |= flag;
          dir = (dir == Dir::W) ? Dir::N : Dir(s32(dir) + 1);
          dirV = dirVecs[s32(dir)];
        }

        // Now remove our temp addition of a wall.
        grid[t] = '.';
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}