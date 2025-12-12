module;

#include <cassert>

export module D10;

import Util;

export namespace D10
{
  auto ParseInput(const std::string &line)
    -> std::tuple<u32, std::vector<s32>, std::vector<u32>>
  {
    auto sets = Split(line, "()[] {}", KeepEmpty::No);

    // First string in the list is the lights display target
    u32 target = 0;
    for (usz i = sets[0].size() - 1; i < sets[0].size(); i--)
    {
      target <<= 1;
      if (sets[0][i] == '#')
        { target |= 1; }
    }

    const auto targetJolts = Split(sets.back(), ",", KeepEmpty::No) | std::views::transform(AsS32) | std::ranges::to<std::vector>();

    auto buttons = std::vector<u32>{};

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

    return {target, std::move(targetJolts), std::move(buttons)};
  }



  auto Part1CalculateForLine(std::span<const u32> buttons, u32 targetBits) -> u32
  {
    u32 minCount = std::numeric_limits<u32>::max();
    for (u32 i = 0; i < (1u << buttons.size()); i++)
    {
      u64 r = 0;
      for (usz j = 0; j < buttons.size(); j++)
      {
        if ((1ul << j) & i)
          { r ^= buttons[j]; }
      }

      if (r == targetBits)
        { minCount = std::min(minCount, u32(std::popcount(i))); }
    }

    return minCount;
  }



  // Take the buttons and joltage inputs and build a matrix from them (yes, it's column, row indexing because I'm a
  //  monster)
  // The matrix has buttonCount + 1 columns and targetJolts rows. Each column (sans the right-most) represents a button
  //  and has a 1 for any row where the button increases the corresponding joltage.
  // The right-most column is the target joltage values.
  auto Part2GenerateMatrix(std::span<const u32> buttons, std::span<const s32> targetJolts) -> Array2D<s32>
  {
    Array2D<s32> mat = { buttons.size() + 1, targetJolts.size() };
    for (usz joltIndex = 0; joltIndex < targetJolts.size(); joltIndex++)
    {
      // The right-most column contains the target joltages
      mat[buttons.size(), joltIndex] = targetJolts[joltIndex];

      // The other columns all have a 1 where the button increases the joltage for its corresponding row, and a 0
      //  where it does nothing.
      const u64 joltBit = 1ull << joltIndex;
      for (usz buttonIndex = 0; buttonIndex < buttons.size(); buttonIndex++)
      {
          if ((buttons[buttonIndex] & joltBit) != 0)
            { mat[buttonIndex, joltIndex] = 1; }
      }
    }

    return mat;
  }



