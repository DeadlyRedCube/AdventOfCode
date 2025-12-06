export module D06;

import Util;

export namespace D06
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray2D(path);

    // First up: parse the operators out of the final row, this lets us know how many columns there are.
    std::vector<char> operators;
    for (ssz x = 0; x < grid.Width(); x++)
    {
      if (auto ch = grid[x, grid.Height() - 1]; ch  != ' ')
        { operators.push_back(ch); }
    }

    // Each result starts with a 0 if it's addition or a 1 if it's a multiply (for the accumulation to be correct)
    std::vector<s64> part1Results;
    part1Results.reserve(operators.size());
    for (auto &op : operators)
      { part1Results.push_back(s64(op == '*')); }

    // Now parse all the numbers out
    for (ssz y = 0; y < grid.Height() - 1; y++)
    {
      ssz x = 0;
      usz columnIndex = 0;
      while (true)
      {
        // Skip any spaces and then leave if we reached the edge
        while (x < grid.Width() && grid[x, y] == ' ')
          { x++; }
        if (x == grid.Width())
          { break; }

        // Parse the number out of the grid and add it to the current row. We know the current grid space must be a
        //  digit so we can skip a check.
        s64 v = (grid[x, y] - '0');
        x++;
        while (x < grid.Width() && grid[x, y] != ' ')
        {
          v = (v * 10) + (grid[x, y] - '0');
          x++;
        }

        // Accumulate using the correct operator into the results for the column
        switch (operators[columnIndex])
        {
        case '+': part1Results[columnIndex] += v; break;
        case '*': part1Results[columnIndex] *= v; break;
        }
        columnIndex++;
      }
    }

    PrintFmt("Part 1: {}\n", *std::ranges::fold_left_first(part1Results, std::plus{}));

    // For part2 we're parsing transposed, effectively. This direction there's one number per column, read top to
    //  bottom. The puzzle specifies it's done right to left but it's addition and subtraction so it doesn't actually
    //  matter.
    s64 part2 = 0;
    usz opIndex = 0;
    s64 curOpValue = operators[opIndex] == '+' ? 0 : 1;
    for (ssz outY = 0; outY < grid.Width(); outY++)
    {
      // Skip all of the spaces at the start
      ssz outX = 0;
      while (outX < grid.Height() - 1 && grid[outY, outX] == ' ')
        { outX++;}
      if (outX == grid.Height() - 1)
      {
        // The whole line was spaces, so this was a blank line, which denotes the switch to the next operator.
        opIndex++;
        part2 += curOpValue;
        curOpValue = operators[opIndex] == '+' ? 0 : 1;
        continue;
      }

      // Now parse the number out of the pile (just like in part 1 we know the first space is a digit) so we can
      // accumulate it.
      s64 v = grid[outY, outX] - '0';
      outX++;
      while (outX < grid.Height() - 1 && grid[outY, outX] >= '0' && grid[outY, outX] <= '9')
      {
        v = (v * 10) + (grid[outY, outX] - '0');
        outX++;
      }

      switch (operators[opIndex])
      {
      case '+': curOpValue += v; break;
      case '*': curOpValue *= v; break;
      }
    }

    // We have one leftover value so don't forget to add it (I absolutely did forget to add it at first)
    part2 += curOpValue;

    PrintFmt("Part 2: {}\n", part2);
  }
}