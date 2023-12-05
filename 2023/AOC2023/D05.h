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

    for (auto line : lines)
    {
      if (line.empty())
        { continue; }

      if (line.starts_with("seeds"))
      {
        for (auto seed : Split(line.substr(line.find(':') + 1), " ", KeepEmpty::No))
         {  seeds.Append(std::atoll(seed.c_str())); }
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
    PrintFmt("Min: {}\n", minLoc);
  }
}