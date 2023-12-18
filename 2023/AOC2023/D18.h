#pragma once

namespace D18
{
  struct GridSpot
  {
    Vec2S32 pos;
    u32 color;
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

      Vec2S64 pos = { 0, 0 };

      std::map<Vec2S64, u32> gridSpots;

      gridSpots[{0, 0}] = 0xFFFFFF;

      for (auto &line : lines)
      {
        auto splits = Split(line, " (#)", KeepEmpty::No);

        auto dirCh = splits[0][0];
        auto count = std::atoi(splits[1].c_str());

        u32 hex;
        {
          char *end;
          hex = std::strtol(splits[2].c_str(), &end, 16);
        }

        auto dir = directions[dirCh];
        for (s32 c = 0; c < count; c++)
        {
          pos += dir;

          gridSpots[pos] = hex;

          minCoord.x = std::min(minCoord.x, pos.x);
          minCoord.y = std::min(minCoord.y, pos.y);

          maxCoord.x = std::max(maxCoord.x, pos.x);
          maxCoord.y = std::max(maxCoord.y, pos.y);
        }
      }

      Array2D<bool> testArray { maxCoord.x - minCoord.x + 1, maxCoord.y - minCoord.y + 1 };
      testArray.Fill(false);
      for (auto &g : gridSpots)
      {
        testArray[g.first - minCoord] = true;
      }

      Array2D<bool> fillArray { maxCoord.x - minCoord.x + 3, maxCoord.y - minCoord.y + 3 };
      fillArray.Fill(false);

      UnboundedArray<Vec2S64> flood;
      flood.Append({0, 0});

      ssz floodCount = 0;
      while (!flood.IsEmpty())
      {
        auto f = flood[FromEnd(-1)];
        flood.RemoveAt(flood.Count() - 1);

        if (f.x >= 0 && f.x < fillArray.Width() && f.y >= 0 && f.y < fillArray.Height())
        {
          if (fillArray[f])
            { continue; }

          if (gridSpots.contains(f + minCoord - Vec2S64(1, 1)))
            { continue; }

          fillArray[f] = true;
          floodCount++;
          flood.Append(f + Vec2S64({1, 0}));
          flood.Append(f + Vec2S64({-1, 0}));
          flood.Append(f + Vec2S64({0, 1}));
          flood.Append(f + Vec2S64({0, -1}));
        }
      }

      PrintFmt("Part 1: {}\n", fillArray.Width() * fillArray.Height() - floodCount);
    }