  // Do a pseudo-Gaussian elimination on the given matrix to get it into a pseudo-reduced row echelon order.
  //  This isn't true reduced row echelon because leading entries are allowed to be > 1 (by virtue of needing to keep
  //  things integral). Additionally (per a comment below), unlike in proper Gaussian elimination, we swap columns as
  //  well as rows (since we don't care about which button is which), to ensure a consistent order (the first N rows
  //  will always be diagonal, followed by any columns that could not be reduced, followed by the target column)
  auto PseudoGaussianElimination(Array2D<s32> &mat) -> u32
  {
    u32 solveCount = u32(mat.Height());

    u32 eliminationColumnIndex = 0;
    u32 eliminationRowIndex = 0;
    u32 incompleteColumnCount = 0;
    while (eliminationColumnIndex < mat.Width() - 1 && eliminationRowIndex < solveCount)
    {
      // Find the first row that has this elimination column
      std::optional<u32> foundEliminationRowIndex;
      for (u32 y = 0; y < solveCount; y++)
      {
        auto row = mat.GetRowAsSpan(y);

        // Ignore columns where it's not the leader
        if (std::ranges::any_of(row | std::views::take(eliminationColumnIndex), [](auto v) { return v != 0; }))
          { continue; }

        if (auto v = row[usz(eliminationColumnIndex)]; v != 0)
        {
          if (!foundEliminationRowIndex.has_value() || v == 1)
          {
            foundEliminationRowIndex = y;
            if (v == 1)
              { break; } // This is the best we can do (dealing with a 1 is easiest).
          }
        }
      }

      if (!foundEliminationRowIndex.has_value())
      {
        // There was no row that had a leading entry for this column. We can do something here that you can't do in
        //  true gaussian elimination: we don't care about which button is which, so we can additionally swap *columns*
        //  instead of rows. Swap this column to the end of the buttons and increment our count of "incomplete" columns
        incompleteColumnCount++;
        u32 swapColumn = u32(mat.Width() - 1 - incompleteColumnCount);
        for (u32 y = 0; y < solveCount; y++)
          { std::swap(mat[eliminationColumnIndex, y], mat[swapColumn, y]); }
        continue;
      }

      // Swap it into place
      auto eliminationRow = mat.GetRowAsSpan(eliminationRowIndex);
      if (*foundEliminationRowIndex != eliminationRowIndex)
        { std::ranges::swap_ranges(mat.GetRowAsSpan(*foundEliminationRowIndex), eliminationRow); }

      assert(eliminationRow[usz(eliminationColumnIndex)] != 0);

      // If the leading value is negative, flip the whole roe (we want only positive values for solved columns)
      if (eliminationRow[usz(eliminationColumnIndex)] < 0)
      {
        for (auto &v : eliminationRow)
          { v = -v; }
      }

      // Now do the elimination (subtract a multiple of it from any row necessary, which may include multiplying
      //  the target row
      for (u32 y = 0; y < solveCount; y++)
      {
        if (y == eliminationRowIndex)
          { continue; }

        auto row = mat.GetRowAsSpan(y);
        if (row[usz(eliminationColumnIndex)] == 0)
          { continue; }

        auto lcm = LeastCommonMultiple(std::abs(row[usz(eliminationColumnIndex)]), eliminationRow[usz(eliminationColumnIndex)]);
        auto dstMul = lcm / row[usz(eliminationColumnIndex)];
        auto srcMul = lcm / eliminationRow[usz(eliminationColumnIndex)];

        for (auto [dst, src] : std::views::zip(row, eliminationRow))
        {
          dst *= dstMul;
          dst -= src * srcMul;
        }

        // Before we're done, two simplification passes:
        if (auto leadingValue = std::ranges::find_if(row, [](auto v) { return v != 0; }); leadingValue != row.end())
        {
          // First, if the leader of this row is negative, negate the whole thing
          if (*leadingValue < 0)
          {
            for (auto &v : row)
              { v = -v; }
          }

          // Next, find the gcd of all the non-zero values and factor it out.
          auto gcd = *leadingValue;
          for (auto &v : row)
            { gcd = (v == 0) ? gcd : GreatestCommonDenominator(gcd, std::abs(v)); }
          for (auto &v : row)
            { v /= gcd; }
        }
        else
        {
          // This row is all zeros, so drop it to the bottom and decrement the count of solvable rows.
          solveCount--;
          auto lastRow = mat.GetRowAsSpan(solveCount);
          std::ranges::swap_ranges(row, lastRow);
          y--; // Repeat this y coordinate since we just brought a new value up.
        }
      }

      eliminationRowIndex++;
      eliminationColumnIndex++;
    }

    return solveCount;
  }



  // This function takes the input, uses the above function to make a matrix from it, then performs a variant of
  //  Gaussian elimination to get a list of "solved" coefficients (button-equivalents that only affect a single
  //  output), a matrix of "unsolved" buttons (buttons that could not be reduced and affect multiple outputs), and a
  //  list of the target values for the buttons (these aren't the joltages anymore, but can be used for the solve)
  auto Part2ProcessInput(std::span<const u32> buttons, std::span<const s32> targetJolts)
  {
    auto mat = Part2GenerateMatrix(buttons, targetJolts);
    auto solveCount = PseudoGaussianElimination(mat);

    // Peel the values we care about out of the solved matrix.
    std::vector<s32> solvedCoefficients;
    std::vector<s32> targets;
    solvedCoefficients.resize(solveCount);
    targets.resize(solveCount);

    for (usz i = 0; i < solveCount; i++)
    {
      solvedCoefficients[i] = mat[i, i];
      targets[i] = mat[mat.Width() - 1, i];
    }

    Array2D<s32> unsolvedColumns;

    if (u32 unsolvedCount = u32(mat.Width() - solveCount - 1); unsolvedCount > 0)
      { unsolvedColumns = { unsolvedCount, solveCount }; }

    for (s32 y = 0; y < unsolvedColumns.Height(); y++)
    {
      std::ranges::copy(
        mat.GetRowAsSpan(y)
          | std::views::drop(solveCount)
          | std::views::take(unsolvedColumns.Width()),
        unsolvedColumns.GetRowAsSpan(y).begin());
    }

    return std::make_tuple(std::move(solvedCoefficients), std::move(unsolvedColumns), std::move(targets));
  }



