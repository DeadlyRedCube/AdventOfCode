#pragma once

namespace D18
{
  void Run(const char *path)
  {
    std::map<char, Vec2S64> directions = { {'R', { 1, 0 } }, {'L', { -1, 0 } }, {'D', { 0, 1 } }, {'U', { 0, -1 } } };

    // Merged parts 1 and 2 because the part 2 solution was faster than part 1, and the only actual difference is the
    //  instruction set.
    char dirMap[] = { 'R', 'D', 'L', 'U' };
    UnboundedArray<Vec2S64> polygons[2] {};
    auto lines = ReadFileLines(path);
    polygons[0].EnsureCapacity(ssz(lines.size()));
    polygons[1].EnsureCapacity(ssz(lines.size()));
    {
      Vec2S64 positions[2] {};
      for (auto &line : lines)
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
          polygons[i].Append(positions[i]);
          positions[i] += directions[dirs[i]] * counts[i];
        }
      }
    }

    for (s32 i = 0; i < 2; i++)
      { PrintFmt("Part {}: {}\n", i + 1, GridPolygonAreaIncludingPerimeter(polygons[i])); }
  }
}
