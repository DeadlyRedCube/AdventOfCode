#pragma once


namespace D06
{
  using Vec = Vec2<s32>;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();

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

    std::vector<std::vector<s32>> wallXsPerY;
    std::vector<std::vector<s32>> wallYsPerX;
    wallYsPerX.resize(grid.Width());
    wallXsPerY.resize(grid.Height());

    // Start by finding the initial guard pos.
    for (auto c : grid.IterCoords<s32>())
    {
      if (grid[c] == '^')
      {
        grid[c] = '.';
        initialGuardPos = c;
      }

      if (grid[c] == '#')
      {
        wallXsPerY[c.y].push_back(c.x);
        wallYsPerX[c.x].push_back(c.y);
      }
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


    // We don't want to consider the initial guard post in this.
    visits[initialGuardPos] = Dir::Unset;

    constexpr Vec dirVecs[] = { -Y, X, Y, -X };

    // Make a grid of the last wallTestIndex value that hit a wall at this location.
    auto loopTestIndices = Array2D<s32>{grid.Width(), grid.Height()};

    for (s32 wallTestIndex = 0; auto wallTest : visits.IterCoords<s32>())
    {
      auto c = wallTest;
      if (visits[c] == Dir::Unset)
        { continue; } // Skip any unvisited locations in the grid.

      ++wallTestIndex;

      // Start just before the guard would have reached this space, rotated to the right of where we were facing when
      //  we would have gotten here the first time
      auto dir = RotateDirRight(visits[c]);
      c -= dirVecs[s32(visits[c])];

      // Start with no directions in the loop test grid
      loopTestIndices[c] = wallTestIndex;

      // Helper lambda to commonize this gross direction-scanning code
      auto ScanDir = [&]<s32 Vec::*travelXY, s32 Vec::*crossXY, auto pred>(auto &&range)
        {
          auto foundWall = std::ranges::find_if(range, [&](auto xy) { return pred(xy, c.*travelXY); });
          s32 coord;
          if (c.*crossXY == wallTest.*crossXY && pred(wallTest.*travelXY, c.*travelXY))
          {
            // The wallTest position is in front of us so we need to see if it's closer to us than foundWall (if we
            //  even found a wall)
            coord = (foundWall == range.end())
              ? wallTest.*travelXY // No wall found so use wallTest's coord directly
              : (pred(wallTest.*travelXY, *foundWall) ? *foundWall : wallTest.*travelXY); // Use the closer of the two
          }
          else if (foundWall == range.end())
            { return false; } // We did not find a wall so this is not a loop
          else
            { coord = *foundWall; } // Found a wall (and we're not in line with wallTest) so use the found wall directly.

          c.*travelXY = coord - dirVecs[s32(dir)].*travelXY;
          dir = RotateDirRight(dir);
          return true;
        };


      while (true)
      {
        // If we're facing a wall, turn right.
        while (grid[OOBZero(c + dirVecs[s32(dir)])] == '#')
          { dir = RotateDirRight(dir); }

        switch (dir)
        {
        case Dir::N:
          // Scan backwards through the list for the first wall with a y less than ours
          if (!ScanDir.operator()<&Vec::y, &Vec::x, std::less<s32>{}>(wallYsPerX[c.x] | std::views::reverse))
            { goto EndOfLoop; }
          break;
        case Dir::S:
          if (!ScanDir.operator()<&Vec::y, &Vec::x, std::greater<s32>{}>(wallYsPerX[c.x]))
            { goto EndOfLoop; }
          break;
        case Dir::W:
          if (!ScanDir.operator()<&Vec::x, &Vec::y, std::less<s32>{}>(wallXsPerY[c.y] | std::views::reverse))
            { goto EndOfLoop; }
          break;
        case Dir::E:
          if (!ScanDir.operator()<&Vec::x, &Vec::y, std::greater<s32>{}>(wallXsPerY[c.y]))
            { goto EndOfLoop; }
          break;
        }

        if (loopTestIndices[c] == wallTestIndex)
        {
          // We've already been here during this wallTest so it's a loop!
          p2++;
          break;
        }

        loopTestIndices[c] = wallTestIndex;
      }

    EndOfLoop:;
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}