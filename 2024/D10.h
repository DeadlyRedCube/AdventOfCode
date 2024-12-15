#pragma once


namespace D10
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray2D(path);

    // Keep track of which spots were visited (as a grid of bools with the same dims as the map)
    auto visited = Array2D<bool>{grid.Width(), grid.Height()};

    std::queue<Vec2<s32>> queuedPositions;
    for (s32 outerY = 0; outerY < grid.Height(); outerY++)
    {
      for (s32 outerX = 0; outerX < grid.Width(); outerX++)
      {
        if (grid[outerX, outerY] != '0')
          { continue; } // not a trailhead

        // Clear all of our visit flags because we're starting at a new trailhead
        visited.Fill(false);

        // Start at the trailhead
        queuedPositions.push({outerX, outerY});

        while (!queuedPositions.empty())
        {
          // Get the next queued position
          auto p = queuedPositions.front();
          queuedPositions.pop();

          // Figure out the next height we want to step to
          auto nextHeight = grid[p] + 1;

          // Now try each direction from the current position to see if it's a valid step or not
          for (auto d : { Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1}})
          {
            // If it's the next height (using the OOBZero helper to return 0 (not '0') for out of bounds reads)
            if (grid[OOBZero(p + d)] == nextHeight)
            {
              if (nextHeight == '9')
              {
                // This is the end of a trail. If it's the first time we got here, it's a unique trailhead ending which
                //  counts for p1. (For p1 originally the visited test was in the outer "if" to early out)
                if (!visited[p + d])
                  { p1++; }

                // Every path to the end counts for p2.
                p2++;
              }
              else
                { queuedPositions.push(p + d); } // This is a valid next step but not the end yet so queue it for now.

              // Mark this spot as visited so we won't double-count it for p1.
              visited[p + d] = true;
            }
          }
        }
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}