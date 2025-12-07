export module D07;

import Util;

export namespace D07
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray2D(path);

    // For part 2 we need a grid of "how many paths have managed to get to here"
    auto countGrid = Array2D<s64>{ grid.Width(), grid.Height() };

    // Find the starting position and note that there is exactly one way to get to it.
    auto start = Vec2{s32(grid.GetRowAsStringView(0).find('S')), 0};
    countGrid[start] = 1;

    s64 part1 = 0;
    s64 part2 = 0;

    // Now we're going to trace the beams from top to bottom (finishing a row before moving on to the next)
    std::queue<Vec2<s32>> beams;
    beams.push(start);
    std::vector<Vec2<s32>> potentials;
    potentials.reserve(2);
    while (!beams.empty())
    {
      auto beam = beams.front();
      beams.pop();

      auto next = beam + Vec2{0, 2}; // Every other line is empty so just skip a line
      if (next.y >= grid.Height())
      {
        // This beam goes off the bottom. Tally its total number of possibility counts into part 2
        part2 += countGrid[beam];
        continue;
      }

      potentials.clear();
      if (grid[next] == '^')
      {
        // This is a splitter, so note for part 1 that it split
        part1++;
        potentials.push_back(next - Vec2{1, 0});
        potentials.push_back(next + Vec2{1, 0});
      }
      else
        { potentials.push_back(next); }

      for (auto potential : potentials)
      {
        if (countGrid[potential] == 0)
          { beams.push(potential); }
        countGrid[potential] += countGrid[beam];
      }
    }

    PrintFmt("Part1: {}\n", part1);
    PrintFmt("Part2: {}\n", part2);
  }
}