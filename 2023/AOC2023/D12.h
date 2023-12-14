#pragma once

#include <set>
namespace D12
{
  // Needed a cache of data to make this thing fast enough to finish part 2
  struct CacheKey
  {
    s64 stringLength;
    s64 brokenCounts;

    bool operator ==(const CacheKey &) const = default;
    auto operator <=>(const CacheKey &) const = default;
  };


  s64 MatchCount(std::string_view str, ArrayView<s64> brokenCounts, std::map<CacheKey, s64> &cache)
  {
    if (auto d = cache.find({ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() });
      d != cache.end())
      { return d->second; }

    if (brokenCounts.IsEmpty())
    {
      // If we're out of things to match, this counts as a match unless there are remaining '#'es
      s64 r = std::ranges::any_of(str, [](char c) { return c == '#'; }) ? 0 : 1;
      cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = r;
      return r;
    }

    // Skip any leading '.'s, they're irrelevant
    while (!str.empty() && str[0] == '.')
      { str = str.substr(1); }

    if (str.length() < usz(AccumulateRange(brokenCounts, s64(0)) + brokenCounts.Count() - 1))
    {
      // Not enough string length left to match this entry so we're done.
      cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = 0;
      return 0;
    }

    s64 matches = 0;

    // Treat the starting point as a '#' - if we have a continuous string of ? or # to match the current broken entry,
    //  and we're either at the end of the list or the next element can be treated as a '.', we can match here.
    if (std::ranges::all_of(str.substr(0, usz(brokenCounts[0])), [](char c) { return c != '.'; })
      && (brokenCounts.Count() == 1 || (str.length() > usz(brokenCounts[0]) && str[brokenCounts[0]] != '#')))
    {
      matches += MatchCount(
        str.substr(brokenCounts[0] + ((brokenCounts.Count() == 1) ? 0 : 1)), // Skip the matched portion
        { brokenCounts, 1, ToEnd }, // Move on to the next entry in the list
        cache);
    }

    if (str[0] == '?')
    {
      // Can also try to treat this as a '.' so skip this character and try matching again at the next spot.
      matches += MatchCount(str.substr(1), brokenCounts, cache);
    }

    cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = matches;
    return matches;
  }


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    s64 p1 = 0;
    s64 p2 = 0;
    for (auto &line : lines)
    {
      auto splits = Split(line, " ,", KeepEmpty::No);

      UnboundedArray<s64> brokenCounts;
      brokenCounts.AppendMultiple(splits | std::views::drop(1) | std::views::transform(StrToNum));

      std::map<CacheKey, s64> cache;
      auto count = MatchCount(splits[0], brokenCounts, cache);
      p1 += count;

      auto p2Str  = Join({ splits[0], splits[0], splits[0], splits[0], splits[0] }, "?");
      UnboundedArray<s64> p2BrokenCounts;
      for (auto i = 0; i < 5; i++)
        { p2BrokenCounts.AppendMultiple(brokenCounts); }

      std::map<CacheKey, s64> p2Cache;
      count = MatchCount(p2Str, p2BrokenCounts, p2Cache);
      p2 += count;
    }

    PrintFmt("Part 1: {}\n", p1);
    PrintFmt("Part 2: {}\n", p2);
  }
}