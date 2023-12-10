#pragma once

namespace D10
{
  void Run(const char *path)
  {
    auto charGrid = ReadFileAsCharArray(path);

    enum class Exits
    {
      N = 1,
      S = 2,
      E = 4,
      W = 8,

      None = 0,
      All = N | S | E | W,
    };

    Vec2S32 entryPoint;
    Array2D<Exits> graph { charGrid.Width(), charGrid.Height() };
    graph.Fill(Exits::None);

    for (auto y = 0; y < charGrid.Height(); y++)
    {
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        switch (charGrid.Idx(x, y))
        {
        case '|': graph.Idx(x, y) = Exits::N | Exits::S; break;
        case '-': graph.Idx(x, y) = Exits::E | Exits::W; break;
        case 'L': graph.Idx(x, y) = Exits::N | Exits::E; break;
        case 'J': graph.Idx(x, y) = Exits::N | Exits::W; break;
        case '7': graph.Idx(x, y) = Exits::S | Exits::W; break;
        case 'F': graph.Idx(x, y) = Exits::S | Exits::E; break;
        case '.': break;
        case 'S': entryPoint = { x, y }; break; // We don't know what exits are here yet but we can mark it for later.
        }

        // Remove any exits that point off the map.
        if (x == 0)
          { graph.Idx(x, y) &= ~Exits::W; }

        if (x == charGrid.Width() - 1)
          { graph.Idx(x, y) &= ~Exits::E; }

        if (y == 0)
          { graph.Idx(x, y) &= ~Exits::N; }

        if (y == charGrid.Height() - 1)
          { graph.Idx(x, y) &= ~Exits::S; }
      }
    }

    constexpr Exits oppositeDirections[] = { Exits::S, Exits::N, Exits::W, Exits::E };
    constexpr Vec2S32 neighborDirections[] = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };

    // Figure out what the entrances to the starting point are.
    for (s32 dirIndex = 0; dirIndex < 4; dirIndex++)
    {
      Exits dir = Exits(1 << dirIndex);
      Vec2S32 n = entryPoint + neighborDirections[dirIndex];

      // If the neighbor in this direction points back to us, add an exit in that direction.
      if ((graph.Idx(n.x, n.y) & oppositeDirections[dirIndex]) != Exits::None)
        { graph.Idx(entryPoint.x, entryPoint.y) |= dir; }
    }

    auto maxDist = 0;
    {
      // Now we're going to do the flood fill path finding thingy.
      Array2D<s32> distances { charGrid.Width(), charGrid.Height() };
      distances.Fill(std::numeric_limits<s32>::max());

      // Distance to the entry point is 0.
      distances.Idx(entryPoint.x, entryPoint.y) = 0;
      UnboundedArray<Vec2S32> positions;

      // Start at the entry
      positions.Append(entryPoint);

      while (!positions.IsEmpty())
      {
        auto p = positions[0];
        positions.RemoveAt(0);

        auto exits = graph.Idx(p.x, p.y);
        auto neighboringDist = distances.Idx(p.x, p.y) + 1;

        // Update the distance in each direction from this point and, if we found a closer entry, add it to the list.
        for (s32 dirIndex = 0; dirIndex < 4; dirIndex++)
        {
          Exits dir = Exits(1 << dirIndex);
          if ((exits & dir) != Exits::None)
          {
            Vec2S32 n = p + neighborDirections[dirIndex];
            if (neighboringDist < distances.Idx(n.x, n.y))
            {
              distances.Idx(n.x, n.y) = neighboringDist;
              positions.Append(n);
              maxDist = std::max(maxDist, neighboringDist);
            }
          }
        }
      }
    }

    PrintFmt("Part 1: {}\n", maxDist);

    // Part 2

    // First thing we need to do is clear out the edges that are not part of the loop to make the search faster.
    //  Could have done a more efficient thing here than just "loop until we don't change anything" but I got lazy.
    UnboundedArray<Vec2S32> deadEnds;
    bool keepRunning = true;
    while (keepRunning)
    {
      // Assume that we aren't going to change anything on this loop.
      keepRunning = false;

      for (auto y = 0; y < charGrid.Height(); y++)
      {
        for (auto x = 0; x < charGrid.Width(); x++)
        {
          Vec2S32 p = {x, y};

          auto &exits = graph.Idx(p.x, p.y);
          if (exits == Exits::None)
            { continue; }

          // Remove any exits where the neighbor doesn't point back.
          for (s32 dirIndex = 0; dirIndex < 4; dirIndex++)
          {
            Exits dir = Exits(1 << dirIndex);
            if ((exits & dir) != Exits::None)
            {
              Vec2S32 n = p + neighborDirections[dirIndex];
              if ((graph.Idx(n.x, n.y) & oppositeDirections[dirIndex]) == Exits::None)
                { exits = Exits(exits & ~dir); }
            }
          }

          // If we no longer have 2 exits, clear our exits and note that we should keep going after this pass through
          //  the grid.
          if (CountSetBits(s32(exits)) < 2)
          {
            exits = Exits::None;
            keepRunning = true;
          }
        }
      }
    }

    // Ray cast algorithm!
    // We're going to track to the left from each point and count how many North/South crossings there are. If there
    //  is an odd number, we're contained within the loop! if there's an odd number (including 0), we are outside the
    //  loop (think about this as determining if a point is in a circle: if you draw a ray in some direction from the
    //  point, if it's in the circle it's guaranteed to intersect the circle once, if it's not, it'll either intersect
    //  0 times or 2 times (excluding glancing angles which we don't exactly have to care about in our grid-based case)
    s32 containedCount = 0;

    for (auto y = 0; y < charGrid.Height(); y++)
    {
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        if (graph.Idx(x, y) != Exits::None)
          { continue; }

        // Track both the count of north and south turns. that we hit.
        //  Take this example from the puzzle (A and B points added):
        //   ...........
        //   .S-------7A
        //   .|F-----7|.
        //   .||.....||.
        //   .||.....||.
        //   .|L-7BF-J|.
        //   .|..|.|..|.
        //   .L--J.L--J.
        //   ...........
        // For point A, as we trace from A to the left, it passes a South, then eventually another South, but we never
        //  encounter a north, so this should be considered outside of the loop.
        //
        // For point B, tracing to the left we hit a South (at the 7), then a North (At the L) then a North/South at
        //  the |. That means we passed two Norths and two Souths (even though there were three spots that had a
        //  north or south), and this counts as outside the loop.
        s32 nCount = 0;
        s32 sCount = 0;
        for (auto i = 0; i < x; i++)
        {
          if ((graph.Idx(i, y) & Exits::N) != Exits::None)
            { nCount++; }

          if ((graph.Idx(i, y) & Exits::S) != Exits::None)
            { sCount++; }
        }

        // In practice, we can look at the minimum of N or S and do the "is this odd" check on that value to determine
        //  containment, since the minimum one is the number of actually paired crossings we had.
        if ((std::min(nCount, sCount) & 1) != 0)
          { containedCount++; }
      }
    }

    PrintFmt("Part 2: {}\n", containedCount);
  }
}