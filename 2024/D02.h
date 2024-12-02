#pragma once


namespace D02
{
  template <typename T>
  bool IsSafe(T &&r)
  {
    if (!std::ranges::is_sorted(r, std::less{}) && !std::ranges::is_sorted(r, std::greater{}))
      { return false; }

    for (auto [a, b] : r | std::views::pairwise)
    {
      auto d = std::abs(a - b);
      if (d < 1 || d > 3)
        { return false; }
    }

    return true;
  }


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    std::vector<std::vector<s64>> entries = lines
      | std::views::transform(
        [](auto &line)
        {
          return Split(line, " ", KeepEmpty::No)
            | std::views::transform([](const auto &str) { char *end; return s64(std::strtoll(str.c_str(), &end, 10)); })
            | std::ranges::to<std::vector>();
        })
      | std::ranges::to<std::vector>();

    s32 p1 = 0;
    s32 p2 = 0;
    for (auto &entry : entries)
    {
      bool safe = IsSafe(entry) ? 1 : 0;
      p1 += safe;
      p2 += safe;

      if (safe)
        { continue; }

      // For p2 if it was not safe then we test every removal
      for (u32 i = 0; i < entry.size(); i++)
      {
        if (
          IsSafe(
            std::views::zip(std::views::iota(size_t(0), entry.size()), entry)
              | std::views::filter([&](auto v) { return std::get<0>(v) != i; })
              | std::views::transform([](auto v) { return std::get<1>(v); })))
        {
          p2++;
          break;
        }
      }
    }

    PrintFmt("Part 1: {}\n", p1);
    PrintFmt("Part 2: {}\n", p2);
  }
}