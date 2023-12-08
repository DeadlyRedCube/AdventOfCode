#pragma once

namespace D08
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    struct Node
    {
      std::string name;
      std::string left;
      std::string right;
    };

    std::map<std::string, Node> nodes;

    auto instructions = lines[0];
    for (auto p : lines | std::views::drop(2))
    {
      auto split = Split(p, " =(,)", KeepEmpty::No);
      ASSERT(split.size() == 3);

      Node n = { .name = split[0], .left = split[1], .right = split[2] };
      nodes[split[0]] = n;
    }

    s64 stepCount = 0;
    std::string cur = "AAA";
    while (true)
    {
      char i = instructions[stepCount % instructions.size()];
      auto &node = nodes[cur];
      stepCount++;
      cur = (i == 'L') ? node.left : node.right;
      if (cur == "ZZZ")
        { break; }
    }

    PrintFmt("Part 1: {}\n", stepCount);
  }
}