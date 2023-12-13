#pragma once

namespace D13
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    UnboundedArray<Array2D<char>> grids;
    int prevGridStart = 0;
    for (int i = 0; i < lines.size(); i++)
    {
      if (lines[i].empty())
      {
        grids.Append({ssz(lines[i - 1].length()), i - prevGridStart});
        auto &grid = grids[FromEnd(-1)];
        for (int y = 0; y < grid.Height(); y++)
        {
          for (int x = 0; x < grid.Width(); x++)
            { grid.Idx(x, y) = lines[y + prevGridStart][x]; }
        }

        prevGridStart = i + 1;
      }
    }

    {
      grids.Append({ssz(lines[lines.size() -1].length()), ssz(lines.size()) - prevGridStart});
      auto &grid = grids[FromEnd(-1)];
      for (int y = 0; y < grid.Height(); y++)
      {
        for (int x = 0; x < grid.Width(); x++)
          { grid.Idx(x, y) = lines[y + prevGridStart][x]; }
      }
    }

    s64 horizontalReflections = 0;
    s64 verticalReflections = 0;

    s64 p2HReflections = 0;
    s64 p2VReflections = 0;
    for (auto &grid : grids)
    {
      PrintFmt("GRID : {} x {}\n", grid.Width(), grid.Height());
      for (ssz reflPoint = 1; reflPoint < grid.Width(); reflPoint++)
      {
        s32 failCount = 0;
        for (ssz y = 0; y < grid.Height(); y++)
        {
          for (ssz i = 0; i < std::min(reflPoint, grid.Width() - reflPoint); i++)
          {
            if (grid.Idx(reflPoint + i, y) != grid.Idx(reflPoint - i - 1, y))
            {
              failCount++;
              if (failCount >= 2)
                { break; }
            }
          }

          if (failCount >= 2)
            { break; }
        }

        if (failCount == 0)
        {
          PrintFmt("H:  {}\n", reflPoint);
          horizontalReflections += reflPoint;
        }
        if (failCount == 1)
        {
          PrintFmt("H2: {} !!! \n", reflPoint);
          p2HReflections += reflPoint;
        }
      }

      for (ssz reflPoint = 1; reflPoint < grid.Height(); reflPoint++)
      {
        s32 failCount = 0;
        for (ssz x = 0; x < grid.Width(); x++)
        {
          for (ssz i = 0; i < std::min(reflPoint, grid.Height() - reflPoint); i++)
          {
            if (grid.Idx(x, reflPoint + i) != grid.Idx(x, reflPoint - i - 1))
            {
              failCount++;
              if (failCount >= 2)
                { break; }
            }
          }

          if (failCount >= 2)
            { break; }
        }

        if (failCount == 0)
        {
          PrintFmt("V:  {}\n", reflPoint);
          verticalReflections += reflPoint;
        }
        if (failCount == 1)
        {
          PrintFmt("V2: {} !!! \n", reflPoint);
          p2VReflections += reflPoint;
        }
      }
    }

    PrintFmt("Part 1: {}\n", horizontalReflections + 100 * verticalReflections);
    PrintFmt("Part 2: {}\n", p2HReflections + 100 * p2VReflections);
  }
}