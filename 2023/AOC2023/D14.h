#pragma once

#include <set>
namespace D14
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    auto newGrid = Array2D<char>{ grid.Width(), grid.Height() };

    s64 load = 0;
    for (s32 x = 0; x < grid.Width(); x++)
    {
      s32 freeY = 0;  // Keep track of the next free Y position for a ball to roll to
      for(s32 y = 0; y < grid.Height(); y++)
      {
        // Add walls to newGrid but not balls
        newGrid.Idx(x, y) = (grid.Idx(x, y) == '#') ? '#' : '.';
        if (grid.Idx(x, y) == 'O')
        {
          // This was a ball, so roll it up to the next free space, calculate the corresponding load, and then
          //  bump the free space to the next spot.
          newGrid.Idx(x, freeY) = 'O';
          load += grid.Height() - freeY;
          freeY++;
        }
        else if (grid.Idx(x, y) == '#')
          { freeY = y + 1; }  // Next free space is now the space below this wall (balls won't roll past the wall)
      }
    }

    PrintFmt("Part 1:{}\n", load);
  }
}