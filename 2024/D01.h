#pragma once


namespace D01
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    UnboundedArray<s64> leftList, rightList;
    std::map<s64, s32> leftCounts;
    std::map<s64, s32> rightCounts;

    for (auto line : lines)
    {
      auto toks = Split(line, " ", KeepEmpty::No);
      char *end;
      s64 l = std::strtol(toks[0].c_str(), &end, 10);
      s64 r = std::strtol(toks[1].c_str(), &end, 10);
      leftList.Append(l);
      rightList.Append(r);
      leftCounts[l]++;
      rightCounts[r]++;
    }

    std::ranges::sort(leftList);
    std::ranges::sort(rightList);

    s64 dist = 0;
    for (s32 i = 0; i < leftList.Count(); i++)
      { dist += std::abs(leftList[i] - rightList[i]); }

    PrintFmt("Part 1: {}\n", dist);

    s64 dist2 = 0;
    for (auto [l, c] : leftCounts)
    {
      auto rc = rightCounts[l];
      dist2 += rc * l * c;
    }

    PrintFmt("Part 2: {}\n", dist2);
  }
}