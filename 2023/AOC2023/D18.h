#pragma once

namespace D18
{
  struct GridSpot
  {
    Vec2S32 pos;
  };


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    std::map<char, Vec2S64> directions =
    {
      {'R', { 1, 0 } },
      {'L', { -1, 0 } },
      {'D', { 0, 1 } },
      {'U', { 0, -1 } },
    };

    // Part 1!
    {
      Vec2S64 minCoord { 0, 0 };
      Vec2S64 maxCoord { 0, 0 };


      // Make a set of all the grid spots we encounter
      std::set<Vec2S64> gridSpots;
      gridSpots.insert({0, 0});

      // Track our world position
      Vec2S64 pos = { 0, 0 };
      for (auto &line : lines)
      {
        auto splits = Split(line, " ", KeepEmpty::No);

        // Turns out we can ignore the RGB codes - but know that I tracked them in the original Part 1 implementation
        auto dirCh = splits[0][0];
        auto count = std::atoi(splits[1].c_str());

        // Add grid coordinates along the entire movement span to the next spot
        auto dir = directions[dirCh];
        for (s32 c = 0; c < count; c++)
        {
          pos += dir;

          gridSpots.insert(pos);

          minCoord.x = std::min(minCoord.x, pos.x);
          minCoord.y = std::min(minCoord.y, pos.y);

          maxCoord.x = std::max(maxCoord.x, pos.x);
          maxCoord.y = std::max(maxCoord.y, pos.y);
        }
      }

      // Now time to flood fill - make an array that's extended 1 in each direction from our grid (so there's a ring
      //  of empty space around it)
      Array2D<bool> fillArray { maxCoord.x - minCoord.x + 3, maxCoord.y - minCoord.y + 3 };
      fillArray.Fill(false);

      // We'll start flooding in the upper-left corner
      UnboundedArray<Vec2S64> flood;
      flood.Append({0, 0});

      ssz count = fillArray.Width() * fillArray.Height();
      while (!flood.IsEmpty())
      {
        auto f = flood[FromEnd(-1)];
        flood.RemoveAt(flood.Count() - 1);

        if (f.x >= 0 && f.x < fillArray.Width() && f.y >= 0 && f.y < fillArray.Height())
        {
          // If it's already filled, we're done
          if (fillArray[f])
            { continue; }

          // Also we can't flood fill into a grid spot
          if (gridSpots.contains(f + minCoord - Vec2S64(1, 1)))
            { continue; }

          // Fill in and prepare to flood all the neighbors
          fillArray[f] = true;
          flood.Append(f + Vec2S64({1, 0}));
          flood.Append(f + Vec2S64({-1, 0}));
          flood.Append(f + Vec2S64({0, 1}));
          flood.Append(f + Vec2S64({0, -1}));

          // We flooded a new space, so one more square is removed from the polygon count
          count--;
        }
      }

      PrintFmt("Part 1: {}\n", count);
    }



