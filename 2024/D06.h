#pragma once


namespace D06
{
  enum class Dir
  {
    N,
    E,
    S,
    W,

    Unset = -1, // A special value for the visits array for spots that the guard never visits
  };

  [[nodiscard]] Dir RotateDirRight(Dir d)
    { return (d == Dir::W) ? Dir::N : Dir(s32(d) + 1); }

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

    auto grid = ReadFileAsCharArray2D(path);

    Vec2<s32> initialGuardPos;

    // Start by finding the initial guard pos.
    for (s32 y = 0; y < grid.Height(); y++)
    {
      bool found = false;
      for (s32 x = 0; x < grid.Width(); x++)
      {
        if (grid[x, y] == '^')
        {
          grid[x, y] = '.';
          initialGuardPos = { x, y };
          found = true;
          break;
        }
      }

      if (found)
        { break; }
    }

    // Make an array of Dirs (all starting Unset) to track both where the guard visits and also which direction they
    //  were going the first time they reached any given space.
    auto visits = Array2D<Dir>{grid.Width(), grid.Height()};
    visits.Fill(Dir::Unset);

    // Mark the initial position as visited (and increment our visit counter by 1)
    visits[initialGuardPos] = Dir::N;
    p1++;

    auto guardPos = initialGuardPos;

    for (auto [dir, dirV] = std::make_tuple(Dir::N, Vec2{0, -1});;)
    {
      auto n = guardPos + dirV;
      if (auto g = grid[OOBZero(n)]; g == 0)
        { break; }
      else if (g == '#')
      {
        // Next spot is a wall so rotate right and carry on.
        dirV = dirV.RotateRight();
        dir = RotateDirRight(dir);
        continue;
      }

      // Move and update the visit grid and count if we hadn't already been here.
      guardPos = n;
      if (visits[guardPos] == Dir::Unset)
      {
        p1++;
        visits[guardPos] = dir;
      }
    }

    auto loopTestDirs = Array2D<DirFlag>{grid.Width(), grid.Height()};

    // We don't want to consider the initial guard post in this.
    visits[initialGuardPos] = Dir::Unset;

    constexpr Vec2<s32> dirVecs[] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    // I tried a few ways and iterating the visits list and checking for "true"s was the fastest
    for (auto vy = 0; vy < visits.Height(); vy++)
    {
      for (auto vx = 0; vx < visits.Width(); vx++)
      {
        auto t = Vec2{vx, vy};
        if (visits[t] == Dir::Unset)
          { continue; } // This isn't a location we visited so there's no point in trying a wall here.

        grid[t] = '#';

        // Start just before the guard would have reached this space.
        guardPos = t - dirVecs[s32(visits[t])];

        // ...but also, we can start facing to the right of this spot because we know we're going to turn (we literally
        //  just put a block in front of the guard)
        auto dir = RotateDirRight(visits[t]);
        auto dirV = dirVecs[s32(dir)];

        // Start with no directions in the loop test grid
        loopTestDirs.Fill(DirFlag::None);
        while (true)
        {
          auto n = guardPos + dirV;

          // Move until we go out of range or reach a wall.
          auto g = grid[OOBZero(n)];
          while (g == '.')
          {
            guardPos = n;
            n += dirV;
            g = grid[OOBZero(n)];
          }

          // We're done if we went out of range, this isn't a loop.
          if (g == 0)
            { break; }

          auto flag = DirFlag(1 << s32(dir));
          if ((loopTestDirs[guardPos] & flag) == flag)
          {
            // We already reached this block going this direction so this is a loop!
            p2++;
            break;
          }

          // Add our direction to the loop and rotate right.
          loopTestDirs[guardPos] |= flag;
          dir = RotateDirRight(dir);
          dirV = dirV.RotateRight();
        }

        // Now remove our temp addition of a wall.
        grid[t] = '.';
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}