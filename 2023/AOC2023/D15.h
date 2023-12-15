namespace D15
{
  u8 Hash(std::string_view v)
  {
    u8 currentV = 0;
    for (auto c : v)
    {
      currentV += c;
      currentV *= 17;
    }

    return currentV;
  }


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    auto splits = Split(lines[0], ",", KeepEmpty::No);

    s32 sum = 0;
    for (auto &split : splits)
    {
      sum += Hash(split);
    }

    PrintFmt("{}\n", sum);
  }
}