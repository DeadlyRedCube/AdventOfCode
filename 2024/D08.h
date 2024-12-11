#pragma once


namespace D08
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    // Scan through the list for antennas (they're alphanumeric) and blop them into a map by type (which character it
    //  is).
    std::unordered_map<char, std::vector<Vec2S32>> antennasByType;
    for (s32 y = 0; y < grid.Height(); y++)
    {
      for (s32 x = 0; x < grid.Width(); x++)
      {
        if (std::isalnum(grid[x,y]))
          { antennasByType[grid[x,y]].push_back(Vec2{x, y}); }
      }
    }

    // Now we can scan through the anntena types to find antinodes
    std::unordered_set<Vec2S32> antinodesP1;
    std::unordered_set<Vec2S32> antinodesP2;
    for (auto [_, ants] : antennasByType)
    {
      // Nested loops was the lazy way to test every pair of antennas.
      for (s32 i = 0; i < ants.size(); i++)
      {
        for (s32 j = i + 1; j < ants.size(); j++)
        {
          const auto a = ants[i];
          const auto b = ants[j];

          // Get the delta between the two
          auto d = (b - a);

          // Moving back away from b by one delta gives us one antinode, and moving forward from b gives the other
          //  (assuming they're in range)
          if (auto p = a - d; grid.PositionInRange(p))
            { antinodesP1.insert(p); }

          if (auto p = b + d; grid.PositionInRange(p))
            { antinodesP1.insert(p); }

          // For part 2, we want to ignore any distance multiples here so divide out the GCD to get the valid step.
          d /= GreatestCommonDemoninator(d.x, d.y);

          // Starting at a, test every step along this line to find the correct antinodes.
          for (auto p = a; grid.PositionInRange(p); p -= d)
            { antinodesP2.insert(p); }

          // Do the same thing the other direction (start one off of a because a is already in the list)
          for (auto p = a + d; grid.PositionInRange(p); p += d)
            { antinodesP2.insert(p); }
        }
      }
    }

    PrintFmt("P1: {}\n", antinodesP1.size());
    PrintFmt("P2: {}\n", antinodesP2.size());
  }
}