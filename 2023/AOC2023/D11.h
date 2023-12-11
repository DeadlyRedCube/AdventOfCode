#pragma once

namespace D11
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    // Y-coordinate adders updated on blank linkes
    s64 yAdd1 = 0;
    s64 yAdd2 = 0;

    UnboundedArray<Vec2S64> galaxies1;
    UnboundedArray<Vec2S64> galaxies2;
    for (auto y = 0; y < grid.Height(); y++)
    {
      bool anyGalaxy = false;
      for (auto x = 0; x < grid.Width(); x++)
      {
        if (grid.Idx(x, y) == '#')
        {
          anyGalaxy = true;
          galaxies1.Append({x, y + yAdd1});
          galaxies2.Append({x, y + yAdd2});
        }
      }

      if (!anyGalaxy)
      {
        // If there were no galaxies, part 1 says each line becomes 2 and part 2 says each line becomes a million
        //  (yes, that -1 is there because I was off by one the first time, whoops)
        yAdd1++;
        yAdd2 += 1000000 - 1;
      }
    }

    // Now scan through each column from right to left and update our galaxy coordinates to take into account the
    //  empty columns
    for (auto x = grid.Width() - 1; x >= 0; x--)
    {
      bool anyGalaxy = false;
      for (auto y = 0; y < grid.Height(); y++)
      {
        if (grid.Idx(x, y) == '#')
        {
          anyGalaxy = true;
          break;
        }
      }

      if (!anyGalaxy)
      {
        for (auto &gal : galaxies1)
        {
          if (gal.x >= x)
            { gal.x++; }
        }

        for (auto &gal : galaxies2)
        {
          if (gal.x >= x)
            { gal.x += 1000000 - 1; }
        }
      }
    }

    s32 part = 1;
    for (auto &gals : { galaxies1, galaxies2 })
    {
      s64 sum = 0;
      for (auto i = 0; i < gals.Count(); i++)
      {
        auto &galA = gals[i];
        for (auto j = i + 1; j < gals.Count(); j++)
        {
          auto &galB = gals[j];
          s64 dist = std::abs(galA.x - galB.x) + std::abs(galA.y - galB.y);
          sum += dist;
        }
      }

      PrintFmt("Part {}: {}\n", part, sum);
      part++;
    }
  }
}
