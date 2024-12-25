#pragma once


namespace D03
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;
    bool doMuls = true;

    for (auto line : ReadFileLines(path) | std::views::transform([](auto &s) { return std::string_view(s); }))
    {
      while (!line.empty())
      {
        if (line.starts_with("do()"))
        {
          doMuls = true;
          line = line.substr(4);
        }
        else if (line.starts_with("don't()"))
        {
          doMuls = false;
          line = line.substr(7);
        }
        else if (line.starts_with("mul("))
        {
          line = line.substr(4);
          char *end;
          auto l = std::strtoll(line.data(), &end, 10);
          if (*end != ',')
            { continue; }
          line = line.substr(intptr_t(end - line.data()) + 1);
          auto r = std::strtoll(line.data(), &end, 10);
          if (*end != ')')
            { continue; }

          p1 += l * r;
          if (doMuls)
            { p2 += l * r; }
        }
        else
          { line = line.substr(1); }
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}
