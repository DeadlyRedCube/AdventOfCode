#pragma once


namespace D03
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;
    bool doMuls = true;

    for (auto line : ReadFileLines(path))
    {
      for (auto match : FindAllMatchesOverlapping(std::regex { R"((do\(\)|don't\(\)|mul\((\d+),(\d+)\)))" }, line))
      {
        char *end;
        if (match[0] == "do()")
          { doMuls = true; }
        else if (match[0] == "don't()")
          { doMuls = false; }
        else
        {
          auto l = std::strtoll(match[2].str().c_str(), &end, 10);
          auto r = std::strtoll(match[3].str().c_str(), &end, 10);
          p1 += l * r;
          if (doMuls)
            { p2 += l * r; }
        }
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}
