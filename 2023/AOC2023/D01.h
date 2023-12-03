#pragma once


void D01(const char *path)
{
  auto lines = ReadFileLines(path);
  int sum = 0;
  for (auto line : lines)
  {
    auto first = std::ranges::find_if(line, [](auto c){ return std::isdigit(c); });
    auto last = std::ranges::find_if(std::views::reverse(line), [](auto c){ return std::isdigit(c); });

    int num = (*first - '0') * 10 + (*last - '0');
    printf(std::format("{}\n", num).c_str());
    sum += num;
  }

  printf(std::format("\nSUM: {}\n", sum).c_str());
}
