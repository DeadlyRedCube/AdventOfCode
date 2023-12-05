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

    struct OutRange
    {
      int64_t start;
      int64_t length;
    };

    UnboundedArray<OutRange> inRanges;
    for (auto line : lines)
    {
      if (line.empty())
        { continue; }

      if (line.starts_with("seeds"))
      {
        for (auto seed : Split(line.substr(line.find(':') + 1), " ", KeepEmpty::No))
        {
          seeds.Append(std::atoll(seed.c_str()));
          if (seeds.Count() % 2 == 0)
            { inRanges.Append({ seeds[FromEnd(-2)], seeds[FromEnd(-1)] }); }
        }
      }
      else if (std::isdigit(line[0]))
      {
        BoundedArray<int64_t, 3> es;
        for (auto e : Split(line, " ", KeepEmpty::No))
          { es.Append(std::atoll(e.c_str())); }

        maps[FromEnd(-1)].Append({ es[0], es[1], es[2] });
      }
      else
        { maps.AppendNew(); }
    }

    int64_t minLoc = std::numeric_limits<int64_t>::max();

    for (auto seed : seeds)
    {
      auto target = seed;
      for (auto &map : maps)
      {
        bool found = false;
        for (auto &e : map)
        {
          if (target >= e.srcStart && target < e.srcStart + e.length)
          {
            found = true;
            target += (e.destStart - e.srcStart);
            break;
          }
        }
      }

      minLoc = std::min(minLoc, target);
    }

    PrintFmt("Part 1: {}\n", minLoc);

    for (auto &m : maps)
      { std::ranges::sort(m, {}, &MapEntry::srcStart); }

    UnboundedArray<OutRange> scratch;
    for (auto map : maps)
    {
      scratch.SetCount(0);

      for (auto r : inRanges)
      {
        for (auto e : map)
        {
          if (r.start < e.srcStart)
          {
            // These have no correspondences
            scratch.Append({ r.start, std::min(r.length, e.srcStart - r.start) });
            r.length -= scratch[FromEnd(-1)].length;
            r.start = e.srcStart;
            if (r.length <= 0)
              { break; }
          }

          if (r.start < e.srcStart + e.length)
          {
            scratch.Append({ r.start - e.srcStart + e.destStart, std::min(r.length, e.srcStart + e.length - r.start) });
            r.length -= scratch[FromEnd(-1)].length;
            r.start = e.srcStart + e.length;
            if (r.length <= 0)
              { break; }
          }
        }

        if (r.length > 0)
        {
          // Leftover!
          scratch.Append(r);
        }
      }

      std::swap(scratch, inRanges);
    }

    minLoc = std::numeric_limits<int64_t>::max();
    for (auto &r : inRanges)
      { minLoc = std::min(minLoc, r.start); }
    PrintFmt("Part 2: {}\n", minLoc);
  }
}