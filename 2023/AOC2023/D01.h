#pragma once


namespace D01
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);
    int sumPart1 = 0;
    int sumPart2 = 0;

    std::regex re { R"([\d]|one|two|three|four|five|six|seven|eight|nine)" };
    std::vector<std::string> digs = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
    for (auto line : lines)
    {
      auto firstPart1 = *std::ranges::find_if(line, [](auto c){ return std::isdigit(c); }) - '0';
      auto lastPart1  = *std::ranges::find_if(std::views::reverse(line), [](auto c){ return std::isdigit(c); }) - '0';

      auto firstPart2 = 0;
      auto lastPart2 = 0;

      bool firstMatch = true;

      for (auto match : FindAllMatchesOverlapping(re, line))
      {
        auto ms = match[0].str();
        int v = std::isdigit(ms[0]) ? (ms[0] - '0') : int(std::ranges::find(digs, ms) - digs.begin());

        if (firstMatch)
        {
          firstPart2 = v;
          firstMatch = false;
        }

        lastPart2 = v;
      }

      sumPart1 += firstPart1 * 10 + lastPart1;
      sumPart2 += firstPart2 * 10 + lastPart2;
    }

    PrintFmt("Part 1: {}\n", sumPart1);
    PrintFmt("Part 2: {}\n", sumPart2);
  }
}