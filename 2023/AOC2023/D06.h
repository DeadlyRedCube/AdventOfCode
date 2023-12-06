#pragma once

namespace D06
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    UnboundedArray<s64> times;
    for (auto str : Split(lines[0].substr(lines[0].find(':') + 1), " ", KeepEmpty::No))
      { times.Append(atoll(str.c_str())); }

    UnboundedArray<s64> dists;
    for (auto str : Split(lines[1].substr(lines[1].find(':') + 1), " ", KeepEmpty::No))
      { dists.Append(atoll(str.c_str())); }

    struct Race
    {
      s64 time;
      s64 dist;
    };

    UnboundedArray<Race> races;
    for (auto i = 0; i < times.Count(); i++)
      { races.Append({ times[i], dists[i] }); }

    int prod = 1;
    for (auto &race : races)
    {
      int winCount = 0;
      for (int i = 1; i < race.time; i++)
      {
        int dist = i * (race.time - i);
        if (dist > race.dist)
          { winCount++; }
      }

      PrintFmt("Win: {}\n", winCount);
      prod *= winCount;
    }

    PrintFmt("Part 1: {}\n", prod);
  }
}