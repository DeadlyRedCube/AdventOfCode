export module D05;

import Util;

export namespace D05
{
  using Interval = ::Interval<u64>;

  auto AsInterval(const std::string &s)
  {
    char *end;
    auto first = std::strtoull(s.c_str(), &end, 10);
    return Interval::FromFirstAndLast(first, std::strtoull(end + 1, &end, 10));
  }



  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    // Find the iterator for the empty string (the break point between ranges and values)
    auto emptyStringIt = std::ranges::find(lines, std::string{});

    // Build a list of intervals, merging/sorting as we go
    std::vector<Interval> mergedIntervals;
    mergedIntervals.reserve(lines.size());
    for (auto iv : std::span{lines.begin(), emptyStringIt} | std::views::transform(AsInterval))
    {
      // Find the insertion point
      auto target = std::ranges::lower_bound(mergedIntervals, iv.First(), {}, &Interval::End);
      if (auto u = (target != mergedIntervals.end()) ? target->Union(iv) : std::nullopt; u.has_value())
      {
        // If the insertion point interval unions with this one, we'll merge in place (and continue to merge up as
        // needed)
        *target = u.value();
        for (auto next = target + 1; next != mergedIntervals.end();)
        {
          u = target->Union(*next);
          if (!u.has_value()) { break; }
          *target = u.value();
          next = mergedIntervals.erase(next);
        }
      }
      else
        { mergedIntervals.insert(target, iv); } // Didn't overlap an existing interval so insert instead.
    }

    const auto AnyIntervalContains
      = [&](auto val) { return std::ranges::any_of(mergedIntervals, [&](auto &iv) { return iv.Contains(val); }); };

    // Part 1 is the count of all entries in the second part of the input contained by any of the intervals.
    const s64 part1 = std::ranges::count_if(
      std::span{emptyStringIt + 1, lines.end()} | std::views::transform(AsU64),
      AnyIntervalContains);

    // Part 2 is the sum of all of the lengths of all of the merged intervals. I knew I spent all that time in part 1
    //  merging these things for a reason!
    const u64 part2 = *std::ranges::fold_left_first(
      mergedIntervals | std::views::transform(&Interval::Length),
      std::plus{});

    PrintFmt("Part 1: {}\n", part1);
    PrintFmt("Part 2: {}\n", part2);
  }
}