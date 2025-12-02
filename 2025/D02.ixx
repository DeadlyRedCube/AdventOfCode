module;

#include <cassert>

export module D02;

import Util;

export namespace D02
{
  u32 Exp10(u32 digitCount)
  {
    auto v = 10u;
    for (usz i = 1; i < digitCount; i++)
      { v *= 10; }
    return v;
  }

  u64 MakeRepeat(u64 base, u32 repeatCount, u32 mul)
  {
    u64 result = base;
    for (usz i = 1; i < repeatCount; i++)
    {
      result *= mul;
      result += base;
    }

    return result;
  }

  bool IsRepeat(u64 v, u32 digitCount, u32 repeatCount)
  {
    auto div = Exp10(digitCount / repeatCount);
    auto base = v % div;
    auto testN = v / div;
    for (u32 i = 1; i < repeatCount; i++)
    {
      if (base != testN % div)
        { return false; }
      testN /= div;
    }

    return true;
  }

  void Run(const char *path)
  {
    u64 sum1 = 0;
    u64 sum2 = 0;

    std::unordered_set<u64> valuesForRange;
    for (auto rangeString : Split(ReadFileLines(path)[0], ",", KeepEmpty::No))
    {
      auto range = Split(rangeString, "-", KeepEmpty::No)
        | std::views::transform([](auto &e) { char *end; return std::strtoull(e.c_str(), &end, 10); })
        | std::ranges::to<std::vector>();

      auto minDigitCount = CountDecimalDigits(range[0]);
      auto maxDigitCount = CountDecimalDigits(range[1]);

      // We're going to build a set of values for each
      valuesForRange.clear();

      // This will put any "invalid" values within the current range for the given repeat count into the
      //  valuesForRange set.
      auto GenerateValuesForRepeatCount = [&](u32 repeatCount)
      {
        for (auto digitCount = std::max(minDigitCount, repeatCount); digitCount <= maxDigitCount; digitCount++)
        {
          // If the current digit count doesn't divide nicely, we're done.
          if (digitCount % repeatCount != 0)
            { continue; }

          // Get the power of 10 that our repeated value has to be within.
          auto baseDiv = Exp10(digitCount / repeatCount);

          // Figure out the minimum invalid section value
          auto minInvalidSection = range[0];
          if (minDigitCount == digitCount)
          {
            // The minimum has the same number of digits as the digit count so we need to test against it.

            // Get the upper-most set of digits.
            while (minInvalidSection >= baseDiv)
              { minInvalidSection /= baseDiv; }

            // If, repeated, it is out of range, move to the next repeated section value.
            if (MakeRepeat(minInvalidSection, repeatCount, baseDiv) < range[0])
              { minInvalidSection++; }
          }
          else
            { minInvalidSection = baseDiv / 10; } // The lowest section value that has all of the digits set (no leading zeros)

          // Do a similar thing to calculate the maximum invalid value.
          auto maxInvalidSection = range[1];
          if (digitCount == maxDigitCount)
          {
            while (maxInvalidSection >= baseDiv)
              { maxInvalidSection /= baseDiv; }

            if (MakeRepeat(maxInvalidSection, repeatCount, baseDiv) > range[1])
              { maxInvalidSection--; }
          }
          else
            { maxInvalidSection = baseDiv - 1; }

          // For each "section" that are in range, repeat it and add it to the set (to avoid counting duplicates)
          for (auto n = minInvalidSection; n <= maxInvalidSection; n++)
            { valuesForRange.emplace(MakeRepeat(n, repeatCount, baseDiv));  }
        }
      };

      if (maxDigitCount >= 2)
      {
        // Handle the part 1 calculation separately.
        GenerateValuesForRepeatCount(2);
        sum1 += std::ranges::fold_left(valuesForRange, 0u, std::plus{});
      }

      // Now handle the rest, for part 2.
      for (auto repeatCount = 3u; repeatCount <= maxDigitCount; repeatCount++)
        { GenerateValuesForRepeatCount(repeatCount); }
      sum2 += std::ranges::fold_left(valuesForRange, 0u, std::plus{});
    }

    PrintFmt("Part 1: {}\n", sum1);
    PrintFmt("Part 2: {}\n", sum2);
  }
}