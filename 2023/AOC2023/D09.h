#pragma once

namespace D09
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    ssz sum = 0;
    s64 sump2 = 0;
    for (auto line : lines)
    {
      auto nums = Split(line, " ", KeepEmpty::No) | std::views::transform([](auto &&s) { return s64(std::atoi(s.c_str())); }) | std::ranges::to<std::vector>();

      UnboundedArray<UnboundedArray<s64>> histories;
      histories.AppendNew().AppendMultiple(ArrayView{&nums[0], ssz(nums.size()) });

      for (;;)
      {
        auto &newHist = histories.AppendNew();
        auto &prevHist = histories[FromEnd(-2)];

        for (s64 i = 0; i < prevHist.Count() - 1; i++)
        {
          newHist.Append(prevHist[i + 1] - prevHist[i]);
        }

        if (std::ranges::all_of(newHist, [](auto &&v) { return v == 0; }))
          { break; }
      }

      histories[FromEnd(-1)].Append(0);

      for (ssz i = histories.Count() - 2; i >= 0; i--)
      {
        histories[i].Append(histories[i][FromEnd(-1)] + histories[i + 1][FromEnd(-1)]);
      }

      PrintFmt("Last: {}\n", histories[0][FromEnd(-1)]);
      sum += histories[0][FromEnd(-1)];

      histories[FromEnd(-1)].Insert(0, 0);

      for (ssz i = histories.Count() - 2; i >= 0; i--)
      {
        histories[i].Insert(0, histories[i][0] - histories[i + 1][0]);
      }

      PrintFmt("LastP2: {}\n", histories[0][0]);
      sump2 += histories[0][0];
    }

    PrintFmt("Part 1: {}\n", sum);
    PrintFmt("Part 2: {}\n", sump2);
  }
}