export module D09;

import Util;

export namespace D09
{
  using V = Vec2<s32>;



  // Parse a line of text as a Vec2<s32>
  V AsV(const std::string &v)
  {
    char *end;
    V r;
    // Taking advantage of the fact that strtol stops at the first non-digit character to parse without doing a split
    // first.
    r.x = std::strtol(v.c_str(), &end, 10);
    r.y = std::strtol(end + 1, &end, 10);
    return r;
  }



  void Run(const char *path)
  {
    auto redTiles = ReadFileLines(path) | std::views::transform(&AsV) | std::ranges::to<std::vector>();

    std::vector<std::tuple<s64, V, V>> areas;
    areas.reserve((redTiles.size() - 1) * redTiles.size());

    std::map<s32, s32> compressedXes;
    std::map<s32, s32> compressedYs;

    // First, build a mapping from real[x, y] to compressed[x, y]
    for (usz aI = 0; aI < redTiles.size(); aI++)
    {
      // Start at 0 for each of them
      compressedXes.emplace(redTiles[aI].x, 0);
      compressedYs.emplace(redTiles[aI].y, 0);
    }

    // Finish up the mapping and build a reverse mapping as well
    std::vector<s32> uncompressedXes;
    uncompressedXes.resize(compressedXes.size() * 2 - 1);
    for (auto [i, kvp] : std::views::enumerate(compressedXes))
    {
      auto &&[x, v] = kvp;
      v = s32(i) * 2;
      uncompressedXes[usz(v)] = x + 1;
    }

    std::vector<s32> uncompressedYs;
    uncompressedYs.resize(compressedYs.size() * 2);
    uncompressedYs[0] = 0;
    for (auto [i, kvp] : std::views::enumerate(compressedYs))
    {
      auto &&[y, v] = kvp;
      v = s32(i) * 2;
      uncompressedYs[usz(v)] = y + 1;
    }

    // Now that the mapping is done, make a little helper grid that we can use to build the summed area table that
    //  denotes where we enter/exit the shape
    Array2D<char> compressedGrid { s32(compressedXes.size() * 2 - 1), s32(compressedYs.size() * 2 - 1) };
    for (usz i = 0; i < redTiles.size(); i++)
    {
      auto a = redTiles[i];
      auto b = redTiles[(i + 1) % redTiles.size()];
      if (a.y == b.y)
        { continue; } // Ignore horizontal edges

      auto x = compressedXes[a.x];
      auto aY = compressedYs[a.y];
      auto bY = compressedYs[b.y];

      char c = '<';
      if (aY > bY)
      {
        // This is a vertical edge that is an outside->inside transition
        c = '>';
        std::swap(aY, bY);
      }

      for (s32 y = aY; y <= bY; y++)
        { compressedGrid[x, y] = c; }
    }

    // Finally, build a summed area table (add an extra row/column at the left/top that's all 0s to make lookups faster)
    Array2D<s64> areaTable{compressedGrid.Width() + 1, compressedGrid.Height() + 1};
    for (s32 y = 0; y < compressedGrid.Height(); y++)
    {
      bool inGrid = false;
      s64 yLen = (uncompressedYs[usz(y)] == 0)
        ? uncompressedYs[usz(y + 1)] - uncompressedYs[usz(y - 1)] - 1
        : 1;
      s64 areaForJustRow = 0;
      for (s32 x = 0; x < compressedGrid.Width(); x++)
      {
        // This character in the grid tells us that we've now entered the polygon
        if (compressedGrid[x, y] == '>')
          { inGrid = true; }

        // Default the area of this cell to be the area of the cell above it
        areaTable[x + 1, y + 1] = areaTable[x + 1, y];

        if (inGrid)
        {
          s64 xLen = (uncompressedXes[x] == 0)
            ? uncompressedXes[usz(x + 1)] - uncompressedXes[usz(x - 1)] - 1
            : 1;

          // Keep track of the area for only this row
          areaForJustRow += xLen * yLen;

          // Add the current row area to the table's cell area
          areaTable[x + 1, y + 1] += areaForJustRow;
        }


        if (compressedGrid[x, y] == '<')
          { inGrid = false; }
      }
    }

    s64 area1 = 0;
    s64 area2 = 0;

    for (usz aI = 0; aI < redTiles.size(); aI++)
    {
      for (usz bI = aI + 1; bI < redTiles.size(); bI++)
      {
        auto ul = Min(redTiles[aI], redTiles[bI]);
        auto lr = Max(redTiles[aI], redTiles[bI]);

        auto area = s64((lr.x - ul.x + 1)) * (lr.y - ul.y + 1);
        area1 = std::max(area1, area);

        if (area > area2)
        {
          ul = { compressedXes[ul.x], compressedYs[ul.y] };
          lr = { compressedXes[lr.x] + 1, compressedYs[lr.y] + 1 };
          auto areaFromTable = areaTable[lr]
            - areaTable[lr.x, ul.y]
            - areaTable[ul.x, lr.y]
            + areaTable[ul];

          if (areaFromTable == area)
            { area2 = area; }
        }
      }
    }

    PrintFmt("Part 1: {}\n", area1);
    PrintFmt("Part 2: {}\n", area2);
  }
}
