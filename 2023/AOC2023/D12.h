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


  s64 MatchCount(std::string str, ArrayView<s64> brokenCounts, std::map<CacheKey, s64> &cache)
  {
    // Skip any leading dots (they cannot somehow become relevant to later matches)
    while (!str.empty() && str[0] == '.')
      { str = str.substr(1); }

    // If we have already seen this configuration of string segment and broken elements, we can return our cached
    // value.
    if (auto d = cache.find({ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() });
      d != cache.end())
      { return d->second; }

    if (brokenCounts.IsEmpty())
    {
      if (std::ranges::any_of(str, [](char c) { return c == '#'; }))
      {
        // We have no more items to match, however there are still unconsumed '#'s in the string, so this is not a
        //  match.
        cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = 0;
        return 0;
      }

      // We found one (1) match! Yay.
      cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = 1;
      return 1;
    }

    {
      s64 necessaryCount = 0;
      for (auto &c : brokenCounts)
        { necessaryCount += c; }
      necessaryCount += brokenCounts.Count() - 1;

      if (necessaryCount > str.length())
      {
        // If we have more stuff to match than string length, we can't.
        cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = 0;
        return 0;
      }
    }

    {
      // Check to see if we hit any '.'s before we hit enough '#' or '?' entries to fulfil this match.
      bool anyHash = false;
      for (s64 i = 0; i < brokenCounts[0]; i++)
      {
        if (str[i] == '#')
          { anyHash = true; }

        if (str[i] == '.')
        {
          if (anyHash)
          {
            // Unfortunately, we can't match here but there are '#'s that would need to be matched
            cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = 0;
            return 0;
          }

          // can't match here so try to match after this point
          auto r = MatchCount(str.substr(i + 1), brokenCounts, cache);
          cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = r;
          return r;
        }
      }
    }

    // We can maybe match here, figure out how to arrange it
    if (str[brokenCounts[0]] == '#')
    {
      // Can't match here, if we aren't at a '#' try matching again one char over.
      s64 r = (str[0] == '#') ? 0 : MatchCount(str.substr(1), brokenCounts, cache);
      cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = r;
      return r;
    }

    if(str[0] == '#')
    {
      // We have to match right here, so register the match, and move on to the next area, skipping one additional
      //  character (unless we're at end of string) which must be either a '?' or a '.' (which we'll treat as a '.'
      //  either way)
      auto r = MatchCount(
        str.substr(std::min(s64(str.length()), brokenCounts[0] + 1)),
        { brokenCounts, 1, ToEnd },
        cache);
      cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = r;
      return r;
    }


    // Question mark, so we CAN match here. Start by counting the matches that happen if we match our current entry
    //  right here.
    s64 count = MatchCount(
      str.substr(std::min(s64(str.length()), brokenCounts[0] + 1)),
      { brokenCounts, 1, ToEnd },
      cache);

    // We can also treat this ? as a ., so skip it and keep trying matches
    auto r = count + MatchCount(str.substr(1), brokenCounts, cache);
    cache[{ .stringLength = s64(str.length()), .brokenCounts = brokenCounts.Count() }] = r;
    return r;
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
      brokenCounts.AppendMultiple(
        splits | std::views::drop(1) | std::views::transform([](auto &&s) { return std::atoll(s.c_str()); }) | std::ranges::to<std::vector>());

      std::map<CacheKey, s64> cache;
      auto count = MatchCount(splits[0], brokenCounts, cache);
      p1 += count;

      auto p2Str = splits[0];
      UnboundedArray<s64> p2BrokenCounts = brokenCounts;
      for (auto i = 0; i < 4; i++)
      {
        p2Str += '?';
        p2Str += splits[0];
        p2BrokenCounts.AppendMultiple(brokenCounts);
      }

      std::map<CacheKey, s64> p2Cache;
      count = MatchCount(p2Str, p2BrokenCounts, p2Cache);
      p2 += count;
    }

    PrintFmt("Part 1: {}\n", p1);
    PrintFmt("Part 2: {}\n", p2);
  }
}