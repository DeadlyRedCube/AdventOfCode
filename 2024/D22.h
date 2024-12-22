#pragma once


namespace D22
{
  // An array of possibilities (the lookup index is the four previous changes (p0-p3) where it's
  //  Sum(n == 0..3) of (pn + 9) << ((3 - n) * 5)
  //  That is to say, add 9 to ensure a positive value (these changes are using bets mod 10 so only -9 to 9 are valid)
  //   then shifted up 5 per age
  //  (this means there are array entries that never get used because there's 19 possible values per change but we're
  //   shifting by 32, but that's purely so that it can be shifts and masking instead of muls and modulos).
  constexpr u32 PossibilityKeyCount = 1024*1024;

  struct Possibility
  {
    s16 bananaCount;    // The number of bananas seen for this possibility
    s16 lastSeenIndex;  // The last index of a secret number that updated the banana count.
  };

  std::array<Possibility, PossibilityKeyCount> possibilities;

  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s16 p2 = 0;

    for (auto [secretNumIndex, secretNum]
      : std::views::enumerate( ReadFileLines(filePath) | std::views::transform(AsS32)))
    {
      u32 possibilityKey = 0;

      // Store the bet before this one (to compute the changes)
      u32 prevBet = secretNum % 10;

      // Made this a lambda to not repeat the logic.
      auto DoHashAndGetCurrentBet =
        [&]
        {
          // Do the hash
          secretNum ^= (secretNum << 6) & 0xffffff;
          secretNum ^= (secretNum >>  5);
          secretNum ^= (secretNum << 11) & 0xffffff;

          // Calculate the bet (lowest digit)
          auto curBet = (secretNum % 10);

          // Shift the key over 5 (moving things earlier in the history) then mix in the change (+ 9 so it's positive)
          //  (then mask off the upper bits for values that are too old)
          possibilityKey = ((possibilityKey << 5) | (curBet - prevBet + 9)) & (PossibilityKeyCount - 1);
          prevBet = curBet;

          return s16(curBet);
        };

      // Do 3 hash values without checking possibilities (since there are no sets of four changes until, you know,
      //  the fourth price change)
      for (u32 i = 0; i < 3; i++)
        { (void)DoHashAndGetCurrentBet(); }

      // Now do all the rest
      for (u32 i = 3; i < 2000; i++)
      {
        auto curBet = DoHashAndGetCurrentBet(); // This time we care about the current bet

        auto &p = possibilities[possibilityKey];

        // Check to see if this secret number already updated this possibility yet.
        if (p.lastSeenIndex != secretNumIndex)
        {
          // ...we hadn't updated this one yet, so update it now.
          p.bananaCount += curBet;
          p.lastSeenIndex = s16(secretNumIndex);

          // Update the maximum value we've found thus far.
          p2 = std::max(p2, p.bananaCount);
        }
      }

      // For part 1 we just care about the sums of all of the secret numbers
      p1 += secretNum;
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}