  // A structure of scratch space so that we can allocate as little as possible during the part 2 solve
  struct ScratchSpace
  {
    ScratchSpace(usz unsolvedButtonCount, usz targetCount)
    {
      pressGuesses.reserve(unsolvedButtonCount);
      targets.resize(targetCount);
      mins.resize(unsolvedButtonCount);
      maxes.resize(unsolvedButtonCount);
    }

    std::vector<s32> pressGuesses;  // This is used in the recursion to store the guess values of the calling func(s)
    std::vector<s32> targets;       // Scratch space used when calculating our current targets
    std::vector<s32> mins;          // Buffer for calculating the minimum values for unsolved columns
    std::vector<s32> maxes;         // Buffer for calculating the maximum values for unsolved columns
  };



  void UpdateScratchTargetsForRemainingUnsolvedColumns(
    auto &unsolvedColumns,
    auto &targets,
    ScratchSpace &scratch)
  {
    // test targets need to subtract the previous columns.
    std::ranges::copy(targets, scratch.targets.begin());

    // Factor out all of the buttons to the left of this (for which we have currently committed a guess)
    for (usz bi = 0; bi < scratch.pressGuesses.size(); bi++)
    {
      for (usz i = 0; i < targets.size(); i++)
        { scratch.targets[i] -= scratch.pressGuesses[bi] * unsolvedColumns[bi, i]; }
    }
  }



  // This does an iterative solution using the unsolved columns and the target values as a system of inequalities.
  //  This drastically limits the amount of button presses we need to consider.
  auto FindExtentsForRemainingUnsolvedColumns(
    s32 bestPressCount,
    auto &unsolvedColumns,
    usz columnIndex,
    ScratchSpace &scratch) -> std::pair<s32, s32>
  {
    // Prep our scratch space
    std::ranges::fill(scratch.mins, 0);
    std::ranges::fill(scratch.maxes, bestPressCount);

    while (true)
    {
      bool changed = false;

      for (usz curColIndex = columnIndex; curColIndex < usz(unsolvedColumns.Width()); curColIndex++)
      {
        // For this column, check scratch.targets against the other mins/maxes to see if we can commit something better
        for (usz joltIndex = 0; joltIndex < scratch.targets.size(); joltIndex++)
        {
          // ignore 0s for this column
          if (unsolvedColumns[curColIndex, joltIndex] == 0)
            { continue; }

          s32 limit = scratch.targets[joltIndex];
          for (usz otherColIndex = columnIndex; otherColIndex < usz(unsolvedColumns.Width()); otherColIndex++)
          {
            if (otherColIndex == curColIndex)
              { continue; }

            // If the value of the other column is negative, we want to take its maximum into account. Otherwise,
            //  the minimum is what matters.
            if (unsolvedColumns[otherColIndex, joltIndex] < 0)
              { limit -= scratch.maxes[otherColIndex] * unsolvedColumns[otherColIndex, joltIndex]; }
            else if (unsolvedColumns[otherColIndex, joltIndex] > 0)
              { limit -= scratch.mins[otherColIndex] * unsolvedColumns[otherColIndex, joltIndex]; }
          }

          // Technically can be one too low for a lower limit due to int rounding but that's fine.
          limit /= unsolvedColumns[curColIndex, joltIndex];

          if (unsolvedColumns[curColIndex, joltIndex] < 0)
          {
            // Updating minimum
            if (scratch.mins[curColIndex] < limit)
            {
              scratch.mins[curColIndex] = limit;
              changed = true;
            }
          }
          else
          {
            limit = std::max(0, limit);

            // Updating maximum
            if (scratch.maxes[curColIndex] > limit)
            {
              scratch.maxes[curColIndex] = limit;
              changed = true;
            }
          }
        }
      }

      if (!changed)
        { break; }
    }

    return
      {
        scratch.mins[columnIndex],
        std::min(bestPressCount, scratch.maxes[columnIndex]),
      };
  }



