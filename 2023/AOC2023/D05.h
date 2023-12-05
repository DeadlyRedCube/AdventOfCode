#pragma once

namespace D05
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    using Interval = ::Interval<int64_t>;

    struct MapEntry
    {
      int64_t destStart;
      Interval srcInterval;
    };

    UnboundedArray<UnboundedArray<MapEntry>> maps;
    UnboundedArray<int64_t> seeds;

    UnboundedArray<Interval> part2Ranges;

    // Parse the text into structures
    for (auto line : lines)
    {
      // Ignore empty lines
      if (line.empty())
        { continue; }

      if (line.starts_with("seeds"))
      {
        // The seed list starts after a colon and is otherwise a space-separated list.
        for (auto seed : Split(line.substr(line.find(':') + 1), " ", KeepEmpty::No))
        {
          seeds.Append(std::atoll(seed.c_str()));

          // Every two seed entries goes into a part 2 seed range.
          if (seeds.Count() % 2 == 0)
            { part2Ranges.Append(Interval::FromStartAndLength(seeds[FromEnd(-2)], seeds[FromEnd(-1)])); }
        }
      }
      else if (std::isdigit(line[0]))
      {
        // If this starts with a digit it's a 3-number line that gets added, as an entry, into the most-recent
        //  maps entry.
        BoundedArray<int64_t, 3> es;
        for (auto e : Split(line, " ", KeepEmpty::No))
          { es.Append(std::atoll(e.c_str())); }

        maps[FromEnd(-1)].Append({ es[0], Interval::FromStartAndLength(es[1], es[2]) });
      }
      else
      {
        // Anything else is the name of a new map. Don't care what the names are, just the ordering is important.
        maps.AppendNew();
      }
    }

    int64_t minLoc = std::numeric_limits<int64_t>::max();

    // For part 1, map each seed through the maps to the end.
    for (auto seed : seeds)
    {
      auto target = seed;
      for (auto &map : maps)
      {
        for (auto &e : map)
        {
          if (e.srcInterval.Contains(target))
          {
            // Found a match so update our target point and we can stop searching.
            target += (e.destStart - e.srcInterval.Start());
            break;
          }
        }
      }

      minLoc = std::min(minLoc, target);
    }

    PrintFmt("Part 1: {}\n", minLoc);

    // Sort the map entries by srcStart to make overlap testing easier (since we can just scan linearly through
    //  in source space).
    for (auto &m : maps)
      { std::ranges::sort(m, [](auto a, auto b) { return a.srcInterval.Start() < b.srcInterval.Start(); }); }

    // Now we're going to scan the ranges and see how they overlap each level of map entry to filter down to the
    //  final set.
    UnboundedArray<Interval> scratch;
    for (auto map : maps)
    {
      scratch.SetCount(0);

      for (auto r : part2Ranges)
      {
        for (auto e : map)
        {
          if (auto before = r.PartBefore(e.srcInterval); before.has_value())
          {
            // At least part of our range extends before the current map interval, so add that portion verbatim.
            scratch.Append(*before);

            // Keep the remainder if there is a remainder, otherwise clear out our range and break, we're done.
            if (auto remainder = r.PartAfter(*before); remainder.has_value())
              { r = *remainder; }
            else
            {
              r = {};
              break;
            }
          }

          if (auto overlap = r.Intersection(e.srcInterval); overlap.has_value())
          {
            // At least part of the range overlapped the map entry, add the overlapped portion, mapped from
            //  source -> dest.
            scratch.Append(Interval::FromStartAndLength(
              overlap->Start() + e.destStart - e.srcInterval.Start(),
              overlap->Length()));

            // Keep the remainder if there is a remainder, otherwise clear out our range and break, we're done.
            if (auto remainder = r.PartAfter(e.srcInterval); remainder.has_value())
              { r = *remainder; }
            else
            {
              r = {}; break;
            }
          }
        }

        if (r.Length() > 0)
        {
          // We still have some range left after checking all the map entries so add whatever's left verbatim.
          scratch.Append(r);
        }
      }

      // Now swap in our ranges for the next pass.
      std::swap(scratch, part2Ranges);
    }

    // Now find the minimum starting range point, as that'll guaranteed be our first entry.
    minLoc = std::numeric_limits<int64_t>::max();
    for (auto &r : part2Ranges)
      { minLoc = std::min(minLoc, r.Start()); }
    PrintFmt("Part 2: {}\n", minLoc);
  }
}