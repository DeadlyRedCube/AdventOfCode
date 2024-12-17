#pragma once


namespace D02
{
  void Run(const char *path)
  {
    s32 p1 = 0;
    s32 p2 = 0;
    for (auto &entries : ReadFileLines(path)
      | std::views::transform(
        [](auto &line)
        {
          return Split(line, " ", KeepEmpty::No)
            | std::views::transform([](const auto &str) { char *end; return s64(std::strtoll(str.c_str(), &end, 10)); })
            | std::ranges::to<std::vector>();
        })
      | std::ranges::to<std::vector>())
    {
      // This is treated as increasing if two of the three first entries are increasing.
      bool increasing
        = (u32(entries[3] > entries[2]) + u32(entries[2] > entries[1]) + u32(entries[1] > entries[0])) >= 2;

      bool p1Safe = true;
      bool p2Safe = true;

      auto IsGood =
        [=](s64 a, s64 b)
        {
          return ((increasing && b > a) || (!increasing && b < a))
            && InRangeInclusive(std::abs(b - a), 1, 3);
        };

      for (size_t i = 1; i < entries.size(); i++)
      {
        if (IsGood(entries[i - 1], entries[i]))
          { continue; }

        if (!p1Safe)
        {
          // We hit a second error here, so p2 is invalid (and thus we can stop checking)
          p2Safe = false;
          break;
        }

        // Any wrong answer means p1 is not safe.
        p1Safe = false;

        // If we're either at the end or testing the next element against our previous one is good, skip the next
        //  element and move on.
        if (i == entries.size() - 1 || IsGood(entries[i - 1], entries[i + 1]))
        {
          i++;
          continue;
        }

        // If we cannot remove *this* element, instead see what happens if we removed the previous element. If it's
        //  *also* bad then there's no fix here and p2 is just wrong.
        if (i > 1 && !IsGood(entries[i - 2], entries[i]))
        {
          p2Safe = false;
          break;
        }
      }

      p1 += s32(p1Safe);
      p2 += s32(p2Safe);
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}