#pragma once


namespace D18
{
  using Vec = Vec2<s32>;

  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();

  void Run(const char *filePath)
  {
    std::vector<Vec> bads = ReadFileLines(filePath)
      | std::views::transform([](const auto &line)
        {
          return (Split(line, ", ", KeepEmpty::No)
            | std::views::adjacent_transform<2>([](auto &x, auto &y) { return Vec{AsS32(x), AsS32(y)}; }))[0];
        })
      | std::ranges::to<std::vector<Vec>>();

    const s32 Size = (bads.size() == 25) ? 7 : 71; // Choose grid size based on whether it's the sample or not.
    auto grid = Array2D<char> { Size, Size };

    auto AddBads =
      [&](size_t count)
      {
        grid.Fill('.');
        for (auto i = 0; i < count; i++)
          { grid[bads[i]] = '#'; }
      };
    auto FindMinDist =
      [&]
      {
        // Start with our starting space as an 'O'
        grid[0,0] = 'O';

        struct Walk { Vec pos; s32 dist = 0; };

        // Starting in the upper left, BFS until we hit the lower right.
        std::queue<Walk> queue;
        queue.push({ .pos = {} });
        while (!queue.empty())
        {
          auto w = queue.front();
          queue.pop();

          for (auto d : { X, Y, X, -X, -Y })
          {
            auto n = w.pos + d;
            if (grid[OOBZero{n}] == '.')
            {
              if (n == Vec{ Size - 1 })
                { return w.dist + 1; } // Found the exit, we're done.

              // Mark this spot as visited and push it into the queue.
              grid[n] = 'O';
              queue.push({ .pos = n, .dist = w.dist + 1 });
            }
          }
        }

        // Unable to find a path to the exit.
        return std::numeric_limits<s32>::max();
      };


    // For P1 simulate the first N and find the minimum distance.
    const s32 SimCount = (bads.size() == 25) ? 12 : 1024;
    AddBads(SimCount);
    PrintFmt("P1: {}\n", FindMinDist());

    // For P2, binary search for the first blocked entry (here basicaly as a projection to false for "can exit" and
    // true for "is blocked")
    auto p2 = *std::ranges::lower_bound(
      std::span{bads.begin() + SimCount, bads.end()}, // Not that it makes much difference but don't have the first 1k
      true,                                           //  entries in the search space.
      std::less{},
      [&](const Vec &v)
      {
        // Get the index from this value in the vector
        size_t index = size_t(&v - &bads[0]);

        // Fill the grid up to this point (the parameter is count and we want to include bads[index] so add 1.
        AddBads(index + 1);

        // Return true when we are blocked and false when not.
        return (FindMinDist() == std::numeric_limits<s32>::max());
      });

    PrintFmt("P2: {},{}\n", p2.x, p2.y);
  }
}