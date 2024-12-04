#pragma once


namespace D04
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto grid = ReadFileAsCharArray(path);

    UnboundedArray<Vec2<s32>> Xcoords;
    UnboundedArray<Vec2<s32>> Acoords;
    for (s32 y = 0; y < grid.Height(); y++)
    {
      for (s32 x = 0; x < grid.Width(); x++)
      {
        if (grid[x, y] == 'X')
          { Xcoords.Append({x, y}); }

        // Find any 'A's that are in one from the edge.
        if (grid[x, y] == 'A' && x > 0 && y > 0 && x < grid.Width() - 1 && y < grid.Width() - 1)
          { Acoords.Append({x, y}); }
      }
    }

    // fwd Horizontal search
    for (auto st : Xcoords)
    {
      ASSERT(grid[st] == 'X');

      // Cardinal
      if (st.x <= grid.Width() - 4
        && grid[st.x + 1, st.y] == 'M'
        && grid[st.x + 2, st.y] == 'A'
        && grid[st.x + 3, st.y] == 'S')
        { p1++; }

      if (st.x >= 3
        && grid[st.x - 1, st.y] == 'M'
        && grid[st.x - 2, st.y] == 'A'
        && grid[st.x - 3, st.y] == 'S')
        { p1++; }

      if (st.y <= grid.Height() - 4
        && grid[st.x, st.y + 1] == 'M'
        && grid[st.x, st.y + 2] == 'A'
        && grid[st.x, st.y + 3] == 'S')
        { p1++; }

      if (st.y >= 3
        && grid[st.x, st.y - 1] == 'M'
        && grid[st.x, st.y - 2] == 'A'
        && grid[st.x, st.y - 3] == 'S')
        { p1++; }

      // Diagonal
      if (st.x <= grid.Width() - 4
        && st.y <= grid.Height() - 4
        && grid[st.x + 1, st.y + 1] == 'M'
        && grid[st.x + 2, st.y + 2] == 'A'
        && grid[st.x + 3, st.y + 3] == 'S')
        { p1++; }

      if (st.x >= 3
        && st.y >= 3
        && grid[st.x - 1, st.y - 1] == 'M'
        && grid[st.x - 2, st.y - 2] == 'A'
        && grid[st.x - 3, st.y - 3] == 'S')
        { p1++; }

      if (st.x <= grid.Width() - 4
        && st.y >= 3
        && grid[st.x + 1, st.y - 1] == 'M'
        && grid[st.x + 2, st.y - 2] == 'A'
        && grid[st.x + 3, st.y - 3] == 'S')
        { p1++; }

      if (st.x >= 3
        && st.y <= grid.Height() - 4
        && grid[st.x - 1, st.y + 1] == 'M'
        && grid[st.x - 2, st.y + 2] == 'A'
        && grid[st.x - 3, st.y + 3] == 'S')
        { p1++; }
    }

    PrintFmt("P1: {}\n", p1);

    for (auto st : Acoords)
    {
      s32 masCount = 0;
      // Search NW to SE
      if (grid[st.x - 1, st.y - 1] == 'M'
        && grid[st.x + 1, st.y + 1] == 'S')
        { masCount++; }

      if (grid[st.x - 1, st.y - 1] == 'S'
        && grid[st.x + 1, st.y + 1] == 'M')
        { masCount++; }

      // Search NE to SW
      if (grid[st.x - 1, st.y + 1] == 'M'
        && grid[st.x + 1, st.y - 1] == 'S')
        { masCount++; }

      if (grid[st.x - 1, st.y + 1] == 'S'
        && grid[st.x + 1, st.y -  1] == 'M')
        { masCount++; }

      ASSERT(masCount <= 2);
      if (masCount == 2)
        { p2++; }
    }

    PrintFmt("P2: {}\n", p2);
  }
}
