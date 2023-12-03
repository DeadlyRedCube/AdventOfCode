#pragma once


namespace D01
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);
    int sumPart1 = 0;
    int sumPart2 = 0;

    std::regex re { R"([\d]|one|two|three|four|five|six|seven|eight|nine)" };
    UnboundedArray<std::string> digs
      = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    for (auto line : lines)
    {
      auto firstPart1 = *std::ranges::find_if(line, [](auto c){ return std::isdigit(c); }) - '0';
      auto lastPart1  = *std::ranges::find_if(std::views::reverse(line), [](auto c){ return std::isdigit(c); }) - '0';

      auto firstPart2 = digs.FindFirst(FindFirstMatch(re, line)->str()) % 10;
      auto lastPart2 = digs.FindFirst(FindLastMatch(re, line)->str()) % 10;

      sumPart1 += firstPart1 * 10 + lastPart1;
      sumPart2 += int(firstPart2 * 10 + lastPart2);
    }

    PrintFmt("Part 1: {}\n", sumPart1);
    PrintFmt("Part 2: {}\n", sumPart2);
  }
}