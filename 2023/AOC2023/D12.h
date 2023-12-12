#pragma once

#include <set>
namespace D12
{
  s64 MatchCount(std::string str, ArrayView<s64> brokenCounts)
  {
    // If we have nothing to match, there'd better not be any remaining #s
    if (brokenCounts.IsEmpty())
    {
      if (std::ranges::any_of(str, [](char c) { return c == '#'; }))
        { return 0; }

      return 1;
    }

    // Skip any leading dots
    while (!str.empty() && str[0] == '.')
    {
      str = str.substr(1);
    }

    // If we have more stuff to match than string length, we can't.
    if (brokenCounts[0] > str.length())
      { return 0; }

    {
      bool anyHash = false;
      for (s64 i = 0; i < brokenCounts[0]; i++)
      {
        if (str[i] == '#')
        {
          anyHash = true;
        }

        if (str[i] == '.')
        {
          if (anyHash)
            { return 0; }

          // can't match here so try to match after this point
          return MatchCount(str.substr(i + 1), brokenCounts);
        }
      }
    }

    // We can maybe match here, figure out how to arrange it
    if (str[brokenCounts[0]] == '#')
    {
      // Can't match here, if we aren't at a '#' try matching again one char over.
      return (str[0] == '#') ? 0 : MatchCount(str.substr(1), brokenCounts);
    }

    if(str[0] == '#')
    {
      // We have to match right here, so register the match, and move on to the next area, skipping one additional
      //  character (unless we're at end of string) which must be either a '?' or a '.' (which we'll treat as a '.'
      //  either way)
      return MatchCount(
        str.substr(std::min(s64(str.length()), brokenCounts[0] + 1)),
        { brokenCounts, 1, ToEnd });
    }


    // Question mark, so we CAN match here. Start by counting the matches that happen if we match our current entry
    //  right here.
    s64 count = MatchCount(
      str.substr(std::min(s64(str.length()), brokenCounts[0] + 1)),
      { brokenCounts, 1, ToEnd });

    // We can also treat this ? as a ., so skip it and keep trying matches
    return count + MatchCount(str.substr(1), brokenCounts);
  }


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    s64 arrangementCount = 0;
    for (auto &line : lines)
    {
      auto splits = Split(line, " ,", KeepEmpty::No);

      UnboundedArray<s64> brokenCounts;
      brokenCounts.AppendMultiple(
        splits | std::views::drop(1) | std::views::transform([](auto &&s) { return std::atoll(s.c_str()); }) | std::ranges::to<std::vector>());

      auto count = MatchCount(splits[0], brokenCounts);
      arrangementCount += count;
    }

    PrintFmt("Part 1: {}\n", arrangementCount);
  }
}