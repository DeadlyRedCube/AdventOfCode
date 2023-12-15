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

    PrintFmt("Part 1: {}\n", sum);

    struct S
    {
      std::string label;
      s64 value;
    };
    FixedArray<UnboundedArray<S>, 256> boxes;

    for (auto &entry : splits)
    {
      bool set = entry.contains('=');
      auto vars = Split(entry, "=-", KeepEmpty::No);
      auto label = vars[0];

      auto hash = Hash(label);
      auto &box = boxes[hash];

      if (set)
      {
        auto value = StrToNum(vars[1]);
        if (auto i = box.FindFirst([&label](auto &&e) { return e.label == label; }); i >= 0)
          { box[i].value = value; }
        else
          { box.Append({ label, value }); }
      }
      else
      {
        if (auto i = box.FindFirst([&label](auto &&e) { return e.label == label; }); i >= 0)
          { box.RemoveAt(i); }
      }
    }

    s64 total = 0;
    for (s32 i = 0; i < boxes.Count(); i++)
    {
      auto &box = boxes[i];

      if (box.IsEmpty())
        { continue; }

      for (auto b = 0; b < box.Count(); b++)
      {
        auto &e = box[b];
        auto power = (i + 1) * (b + 1) * e.value;
        total += power;
      }
    }

    PrintFmt("Part 2: {}\n", total);
  }
}