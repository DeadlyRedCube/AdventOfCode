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

    Vec2S64 minCoord { 0, 0 };
    Vec2S64 maxCoord { 0, 0 };

    Vec2S64 pos = { 0, 0 };

    std::map<Vec2S64, u32> gridSpots;
    std::map<char, Vec2S64> directions =
    {
      {'R', { 1, 0 } },
      {'L', { -1, 0 } },
      {'D', { 0, 1 } },
      {'U', { 0, -1 } },
    };

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

    FILE *f;
    fopen_s(&f, "FUCK.txt", "w");
    for (auto y = 0; y < testArray.Height(); y++)
    {
      for (auto x = 0; x < testArray.Width(); x++)
      {
        Vec2S64 p = { x, y };
        bool isWall = testArray[p];
        bool isFilled = fillArray[p + Vec2S64(1, 1)];

        fprintf_s(f, "%c",
          isWall
            ? (isFilled ? '&' : '#')
            : (isFilled ? '*' : '.'));
      }

      fprintf_s(f, "\n");
    }

    fclose(f);

    PrintFmt("{}\n", fillArray.Width() * fillArray.Height() - floodCount);
  }
}