  // The Part2 solver proper
  auto Part2Calculate(
    s32 bestPressCount,
    auto &solvedCoefficients,
    auto &unsolvedColumns,
    auto &targets,
    ScratchSpace &scratch) -> s32
  {
    UpdateScratchTargetsForRemainingUnsolvedColumns(unsolvedColumns, targets, scratch);

    auto currentColumn = scratch.pressGuesses.size();

    auto [minPress, maxPress] = FindExtentsForRemainingUnsolvedColumns(
      bestPressCount,
      unsolvedColumns,
      currentColumn,
      scratch);

    if (currentColumn < usz(unsolvedColumns.Width() - 1))
    {
      // There are more unsolved columns to our right, so we're going to iterate through our valid range, trying each
      //  value recursively.
      for (s32 columnPressCount = minPress; columnPressCount <= maxPress; columnPressCount++)
      {
        scratch.pressGuesses.push_back(columnPressCount);
        bestPressCount = std::min(
          bestPressCount,
          Part2Calculate(bestPressCount, solvedCoefficients, unsolvedColumns, targets, scratch));
        scratch.pressGuesses.pop_back();
      }

      return bestPressCount;
    }

    // If we got here, this is the right-most unsolved column, so we only have a single one to deal with.

    for (s32 columnPressCount = minPress; columnPressCount <= maxPress; columnPressCount++)
    {
      scratch.pressGuesses.push_back(columnPressCount);

      // Update scratch targets to take into account all of the unsolved press guesses
      std::ranges::copy(targets, scratch.targets.begin());
      for (usz bi = 0; bi < usz(unsolvedColumns.Width()); bi++)
      {
        for (usz i = 0; i < solvedCoefficients.size(); i++)
          { scratch.targets[i] -= scratch.pressGuesses[bi] * unsolvedColumns[bi, i]; }
      }

      // Sum up the button presses: it's the number of unsolved button presses plus the remaining target values divided
      //  by the solved coefficients (i.e. if a solved coefficient is "2" then each press of that column counts as 2
      //  button presses)
      s32 sum = Sum(scratch.pressGuesses);
      for (usz i = 0; i < solvedCoefficients.size(); i++)
        { sum += scratch.targets[i] / solvedCoefficients[i]; }

      scratch.pressGuesses.pop_back();

      // If the sum is worse than our best case we can just move on.
      if (sum >= bestPressCount)
        { continue; }

      // Now see if this is a valid solution
      for (usz i = 0; i < solvedCoefficients.size(); i++)
      {
        if (scratch.targets[i] < 0 || scratch.targets[i] % solvedCoefficients[i] != 0)
          { goto nope; } // would require a negative or fractional button press, not allowed.
      }

      // This is valid (and it's less than our current best, so update our best guess.
      bestPressCount = sum;
    nope:;
    }

    return bestPressCount;
  }



  void Run(const char *path)
  {
    u64 part1 = 0;
    u64 part2 = 0;

    for (auto line : ReadFileLines(path))
    {
      auto [part1Target, targetJolts, buttons] = ParseInput(line);

      part1 += Part1CalculateForLine(buttons, part1Target);

      // Extract the fancy part 2 data from the inputs.
      auto [solvedCoefficients, unsolvedColumns, targets] = Part2ProcessInput(buttons, targetJolts);

      if (unsolvedColumns.Width() == 0)
      {
        // Every button was solved (i.e. solvedCoefficients[i] == 1), so we can simply sum the targets.
        part2 += Sum(targets);
        continue;
      }

      // Otherwise, we have to do things the hard way.
      const s32 absoluteMaxPressCount = Sum(targetJolts);
      auto scratch = ScratchSpace{usz(unsolvedColumns.Width()), solvedCoefficients.size()};
      part2 += Part2Calculate(absoluteMaxPressCount, solvedCoefficients, unsolvedColumns, targets, scratch);
    }

    PrintFmt("Part 1: {}\n", part1);
    PrintFmt("Part 2: {}\n", part2);
  }
}