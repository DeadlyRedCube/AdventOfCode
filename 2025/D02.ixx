module;

#include <cassert>

export module D02;

import Util;

export namespace D02
{
  void Run(const char *path)
  {
    u64 sum1 = 0;
    u64 sum2 = 0;

    for (auto rangeString : Split(ReadFileLines(path)[0], ",", KeepEmpty::No))
    {
      auto range = Split(rangeString, "-", KeepEmpty::No)
        | std::views::transform([](auto &e) { char *end; return std::strtoull(e.c_str(), &end, 10); })
        | std::ranges::to<std::vector>();

      for (auto n = range[0]; n <= range[1]; n++)
      {
        auto digitCount = CountDecimalDigits(n);

        for (usz repeatCount = 2; repeatCount <= digitCount; repeatCount++)
        {
          // If the digit count isn't a multiple of how many repeats we're testing, we're done.
          if (digitCount % repeatCount != 0)
            { continue; }

          // Generate a multiple to use to modulo/divide by to do the testing.
          auto baseDiv = 10;
          for (usz i = 1; i < digitCount / repeatCount; i++)
            { baseDiv *= 10; }

          // This is the bottom N/R digits that we'll use to test against the others
          auto base = n % baseDiv;

          // Start testing against the remaining sections of `n`
          auto testN = n / baseDiv;
          for (usz j = 1; j < repeatCount; j++)
          {
            // If the base section doesn't match this section, it's a mismatch and we should test the next digit count.
            if (base != testN % baseDiv)
              { goto mismatch; } // This is effectively a "continue" for the outer loop

            // Move testN down to the next set of digits.
            testN /= baseDiv;
          }

          // Part 1 only cared about invalid numbers with two repeated sections.
          if (repeatCount == 2)
            { sum1 += n; }
          sum2 += n;

          // We don't want to double-count this as invalid so break.
          break;

          mismatch:; // If C++ had named loops this would be less weird.
        }
      }
    }

    PrintFmt("Part 1: {}\n", sum1);
    PrintFmt("Part 2: {}\n", sum2);
  }
}