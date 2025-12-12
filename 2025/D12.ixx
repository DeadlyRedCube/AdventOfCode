module;

#include <cassert>

export module D12;

import Util;

#pragma warning (disable: 4625 4626)

export namespace D12
{
  void Run(const char *path)
  {
    struct Shape
    {
      // Array2D<char> r0{3,3};
      u32 blockCount = 0;
    };
    std::vector<Shape> shapes;

    struct Region
    {
      s32 width;
      s32 height;

      std::vector<s32> counts;
    };

    std::vector<Region> regions;
    {
      auto lines = ReadFileLines(path);
      usz i = 0;
      while (true)
      {
        auto &line = lines[i];
        if (line.contains('x'))
          { break; }

        assert(line.contains(':'));
        i++;

        auto shape =  Shape{};
        for (usz y = 0; y < 3; y++)
        {
          for (usz x = 0; x < 3; x++)
          {
            // shape.r0[x, y] = lines[i + y][x];
            if (lines[i + y][x] == '#')
              { shape.blockCount++; }
          }
        }

        shapes.push_back(std::move(shape));

        i += 4;
      }

      for (; i < lines.size(); i++)
      {
        Region r;
        auto vals = Split(lines[i], "x: ", KeepEmpty::No) | std::views::transform(AsS32) | std::ranges::to<std::vector>();

        r.width = vals[0];
        r.height = vals[1];

        vals.erase(vals.begin(), vals.begin() + 2);
        r.counts = std::move(vals);

        regions.push_back(std::move(r));
      }
    }

    // Let's see if we can determine some obvious yes/no answers
    s64 part1 = 0;
    s64 definiteNo = 0;
    for (auto &region : regions)
    {
      auto totalPieceCount = Sum(region.counts);
      auto totalBlockCount = 0;
      for (usz i = 0; i < region.counts.size(); i++)
        { totalBlockCount += region.counts[i] * shapes[i].blockCount; }

      if ((region.width / 3) * (region.height / 3) >= totalPieceCount)
      {
        part1++;
        continue;
      }
      else if (region.width * region.height - totalPieceCount * 7 < 0)
      {
        definiteNo++;
        continue;
      }

      // okay, how to do this part
    }

    //PrintFmt("Yes for {}, no for {}: total {}\n", part1, definiteNo, part1 + definiteNo);
    PrintFmt("Part 1: {}\n", part1);

    // oh okay well I guess that's that then. I parsed all those blocks for nothing lol
  }
}