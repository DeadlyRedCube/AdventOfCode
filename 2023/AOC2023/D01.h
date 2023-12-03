#pragma once


int D01FindFirstDigitOrName(const std::string &s)
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


int D01FindLastDigitOrName(const std::string &s)
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

void D01(const char *path)
{
  auto lines = ReadFileLines(path);
  int sum = 0;
  for (auto line : lines)
  {
    auto firstDig = D01FindFirstDigitOrName(line);
    auto lastDig = D01FindLastDigitOrName(line);

    int num = firstDig * 10 + lastDig;
    PrintFmt("{}\n", num);
    sum += num;
  }

  PrintFmt("\nSUM: {}\n", sum);
}
