#pragma once


namespace D25
{
  void Run(const char *filePath)
  {
    s64 p1 = 0;

    auto locks = std::vector<std::array<s32, 5>>{};
    auto keys = std::vector<std::array<s32, 5>>{};

    char openChar {};
    std::array<s32, 5> *cur = nullptr;
    for (auto &line : ReadFileLines(filePath))
    {
      if (line.empty())
      {
        cur = nullptr;
        continue;
      }

      if (cur == nullptr)
      {
        if (line[0] == '#')
        {
          openChar = '.';
          cur = &locks.emplace_back();
        }
        else
        {
          openChar = '#';
          cur = &keys.emplace_back();
        }

        continue;
      }

      for (auto &&[h, ch] : std::views::zip(*cur, line))
        { h += s32(ch == openChar); }
    }

    for (auto &lock : locks)
    {
      p1 += Sum(
        keys
        | std::views::transform(
          [&](auto &key)
          {
            return std::ranges::all_of(
              std::views::zip_transform([](s32 lh, s32 kh) { return lh >= kh; }, lock, key),
              std::identity{}) ? 1 : 0;
          }));
    }

    PrintFmt("P1: {}\n", p1);
  }
}