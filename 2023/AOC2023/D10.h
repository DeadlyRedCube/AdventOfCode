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

    PrintFmt("Part 1: {}\n", maxDist);

    // Part 2

    // Ray cast algorithm!
    s32 containedCount = 0;
    for (auto y = 0; y < charGrid.Height(); y++)
    {
      bool in = false;

      // We're going to track from the left side (starting "not in" and look for North exits).
      //  - Any north exit we see is going to flip our "in" value:
      //     Out | In | Out | In
      //     Out L In 7 In J Out
      //  - Any empty square we hit when we're "in" counts as a contained point.
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        auto e = graph.Idx(x, y);
        if (distances.Idx(x, y) == std::numeric_limits<s32>::max())
        {
          // If it has no set distance value, it was not on the path, so we can treat it as empty and check whether
          //  it is contained or not.
          if (in)
            { containedCount++; }
        }
        else if ((e & Exits::N) != Exits::None)
        {
          // This entry WAS on the path and has a north exit, so flip our "in" flag
          in = !in;
        }
      }
    }

    PrintFmt("Part 2: {}\n", containedCount);
  }
}