module;

#include <cassert>

export module D10;

import Util;

export namespace D10
{
  void Run(const char *path)
  {
    u64 part1 = 0;
    u64 part2 = 0;
    for (auto line : ReadFileLines(path))
    {
      u64 target = 0;

      auto sets = Split(line, "()[] {}", KeepEmpty::No);

      for (usz i = sets[0].size() - 1; i < sets[0].size(); i--)
      {
        target <<= 1;
        if (sets[0][i] == '#')
          { target |= 1; }
      }

      auto targetJolts = Split(sets.back(), ",", KeepEmpty::No) | std::views::transform(AsS32) | std::ranges::to<std::vector>();


      std::vector<u64> buttons;
      for (usz i = 1; i < sets.size() - 1; i++)
      {
        char *end;
        u32 button = 0;
        {
          auto v = std::strtoll(sets[i].c_str(), &end, 10);
          button |= (1u << v);
        }

        while (*end != '\0')
        {
          auto v = std::strtoll(end + 1, &end, 10);
          button |= (1u << v);
        }

        buttons.push_back(button);
      }

      u32 minCount = std::numeric_limits<u32>::max();
      for (u32 i = 0; i < (1u << u32(buttons.size())); i++)
      {
        u64 r = 0;
        for (u32 j = 0; j < u32(buttons.size()); j++)
        {
          if ((1ul << j) & i)
            { r ^= buttons[j]; }
        }

        if (r == target)
          { minCount = std::min(minCount, u32(std::popcount(i))); }
      }

      part1 += minCount;
    }

    PrintFmt("Part 1: {}\n", part1);
    PrintFmt("Part 2: {}\n", part2);
  }
}