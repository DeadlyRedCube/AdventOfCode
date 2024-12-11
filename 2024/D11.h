#pragma once


namespace D11
{
  s32 DigitCount(s64 v)
  {
    s32 c = 0;
    while (v != 0)
      { c++; v /= 10; }
    return c;
  }

  s64 Tens(s32 v)
  {
    s64 r = 1;
    for (s32 i = 0; i < v; i ++)
      { r *= 10; }
    return r;
  }

  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto line = ReadFileLines(path)[0];

    // Make a maping from stone value to count of that value (order of the stones doesn't matter at all, thankfully)
    std::map<s64, s64> stoneCounts;
    for (auto stone : Split(line, " ", KeepEmpty::No) | std::views::transform(AsS64) | std::ranges::to<std::vector>())
      { stoneCounts[stone]++; }

    std::map<s64, s64> newCounts;

    auto Blink =
      [&]
      {
        for (auto [stone, count] : stoneCounts)
        {
          if (stone == 0)
            { newCounts[1] += count; }
          else if (auto digits = DigitCount(stone); digits % 2 == 0)
          {
            s64 tens = Tens(digits / 2);
            s64 left = stone / tens;
            s64 right = stone - (left * tens);
            newCounts[left] += count;
            newCounts[right] += count;
          }
          else
            { newCounts[stone * 2024] += count; }
        }

        std::swap(stoneCounts, newCounts);
        newCounts.clear();
      };

    // Do the first 25 blinks for part 1
    for (s32 blink = 0; blink < 25; blink++)
      { Blink(); }
    for (auto [stone , count] : stoneCounts)
      { p1 += count; }

    // Now do the remaining blinks
    for (s32 blink = 25; blink < 75; blink++)
      { Blink(); }
    for (auto [stone , count] : stoneCounts)
      { p2 += count; }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}