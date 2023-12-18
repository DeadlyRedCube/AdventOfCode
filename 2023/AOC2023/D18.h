#pragma once

namespace D18
{
  void Run(const char *path)
  {
    std::map<char, Vec2S64> directions = { {'R', { 1, 0 } }, {'L', { -1, 0 } }, {'D', { 0, 1 } }, {'U', { 0, -1 } } };

    // Merged parts 1 and 2 because the part 2 solution was faster than part 1, and the only actual difference is the
    //  instruction set.
    char dirMap[] = { 'R', 'D', 'L', 'U' };
    s64 shoelaces[2] {}; // Gonna use shoelace formula to get the area of this polygon!
    s64 perimeters[2] {};
    Vec2S64 positions[2] {};
    for (auto &line : ReadFileLines(path))
    {
      char dirs[2];
      s64 counts[2];
      auto splits = Split(line, " #()", KeepEmpty::No);

      // Part 1 instruction: parse the first two parameters.
      dirs[0] = splits[0][0];
      counts[0] = std::atoi(splits[1].c_str());

      // Part 2 instruction: parse the hex code per the instructions
      dirs[1] = dirMap[splits[2][splits[2].length() - 1] - '0'];
      {
        char *end;
        counts[1] = std::strtoll(splits[2].substr(0, splits[2].length() - 1).c_str(), &end, 16);
      }

      for (s32 i = 0; i < 2; i++)
      {
        Vec2S64 next = positions[i] + directions[dirs[i]] * counts[i];
        shoelaces[i] += (next.x + positions[i].x) * (next.y - positions[i].y);
        perimeters[i] += counts[i];
        positions[i] = next;
      }
    }

    for (s32 i = 0; i < 2; i++)
    {
      // The are from shoelace formula is abs(shoelace) / 2
      // Okay so, Pick's Theorem states that: area = interiorPointCount + perimeter/2 - 1
      // Our full grid-space solution, however, is: gridArea = interiorPointCount + perimeter
      // If you solve Pick's Theorem for interiorPointCount and then solve for gridArea you get:
      auto gridArea = std::abs(shoelaces[i]) / 2 + perimeters[i] / 2 + 1;
      PrintFmt("Part {}: {}\n", i + 1, gridArea);
    }
  }
}
