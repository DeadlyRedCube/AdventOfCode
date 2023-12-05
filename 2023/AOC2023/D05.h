#pragma once

namespace D05
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    struct MapEntry
    {
      int64_t destStart;
      int64_t srcStart;
      int64_t length;
    };

    UnboundedArray<UnboundedArray<MapEntry>> maps;
    UnboundedArray<int64_t> seeds;

    struct Range
    {
      int64_t start;
      int64_t length;
    };

    UnboundedArray<Range> part2Ranges;

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
            { part2Ranges.Append({ seeds[FromEnd(-2)], seeds[FromEnd(-1)] }); }
        }
      }
      else if (std::isdigit(line[0]))
      {
        // If this starts with a digit it's a 3-number line that gets added, as an entry, into the most-recent
        //  maps entry.
        BoundedArray<int64_t, 3> es;
        for (auto e : Split(line, " ", KeepEmpty::No))
          { es.Append(std::atoll(e.c_str())); }

        maps[FromEnd(-1)].Append({ es[0], es[1], es[2] });
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
          if (target >= e.srcStart && target < e.srcStart + e.length)
          {
            // Found a match so update our target point and we can stop searching.
            target += (e.destStart - e.srcStart);
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
      { std::ranges::sort(m, {}, &MapEntry::srcStart); }

    // Now we're going to scan the ranges and see how they overlap each level of map entry to filter down to the
    //  final set.
    UnboundedArray<Range> scratch;
    for (auto map : maps)
    {
      scratch.SetCount(0);

      for (auto r : part2Ranges)
      {
        for (auto e : map)
        {
          if (r.start < e.srcStart)
          {
            // This starts with entries that don't correspond to any map entries for this level, so this goes in as
            //  a new range, clipped to the start of the current map entry.
            scratch.Append({ r.start, std::min(r.length, e.srcStart - r.start) });

            // Clip the area we just consumed off of this range.
            r.length -= scratch[FromEnd(-1)].length;
            r.start = e.srcStart;

            // If we've eaten up all of this range, we're done.
            if (r.length <= 0)
              { break; }
          }

          if (r.start < e.srcStart + e.length)
          {
            // Our range overlaps this map entry, so append the overlapped value (mapped into destination space)
            //  so it can be properly used in the next map pass.
            scratch.Append({ r.start - e.srcStart + e.destStart, std::min(r.length, e.srcStart + e.length - r.start) });

            // Clip the range and break if we're done with this range.
            r.length -= scratch[FromEnd(-1)].length;
            r.start = e.srcStart + e.length;
            if (r.length <= 0)
              { break; }
          }
        }

        if (r.length > 0)
        {
          // This range ended with entries that on't correspond to any map entries for this level, so it gets added
          //  as a new destination range directly (since we've already clipped the front of it as we went).
          scratch.Append(r);
        }
      }

      // Now swap in our ranges for the next pass.
      std::swap(scratch, part2Ranges);
    }

    // Now find the minimum starting range point, as that'll guaranteed be our first entry.
    minLoc = std::numeric_limits<int64_t>::max();
    for (auto &r : part2Ranges)
      { minLoc = std::min(minLoc, r.start); }
    PrintFmt("Part 2: {}\n", minLoc);
  }
}