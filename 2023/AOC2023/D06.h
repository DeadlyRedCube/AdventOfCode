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

    s64 p2t;
    s64 p2d;
    {
      std::string t;
      for (auto c : lines[0].substr(lines[0].find(':') + 1))
      {
        if (!std::isspace(c))
          { t += c; }
      }
      std::string d;
      for (auto c : lines[1].substr(lines[1].find(':') + 1))
      {
        if (!std::isspace(c))
          { d += c; }
      }

      p2t = std::atoll(t.c_str());
      p2d = std::atoll(d.c_str());
    }

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

    double v = std::sqrt(double(p2t)*double(p2t) - 4.0*double(p2d));
    double t0 = (double(p2t) - v) * 0.5;
    double t1 = (double(p2t) + v) * 0.5;

    s64 t0i = s64(std::ceil(t0));
    s64 t1i = s64(std::floor(t1));
    PrintFmt("Part 1: {}\n", prod);
    PrintFmt("Part 2: {}\n", t1i - t0i + 1);
  }
}