    // Part 2!
    {
      struct Inst
      {
        char dir;
        ssz count;
      };

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

      // Find the grid extents
      Vec2S64 minCoord { 0, 0 };
      Vec2S64 maxCoord { 0, 0 };

      Vec2S64 pos = { 0, 0 };

      UnboundedArray<s64> xCoords;
      UnboundedArray<s64> yCoords;
      for (auto &inst : insts)
      {
        pos += directions[inst.dir] * inst.count;
        minCoord.x = std::min(minCoord.x, pos.x);
        minCoord.y = std::min(minCoord.y, pos.y);

        maxCoord.x = std::max(maxCoord.x, pos.x);
        maxCoord.y = std::max(maxCoord.y, pos.y);

        if (!xCoords.Contains(pos.x))
          { xCoords.Append(pos.x); }

        if (!yCoords.Contains(pos.y))
          { yCoords.Append(pos.y); }
      }

      std::ranges::sort(xCoords);
      std::ranges::sort(yCoords);

      xCoords.Insert(0, minCoord.x - 1);
      yCoords.Insert(0, minCoord.y - 1);
      std::map<s64, s64> xCoordToGridSpace;
      std::map<s64, s64> yCoordToGridSpace;
      for (auto i = 0; i < xCoords.Count(); i++)
        { xCoordToGridSpace[xCoords[i]] = i; }

      for (auto i = 0; i < yCoords.Count(); i++)
        { yCoordToGridSpace[yCoords[i]] = i; }

      // Okay now build an actual wall grid
      Vec2S64 worldPos = { 0, 0 };
      Vec2S64 gridPos = { xCoordToGridSpace[0], yCoordToGridSpace[0] };

      enum class Flags
      {
        R = 0x01,
        D = 0x02,
        Flooded = 0x04,
        Corner = 0x08,

        None = 0,

        BothDirs = R | D,
        All = R | D | Flooded | Corner,
      };

      Array2D<Flags> squishGrid { xCoords.Count(), yCoords.Count() };
      squishGrid.Fill(Flags::None);

      squishGrid[gridPos]  = Flags::Corner;
      for (auto &inst : insts)
      {
        auto dir = directions[inst.dir];
        worldPos += dir * inst.count;
        Vec2S64 nextGridPos = { xCoordToGridSpace[worldPos.x], yCoordToGridSpace[worldPos.y] };
        squishGrid[nextGridPos] |= Flags::Corner;
        Flags newExit = (inst.dir == 'R' || inst.dir == 'L') ? Flags::R : Flags::D;
        if (inst.dir == 'R' || inst.dir == 'D')
          { squishGrid[gridPos] |= newExit; }

        while (true)
        {
          gridPos += dir;
          if ((inst.dir == 'R' || inst.dir == 'D') &&  gridPos == nextGridPos)
            { break; }

          squishGrid[gridPos] |= newExit;

          if (gridPos == nextGridPos)
            { break; }
        }
      }

      UnboundedArray<Vec2S64> flood;
      flood.Append({squishGrid.Width() - 1, squishGrid.Height() - 1});

      while (!flood.IsEmpty())
      {
        auto f = flood[FromEnd(-1)];
        flood.RemoveAt(flood.Count() - 1);

        if ((squishGrid[f] & Flags::Flooded) != Flags::None)
          { continue; }

        squishGrid[f] |= Flags::Flooded;

        if (f.x < squishGrid.Width() - 1 && (squishGrid[f + Vec2S64(1, 0)] & Flags::D) == Flags::None)
          { flood.Append(f + Vec2S64(1, 0)); }

        if (f.y < squishGrid.Height() - 1 && (squishGrid[f + Vec2S64(0, 1)] & Flags::R) == Flags::None)
          { flood.Append(f + Vec2S64(0, 1)); }

        if (f.x > 0 && (squishGrid[f] & Flags::D) == Flags::None)
          { flood.Append(f + Vec2S64(-1, 0)); }

        if (f.y > 0 && (squishGrid[f] & Flags::R) == Flags::None)
          { flood.Append(f + Vec2S64(0, -1)); }
      }

      // Now figure out the flood count
      s64 fillCount = (maxCoord.x - minCoord.x + 1) * (maxCoord.y - minCoord.y + 1);
      for (s64 y = 1; y < squishGrid.Height(); y++)
      {
        auto gridH = (y == squishGrid.Height() - 1) ? 1 : yCoords[y + 1] - yCoords[y];
        for (s64 x = 1; x < squishGrid.Width(); x++)
        {
          auto gridW = (x == squishGrid.Width() - 1) ? 1 : xCoords[x + 1] - xCoords[x];
          if ((squishGrid[Vec2S64{x, y}] & Flags::Flooded) != Flags::None)
          {
            fillCount -= gridW * gridH;
            if ((squishGrid[Vec2S64{x, y}] & Flags::BothDirs) == Flags::BothDirs)
              { fillCount += gridW + gridH - 1; }
            else if ((squishGrid[Vec2S64{x, y}] & Flags::R) == Flags::R)
              { fillCount += gridW; }
            else if ((squishGrid[Vec2S64{x, y}] & Flags::D) == Flags::D)
              { fillCount += gridH; }
            else if ((squishGrid[Vec2S64{x, y}] & Flags::Corner) == Flags::Corner)
              { fillCount++; }
          }
        }
      }

      PrintFmt("Part 2: {}\n", fillCount);
    }
  }
}
