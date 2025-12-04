export module D04;

import Util;

export namespace D04
{
  void Run(const char *path)
  {
    auto gridFromFile = ReadFileAsCharArray2D(path);

    // Empty is a value that is large enough that it cannot possibly be decremented to 4.
    const s32 Empty = s32(gridFromFile.Width() + gridFromFile.Height() + 5);

    // Preprocess the grid. Any grid space that is an '@' will get filled with the number of '@' neighbors it has (not
    //  counting itself).  Any other grid space gets the Empty value (a large value that will stay large enough to
    //  never matter later)
    auto grid = Array2D<s32>{gridFromFile.Width(), gridFromFile.Height()};
    std::vector<Vec2<s32>> removalStack;
    gridFromFile.ForEach<s32>(
      [&](auto x, auto y, auto &src)
      {
        auto &dst = grid[x, y];
        dst = ((src == '@') ? -1 : Empty); // Start rolls at -1 because we're going to count them in the 3x3

        // Now count all the neighboring rolls (including itself, which will be canceled out due to the -1 above)
        gridFromFile.ForEachIn3x3(x, y, [&](auto, auto, auto neighbor) { dst += neighbor == '@'; });

        // If it's a under the threshold, queue it for "removal"
        if (dst < 4) { removalStack.push_back({x, y}); }
      });

    // Part 1 is the initial count of values to remove.
    u64 part1 = removalStack.size();
    u64 part2 = part1;
    while (!removalStack.empty())
    {
      // Pop the next element from the list and count it as a part 2 removal
      const auto remCoord = removalStack.back();
      removalStack.pop_back();

      // Check each valid neighbor of the given coordinate to see if we cause any more to remove.
      grid.ForEachIn3x3(
        remCoord,
        [&](auto x, auto y, auto &space)
        {
          // If we decrement it from above the threshold to below, it is a new space to remove.
          if (--space == 3)
          {
            removalStack.push_back({s32(x), s32(y)});
            part2++;
          }
        });
    }

    PrintFmt("Part 1: {}\n", part1);
    PrintFmt("Part 2: {}\n", part2);
  }
}