#pragma once


namespace D05
{
  void Run(const char *path)
  {
    s32 p1 = 0;
    s32 p2 = 0;

    std::unordered_map<s32, std::vector<s32>> rules;

    struct V
    {
      s32 i;
      s32 v;
    };

    std::vector<V> values;
    values.reserve(64);

    // Parse the first part (with '|'s) as a map and thesecond part as lists.
    auto lines = ReadFileLines(path);
    usz lineIndex = 0;
    for (; !lines[lineIndex].empty(); ++lineIndex)
    {
      auto &line = lines[lineIndex];
      rules[AsS32(line)].push_back(AsS32(&line[line.find_first_of('|') + 1]));
    }

    for (++lineIndex; lineIndex < lines.size(); lineIndex++)
    {
      auto &line = lines[lineIndex];
      values.clear();
      std::string_view v = line;
      while (true)
      {
        values.push_back({ .i = s32(values.size()), .v = AsS32(v.data()) });
        auto nextComma = v.find_first_of(',');
        if (nextComma == std::string_view::npos)
          { break; }
        v = v.substr(nextComma + 1);
      }

      // A page is good if none of the numbers *before* a given number are in the rules list for that latter number.
      std::ranges::sort(values, [&](V l, V r) { return std::ranges::contains(rules[l.v], r.v); });
      if (std::ranges::all_of(std::views::enumerate(values), [](const auto &tup) { return std::get<0>(tup) == std::get<1>(tup).v; }))
        { p1 += values[values.size() / 2].v; } // p1 is just tally up the good pages
      else
        { p2 += values[values.size() / 2].v; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}
