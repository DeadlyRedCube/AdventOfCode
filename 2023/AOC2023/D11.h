#pragma once

namespace D11
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    int yAdd = 0;

    UnboundedArray<Vec2S32> galaxies;
    for (auto y = 0; y < grid.Height(); y++)
    {
      bool anyGalaxy = false;
      for (auto x = 0; x < grid.Width(); x++)
      {
        if (grid.Idx(x, y) == '#')
        {
          anyGalaxy = true;
          galaxies.Append({x, y + yAdd});
        }
      }

      if (!anyGalaxy)
        { yAdd++; }
    }

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
        for (auto &gal : galaxies)
        {
          if (gal.x >= x)
            { gal.x++; }
        }
      }
    }

    s64 sum = 0;
    for (auto i = 0; i < galaxies.Count(); i++)
    {
      auto &galA = galaxies[i];
      for (auto j = i + 1; j < galaxies.Count(); j++)
      {
        auto &galB = galaxies[j];
        s32 dist = std::abs(galA.x - galB.x) + std::abs(galA.y - galB.y);
        sum += dist;
      }
    }

    PrintFmt("{}\n", sum);
  }
}
