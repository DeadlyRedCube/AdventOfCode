#pragma once


namespace D01
{
  int FindFirstDigitOrName(const std::string &s)
  {
    std::string sub = s;
    while (sub.length() > 0)
    {
      if (sub.starts_with("zero"))
        { return 0; }
      if (sub.starts_with("one"))
        { return 1; }
      if (sub.starts_with("two"))
        { return 2; }
      if (sub.starts_with("three"))
        { return 3; }
      if (sub.starts_with("four"))
        { return 4; }
      if (sub.starts_with("five"))
        { return 5; }
      if (sub.starts_with("six"))
        { return 6; }
      if (sub.starts_with("seven"))
        { return 7; }
      if (sub.starts_with("eight"))
        { return 8; }
      if (sub.starts_with("nine"))
        { return 9; }
      if (std::isdigit(sub[0]))
        { return sub[0] - '0'; }

      sub = sub.substr(1);
    }

    ASSERT(false);
    return -1;
  }


  int FindLastDigitOrName(const std::string &s)
  {
    auto rev = s;
    std::ranges::reverse(rev);
    std::string sub = rev;
    while (sub.length() > 0)
    {
      if (sub.starts_with("orez"))
        { return 0; }
      if (sub.starts_with("eno"))
        { return 1; }
      if (sub.starts_with("owt"))
        { return 2; }
      if (sub.starts_with("eerht"))
        { return 3; }
      if (sub.starts_with("ruof"))
        { return 4; }
      if (sub.starts_with("evif"))
        { return 5; }
      if (sub.starts_with("xis"))
        { return 6; }
      if (sub.starts_with("neves"))
        { return 7; }
      if (sub.starts_with("thgie"))
        { return 8; }
      if (sub.starts_with("enin"))
        { return 9; }
      if (std::isdigit(sub[0]))
        { return sub[0] - '0'; }

      sub = sub.substr(1);
    }

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