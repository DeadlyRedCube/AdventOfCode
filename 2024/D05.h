#pragma once


namespace D05
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    std::map<s64, std::vector<s64>> rules;
    std::vector<std::vector<s64>> pages;

    // Parse the first part (with '|'s) as a map and thesecond part as lists.
    for (auto line : ReadFileLines(path))
    {
      char *end;
      if (line.contains('|'))
      {
        auto toks = Split(line, "|", KeepEmpty::No);
        auto l = std::strtoll(toks[0].c_str(), &end, 10);
        auto r = std::strtoll(toks[1].c_str(), &end, 10);
        rules[l].push_back(r);
      }

      else if (line.contains(','))
      {
        auto &v = pages.emplace_back();
        for (auto i : Split(line, ",", KeepEmpty::No)
            | std::views::transform([&](auto &&s) { return std::strtoll(s.c_str(), &end, 10); }))
          { v.push_back(i); }
      }
    }

    for (auto &pg : pages)
    {
      // A page is good if none of the numbers *before* a given number are in the rules list for that latter number.
      bool good = true;
      for (size_t i = 1; good && i < pg.size(); i++)
      {
        for (size_t j = 0; good && j < i; j++)
        {
          if (std::ranges::contains(rules[pg[i]], pg[j]))
            { good = false; }
        }
      }

      if (good)
        { p1 += pg[pg.size() / 2]; } // p1 is just tally up the good pages
      else
      {
        // Use the rules to sort numbers - a number should come first if the latter's rule contains it.
        std::ranges::sort(pg, [&](s64 l, s64 r) { return std::ranges::contains(rules[r], l); });
        p2 += pg[pg.size() / 2];
      }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}
