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

    std::map<std::string, s64> states;

    constexpr s64 IterCount = 1'000'000'000;
    for (s64 iter = 0; iter < IterCount; iter++)
    {
      // North cycle (same code as Part 1)
      for (ssz x = 0; x < grid.Width(); x++)
      {
        ssz freeY = 0;
        for(ssz y = 0; y < grid.Height(); y++)
        {
          newGrid.Idx(x, y) = (grid.Idx(x, y) == '#') ? '#' : '.';
          if (grid.Idx(x, y) == 'O')
          {
            newGrid.Idx(x, freeY) = 'O';
            freeY++;
          }
          else if (grid.Idx(x, y) == '#')
            { freeY = y + 1; }
        }
      }

      std::swap(grid, newGrid);

      // West
      for(ssz y = 0; y < grid.Height(); y++)
      {
        ssz freeX = 0;
        for (ssz x = 0; x < grid.Width(); x++)
        {
          newGrid.Idx(x, y) = (grid.Idx(x, y) == '#') ? '#' : '.';
          if (grid.Idx(x, y) == 'O')
          {
            newGrid.Idx(freeX, y) = 'O';
            freeX++;
          }
          else if (grid.Idx(x, y) == '#')
            { freeX = x + 1; }
        }
      }

      std::swap(grid, newGrid);

      // South
      for (ssz x = 0; x < grid.Width(); x++)
      {
        ssz freeY = grid.Height() - 1;
        for(ssz y = grid.Height() - 1; y >= 0; y--)
        {
          newGrid.Idx(x, y) = (grid.Idx(x, y) == '#') ? '#' : '.';
          if (grid.Idx(x, y) == 'O')
          {
            newGrid.Idx(x, freeY) = 'O';
            freeY--;
          }
          else if (grid.Idx(x, y) == '#')
            { freeY = y - 1; }
        }
      }

      std::swap(grid, newGrid);

      // East - actually calculate the load for this one as it's the last in the cycle
      load = 0;
      for(ssz y = 0; y < grid.Height(); y++)
      {
        ssz freeX = grid.Width() - 1;
        for (ssz x = grid.Width() - 1; x >= 0; x--)
        {
          newGrid.Idx(x, y) = (grid.Idx(x, y) == '#') ? '#' : '.';
          if (grid.Idx(x, y) == 'O')
          {
            newGrid.Idx(freeX, y) = 'O';
            load += grid.Height() - y;
            freeX--;
          }
          else if (grid.Idx(x, y) == '#')
            { freeX = x - 1; }
        }
      }

      std::swap(grid, newGrid);

      // Turn the board state into a string so that it's easy to throw into a map.
      std::string state;
      for(ssz y = 0; y < grid.Height(); y++)
      {
        for (ssz x = 0; x < grid.Width(); x++)
          { state += grid.Idx(x, y); }
      }

      if (auto startIter = states.find(state); startIter != states.end())
      {
        // We already encountered this state, so we know there's a loop!
        s64 loopOffset = startIter->second;
        s64 loopLength = iter - loopOffset;

        // Could have stored the load values, but instead just jump to the correct amount of iterations before the
        //  end and just re-simulate the rest.
        iter = IterCount - (IterCount - loopOffset) % loopLength;

        // Clear the states map so that we don't keep hitting matches (since we've already done all of these).
        states = {};
      }

      // Store the state.
      states[state] = iter;
    }

    // The last calculated load value is the correct one
    PrintFmt("Part 2: {}\n", load);
  }
}