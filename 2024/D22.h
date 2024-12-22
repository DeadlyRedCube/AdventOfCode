#pragma once


namespace D22
{
  using Vec = Vec2<s32>;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();

  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto lines = ReadFileLines(filePath);

    std::map<std::array<s64, 4>, s64> changes;

    for (auto &line : lines)
    {
      auto secretNum = AsS64(line);

      std::array<s64, 4> prevs;
      std::set<std::array<s64, 4>> set;
      for (u32 i = 0; i < 4; i++)
      {
        s64 orig = secretNum;
        secretNum = (secretNum ^ (secretNum * 64)) % 16777216;
        secretNum = (secretNum ^ (secretNum / 32)) % 16777216;
        secretNum = (secretNum ^ (secretNum * 2048)) % 16777216;
        prevs[i] = (secretNum % 10) - (orig % 10);
      }

      changes[prevs] += (secretNum % 10);
      set.insert(prevs);

      for (u32 i = 4; i < 2000; i++)
      {
        for (u32 j = 0; j < 3; j++)
          { prevs[j] = prevs[j + 1]; }

        s64 orig = secretNum;
        secretNum = (secretNum ^ (secretNum * 64)) % 16777216;
        secretNum = (secretNum ^ (secretNum / 32)) % 16777216;
        secretNum = (secretNum ^ (secretNum * 2048)) % 16777216;
        prevs[3] = (secretNum % 10) - (orig % 10);

        if (!set.contains(prevs))
        {
          changes[prevs] += (secretNum % 10);
          set.insert(prevs);
        }
      }

      p1 += secretNum;
    }


    for (auto &&[k, v] : changes)
      { p2 = std::max(p2, v); }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}