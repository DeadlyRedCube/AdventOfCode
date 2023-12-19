#pragma once

namespace D06
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    UnboundedArray<s64> times;
    std::string tStr = lines[0].substr(lines[0].find(':') + 1);
    for (auto str : Split(tStr, " ", KeepEmpty::No))
      { times.Append(atoll(str.c_str())); }

    UnboundedArray<s64> dists;
    std::string dStr = lines[1].substr(lines[1].find(':') + 1);
    for (auto str : Split(dStr, " ", KeepEmpty::No))
      { dists.Append(atoll(str.c_str())); }

    std::erase(tStr, ' ');
    std::erase(dStr, ' ');
    s64 p2t = std::atoll(tStr.c_str());
    s64 p2d = std::atoll(dStr.c_str());

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
        s64 dist = i * (race.time - i);
        if (dist > race.dist)
          { winCount++; }
      }

      prod *= winCount;
    }

    PrintFmt("Part 1: {}\n", prod);

    // The actual solution times are a quadratic equation so solve it via the quadratic formula to get the solution
    //  points, then clamp them to the contained integral range to get the final answer.
    double v = std::sqrt(double(p2t)*double(p2t) - 4.0*double(p2d));
    double t0 = (double(p2t) - v) * 0.5;
    double t1 = (double(p2t) + v) * 0.5;
    s64 t0i = s64(std::ceil(t0));
    s64 t1i = s64(std::floor(t1));
    PrintFmt("Part 2: {}\n", t1i - t0i + 1);
  }
}