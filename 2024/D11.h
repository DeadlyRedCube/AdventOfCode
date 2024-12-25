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

    struct S
    {
      s32 lastBlinkIndex = -2;
      s64 count = 0;

      void Add(s32 blinkIndex, s64 c)
      {
        if (lastBlinkIndex != blinkIndex)
        {
          lastBlinkIndex = blinkIndex;
          count = 0;
        }

        count += c;
      }
    };

    // Make a maping from stone value to count of that value (order of the stones doesn't matter at all, thankfully)
    std::unordered_map<s64, S> stoneCounts;
    stoneCounts.reserve(4000);
    for (auto stone : Split(line, " ", KeepEmpty::No) | std::views::transform(AsS64) | std::ranges::to<std::vector>())
    {
      stoneCounts[stone].count++;
      stoneCounts[stone].lastBlinkIndex = -1;
    }

    std::unordered_map<s64, S> newCounts;
    newCounts.reserve(4000);

    auto Blink =
      [&](s32 blinkIndex)
      {
        for (auto [stone, s] : stoneCounts)
        {
          if (s.lastBlinkIndex != blinkIndex - 1)
            { continue; }

          if (stone == 0)
          {
            newCounts[1].Add(blinkIndex, s.count);
          }
          else if (auto digits = DigitCount(stone); digits % 2 == 0)
          {
            s64 tens = Tens(digits / 2);
            s64 left = stone / tens;
            s64 right = stone - (left * tens);
            newCounts[left].Add(blinkIndex, s.count);
            newCounts[right].Add(blinkIndex, s.count);
          }
          else
            { newCounts[stone * 2024].Add(blinkIndex, s.count); }
        }

        std::swap(stoneCounts, newCounts);
      };

    // Do the first 25 blinks for part 1
    for (s32 blink = 0; blink < 25; blink++)
      { Blink(blink); }
    for (auto [stone , s] : stoneCounts)
      { p1 += (s.lastBlinkIndex == 24) ? s.count : 0; }

    // Now do the remaining blinks
    for (s32 blink = 25; blink < 75; blink++)
      { Blink(blink); }
    for (auto [stone, s] : stoneCounts)
      { p2 += (s.lastBlinkIndex == 74) ? s.count : 0; }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}