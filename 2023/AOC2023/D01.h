#pragma once


namespace D01
{
  constexpr const char *digitWords[] =
    { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

  template <bool forward>
  std::optional<std::string> SearchPredicate(const std::string_view &v)
  {
    usz idx = forward ? 0 : (v.length() - 1);
    if (std::isdigit(v[idx]))
      { return std:: string { v, idx, 1 }; }
    for (ssz i = 0; i < 10; i++)
    {
      if (forward ? v.starts_with(digitWords[i]) : v.ends_with(digitWords[i]))
      {
        char chs[] = { char(i) + '0', 0 };
        return chs;
      }
    }

    return std::nullopt;
  }

  int FindFirstDigitOrName(const std::string_view &s)
  {
    if (auto res = FindFirstMatch(s, SearchPredicate<true>); res.has_value())
      { return res->match[0] - '0'; }

    ASSERT(false);
    return -1;
  }


  int FindLastDigitOrName(const std::string &s)
  {
    if (auto res = FindLastMatch(s, SearchPredicate<false>); res.has_value())
      { return res->match[0] - '0'; }

    ASSERT(false);
    return -1;
  }

  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);
    int sumPart1 = 0;
    int sumPart2 = 0;
    for (auto line : lines)
    {
      auto firstPart1 = *std::ranges::find_if(line, [](auto c){ return std::isdigit(c); }) - '0';
      auto lastPart1  = *std::ranges::find_if(std::views::reverse(line), [](auto c){ return std::isdigit(c); }) - '0';

      auto firstPart2 = FindFirstDigitOrName(line);
      auto lastPart2 = FindLastDigitOrName(line);

      sumPart1 += firstPart1 * 10 + lastPart1;
      sumPart2 += firstPart2 * 10 + lastPart2;
    }

    PrintFmt("Part 1: {}\n", sumPart1);
    PrintFmt("Part 2: {}\n", sumPart2);
  }
}