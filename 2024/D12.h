#pragma once


namespace D12
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray(path);

    // This is a second grid that gets filled with an index into a list of calculated areas (one per region). Start
    //  with all -1s which is the "we don't have an index for this space's region yet"
    auto areaIndices = Array2D<s32>{grid.Width(), grid.Height()};
    areaIndices.Fill(-1);

    // This is a stack of next locations to flood fill into.
    std::vector<Vec2<s32>> fillStack;

    // This is the list of areas (indexed
    std::vector<s32> areas;

    // Reserve a best-guess worst case size.
    fillStack.reserve(grid.Width() * 8);
    areas.reserve(grid.Width() * 8);

    for (auto curPos : grid.IterCoords<s32>())
    {
      // Test to see if we've already found the area for this spot.
      if (areaIndices[curPos] < 0)
      {
        // We don't already know the area for whatever this region is so let's find it and fill it in.
        fillStack.clear();

        // Start with the current position.
        fillStack.push_back(curPos);

        // The next area that we calculate will be appended to the end of the areas list so the index for this region
        //  is the current size of that list.
        areaIndices[curPos] = s32(areas.size());

        // Now flood out the coord set.
        s32 area = 0;
        while (!fillStack.empty())
        {
          auto testPos = fillStack.back();
          fillStack.pop_back();

          area++;

          // Test each neighbor to see if it matches (and hasn't already been indexed).
          for (auto dir : { Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1} })
          {
            auto neighbor = testPos + dir;
            if (grid[OOBZero{neighbor}] == grid[testPos] && areaIndices[neighbor] < 0)
            {
              // We need to flood into this one so add it to the stack and update the index
              fillStack.push_back(neighbor);
              areaIndices[neighbor] = s32(areas.size());
            }
          }
        }

        // Now that we've calculated the area of this region, add it to the areas list.
        areas.push_back(area);
      }

      // Now time to calculate the number of fence posts. Since we have the area accessible at each grid spot we can
      //  just calculate how many fences there'll be (how many non-matching borders) for this square and then multiply
      //  it by the area (which will add up to the same result in the end). Start by assuming there will be four
      //  fence section.
      s32 fenceSideCount = 4;
      s32 p2Discount = 0;
      for (auto dir : { Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1} })
      {
        if (grid[OOBZero(curPos + dir)] == grid[curPos])
        {
          // There's a matching grid space in this direction so that's one less fence section.
          fenceSideCount--;
        }
        else
        {
          // For p2 we're going to look to whatever the right is (clockwise) from this direction: if there's a similar
          //  border there (that is, if the clockwise-right tile from here matches our value and the forward tile from
          //  there does not), then we'll discount this spot for part 2 (this ensures that the only fence borders we
          //  count are the first ones in a span clockwise around the area).
          auto right = dir.RotateRight();
          if (grid[OOBZero(curPos + right)] == grid[curPos]
            && grid[OOBZero(curPos + right + dir)] != grid[curPos])
            { p2Discount++; }
        }
      }

      // Aaaand now add this spot's contribution to the price to the final tally.
      p1 += fenceSideCount * areas[areaIndices[curPos]];
      p2 += (fenceSideCount - p2Discount) * areas[areaIndices[curPos]];
    }

    PrintFmt("P1: {}\neighbor", p1);
    PrintFmt("P2: {}\neighbor", p2);
  }
}