    // Part 2!
    {
      struct Inst
      {
        char dir;
        ssz count;
      };

      // New rule: Parse the "color" as an instruction, lol
      UnboundedArray<Inst> insts;
      char dirMap[] = { 'R', 'D', 'L', 'U' };
      for (auto &line : lines)
      {
        auto splits = Split(line, " #()", KeepEmpty::No);
        auto instStr = splits[2];

        auto &inst = insts.AppendNew();

        inst.dir = dirMap[instStr[instStr.length() - 1] - '0'];

        {
          char *end;
          inst.count = std::strtoll(instStr.substr(0, instStr.length() - 1).c_str(), &end, 16);
        }
      }

      // Do a quick run through the instructions to find the min and max extents of our movement. Also get a list of
      //  every unique x and y coordinate.
      Vec2S64 minCoord { 0, 0 };
      Vec2S64 maxCoord { 0, 0 };

      UnboundedArray<s64> xCoords;
      UnboundedArray<s64> yCoords;

      {
        Vec2S64 pos = { 0, 0 };
        for (auto &inst : insts)
        {
          pos += directions[inst.dir] * inst.count;
          minCoord.x = std::min(minCoord.x, pos.x);
          minCoord.y = std::min(minCoord.y, pos.y);

          maxCoord.x = std::max(maxCoord.x, pos.x);
          maxCoord.y = std::max(maxCoord.y, pos.y);

          // Just do this as a linear search, because I expect the unique space to be reasonably small.
          if (!xCoords.Contains(pos.x))
            { xCoords.Append(pos.x); }

          if (!yCoords.Contains(pos.y))
            { yCoords.Append(pos.y); }
        }
      }

      // Append a buffer coordinate just to the top and left of our grid (to allow us to flood fill around the edges)
      //  and then sort each array by coordinate space (so we can turn this into a smaller, manageable grid)
      xCoords.Append(minCoord.x - 1);
      yCoords.Append(minCoord.y - 1);
      std::ranges::sort(xCoords);
      std::ranges::sort(yCoords);

      // Make a mapping from endpoint x and y coordinates to this new space
      std::map<s64, s64> xCoordToGridSpace;
      std::map<s64, s64> yCoordToGridSpace;
      for (auto i = 0; i < xCoords.Count(); i++)
        { xCoordToGridSpace[xCoords[i]] = i; }

      for (auto i = 0; i < yCoords.Count(); i++)
        { yCoordToGridSpace[yCoords[i]] = i; }

      enum class Flags
      {
        Corner = 0x01,  // This space has a corner in the top-left (always the top left)
        R = 0x02,       // This space has an edge from top-left to top-right (Right-leaving edge)
        D = 0x04,       // This space has an edge from top-left to bottom-left (Down-leaving edge)
        Flooded = 0x08, // This space has been hit by the flood fill

        None = 0,
        BothDirs = R | D,
        All = R | D | Flooded | Corner,
      };

      // Build a "squish grid" where each grid space represents some rectangular section of the full-sized coordinate
      //  space.
      Array2D<Flags> squishGrid { xCoords.Count(), yCoords.Count() };
      squishGrid.Fill(Flags::None);

      // Now run through our instructions a second time, this time keeping track of both our full world position as
      //  well as the squished-coordinate-system position.
      Vec2S64 worldPos = { 0, 0 };
      Vec2S64 gridPos = { xCoordToGridSpace[0], yCoordToGridSpace[0] };
      squishGrid[gridPos]  = Flags::Corner;
      for (auto &inst : insts)
      {
        // Update our world position to where we're traveling to.
        auto dir = directions[inst.dir];
        worldPos += dir * inst.count;

        // Our target grid position is the world-to-squish mapping of that target point.
        Vec2S64 nextGridPos = { xCoordToGridSpace[worldPos.x], yCoordToGridSpace[worldPos.y] };

        // This space has at least a corner in it (we've entered it at all, at the top left)
        squishGrid[nextGridPos] |= Flags::Corner;

        // We're either dealing with R edges or D edges
        Flags newExit = (inst.dir == 'R' || inst.dir == 'L') ? Flags::R : Flags::D;

        // If the exit leaves this square through the Right or bottom (down) add this edge.
        if (inst.dir == 'R' || inst.dir == 'D')
          { squishGrid[gridPos] |= newExit; }

        while (true)
        {
          gridPos += dir;

          // If we are going to the right or down, we're done once we reach our final position (the edge doesn't
          //  continue past the top-left of the target)
          if ((inst.dir == 'R' || inst.dir == 'D') &&  gridPos == nextGridPos)
            { break; }

          // Add the edge to this square
          squishGrid[gridPos] |= newExit;

          if (gridPos == nextGridPos)
            { break; }
        }
      }

      // Time to flood fill! This time we can flood fill starting in the bottom right because at worst it's a corner
      //  (with no R or D edges) so we'll 100% be able to fill out of this
      UnboundedArray<Vec2S64> floodStack;
      floodStack.Append({squishGrid.Width() - 1, squishGrid.Height() - 1});

      while (!floodStack.IsEmpty())
      {
        auto f = floodStack[FromEnd(-1)];
        floodStack.RemoveAt(floodStack.Count() - 1);

        // If we already flooded this space we don't have to do it again.
        if ((squishGrid[f] & Flags::Flooded) != Flags::None)
          { continue; }

        squishGrid[f] |= Flags::Flooded;

        // Flood to the right if we're not at the right edge and the square to our right doesn't have a D edge
        //  (which would block filling into it from the left)
        if (f.x < squishGrid.Width() - 1 && (squishGrid[f + Vec2S64(1, 0)] & Flags::D) == Flags::None)
          { floodStack.Append(f + Vec2S64(1, 0)); }

        // Flood downward if we're not at the bottom edge and the square below us doesn't have a R edge (which would
        //  block filling into it from above)
        if (f.y < squishGrid.Height() - 1 && (squishGrid[f + Vec2S64(0, 1)] & Flags::R) == Flags::None)
          { floodStack.Append(f + Vec2S64(0, 1)); }

        // If we are not on the left edge and we don't have a D edge (Which would stop filling to the left), flood
        //  to the left.
        if (f.x > 0 && (squishGrid[f] & Flags::D) == Flags::None)
          { floodStack.Append(f + Vec2S64(-1, 0)); }

        // Similarly, if we're not on top edge and have no R edge (blocking filling up), flood up.
        if (f.y > 0 && (squishGrid[f] & Flags::R) == Flags::None)
          { floodStack.Append(f + Vec2S64(0, -1)); }
      }

      // Now we'll count up how many parts of the grid are dug out. Assume that the entire grid is dug out (note that
      //  we'll skip the first row/column of our grid because we added that extra row/column before to allow flood
      //  filling to go around the whole thing)
      s64 fillCount = (maxCoord.x - minCoord.x + 1) * (maxCoord.y - minCoord.y + 1);
      for (s64 y = 1; y < squishGrid.Height(); y++)
      {
        // Get the height of this grid square (if we're at the bottom, the height is 1)
        auto gridH = (y == squishGrid.Height() - 1) ? 1 : yCoords[y + 1] - yCoords[y];
        for (s64 x = 1; x < squishGrid.Width(); x++)
        {
          // Get the width of the grid square (at the right, it's 1)
          auto gridW = (x == squishGrid.Width() - 1) ? 1 : xCoords[x + 1] - xCoords[x];

          if ((squishGrid[Vec2S64{x, y}] & Flags::Flooded) != Flags::None)
          {
            // The flood fill touched this grid so remove its area from the dig count (we'll add some adjustements back
            //  in later)
            fillCount -= gridW * gridH;

            // If this has both R and D edges, we need to add back the width and height (since there's an edge along
            //  both the top and left sides), but don't count the top corner twice.
            if ((squishGrid[Vec2S64{x, y}] & Flags::BothDirs) == Flags::BothDirs)
              { fillCount += gridW + gridH - 1; }

            // Otherwise, if we have just an R edge, add the width since the top row is dug out.
            else if ((squishGrid[Vec2S64{x, y}] & Flags::R) == Flags::R)
              { fillCount += gridW; }

            // If we have just a D edge, add back the height since the left row is dug out.
            else if ((squishGrid[Vec2S64{x, y}] & Flags::D) == Flags::D)
              { fillCount += gridH; }

            // If we have just a corner, we only touched this square in its top left so only one square needs to be
            //  added back.
            else if ((squishGrid[Vec2S64{x, y}] & Flags::Corner) == Flags::Corner)
              { fillCount++; }
          }
        }
      }

      PrintFmt("Part 2: {}\n", fillCount);
    }
  }
}
