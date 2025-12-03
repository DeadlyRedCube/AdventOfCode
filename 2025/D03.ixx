export module D03;

import Util;

export namespace D03
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray2D(path);

    auto Solve =
      [&](s32 digitCount)
      {
        u64 result = 0;
        for (s32 y = 0; y < grid.Height(); y++)
        {
          s32 startX = 0;
          u64 value = 0;

          for (s32 remainingDigits = digitCount - 1; remainingDigits >= 0; remainingDigits--)
          {
            // Stop checking when we wouldn't have enough digits remaining to the right.
            s32 maxX = s32(grid.Width() - remainingDigits);

            // Find the max value in range (and its index so we know where to start from next)
            // Loop until we reach maxX or
            char maxFirst = grid[startX, y];
            for (s32 x = startX + 1; x < maxX && maxFirst != '9'; x++)
            {
              if (grid[x, y] > maxFirst)
              {
                startX = x;
                maxFirst = grid[x, y];
              }
            }

            // Add our digit to the total value for this row.
            value = (value * 10 + (maxFirst - '0'));
            startX++;
          }

          result += value;
        }

        return result;
      };

    PrintFmt("Part 1: {}\n", Solve(2));
    PrintFmt("Part 2: {}\n", Solve(12));
  }
}