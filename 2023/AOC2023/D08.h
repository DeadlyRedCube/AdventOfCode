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
    for (auto p : lines | std::views::drop(2)) // First line is instructions, second line is empty
    {
      auto split = Split(p, " =(,)", KeepEmpty::No);
      ASSERT(split.size() == 3);

      Node n = { .name = split[0], .left = split[1], .right = split[2] };
      nodes[split[0]] = n;
    }

    if (nodes.contains("AAA") && nodes.contains("ZZZ"))
    {
      s64 stepCount = 0;
      std::string cur = "AAA";
      while (true)
      {
        // Grab the instruction and bump the step count.
        char i = instructions[stepCount % instructions.size()];
        stepCount++;

        // Go left or right based on the instruction
        cur = (i == 'L') ? nodes[cur].left : nodes[cur].right;

        // If we landed on "ZZZ" we're done
        if (cur == "ZZZ")
          { break; }
      }

      PrintFmt("Part 1: {}\n", stepCount);
    }

    {
      // Find all the nodes that end with 'A' as our starting points.
      auto curNodes = nodes
        | std::views::transform([](auto &&a) { return a.first; })
        | std::views::filter([](auto &&s) { return s.ends_with('A'); })
        | std::ranges::to<std::vector>();

      struct Loop
      {
        s64 startIndex;
        s64 length;
        UnboundedArray<s64> zIndices;
      };

      UnboundedArray<Loop> loops;
      for (const auto &start : curNodes)
      {
        auto node = start;
        struct LoopTests
        {
          std::string nodeName;
          s64 stepCount;
        };
        UnboundedArray<LoopTests> firsts;

        auto &loop = loops.AppendNew();
        s64 stepCount = 0;
        while (true)
        {
          if (node.ends_with('Z'))
          {
            // In my answer, there was only one Z ending per loop so I did not bother solving the more complex case
            loop.zIndices.Append(stepCount);
          }

          auto inst = instructions[stepCount % instructions.size()];
          node = (inst == 'L') ? nodes[node].left : nodes[node].right;
          stepCount++;
          if (stepCount % instructions.size() == 0)
          {
            // At the end of each instruction list (when the instruction list loops), we see if we've landed on a node
            //  we already landed on at the end of a previous end-of-instruction-list. Once we hit one, we know that
            //  we're in a loop from then on.
            if (ssz targetIndex = firsts.FindFirst([&node](auto &&a) { return a.nodeName == node; }); targetIndex >= 0)
            {
              loop.startIndex = firsts[targetIndex].stepCount;
              loop.length = stepCount - firsts[targetIndex].stepCount;
              break;
            }

            // Hadn't seen this one before so add it into the list (with its step count so we can know where the loop
            //  started when we get back to it).
            firsts.Append({ .nodeName = node, .stepCount = stepCount});
          }
        }
      }

      // Now we can iterate through using modular arithmetic
      UnboundedArray<s64> zIndices = loops[0].zIndices;

      s64 count = loops[0].length;

      for (auto &loop : ArrayView{loops, 1, ToEnd})
      {
        UnboundedArray<s64> zScratch;
        auto lcm = LeastCommonMultiple(count, loop.length);

        for (const auto outerZIndex : zIndices)
        {
          for (const auto innerZIndex : loop.zIndices)
          {
            auto zIndex = outerZIndex;
            s64 inc = 0;
            for (;;)
            {
              // Check where our zIndex is modulo the current loop
              auto mod = (zIndex - loop.startIndex) % loop.length + loop.startIndex;
              if (mod == innerZIndex)
              {
                zScratch.Append(zIndex);
                // The zIndices match, so they will next match again on the least common multiple of the current count and
                //  the loop length.
                // Every iteration of the loop increases the count so that it's the interval that all of the previous
                //  loops had Zs at once.
                break;
              }

              // No match so increase by the count.
              zIndex += count;
              inc += count;
              if (inc >= lcm)
                { break; } // Doesn't ever intersect
            }
          }
        }

        count = LeastCommonMultiple(count, loop.length);
        std::swap(zIndices, zScratch);
      }

      // We finally found a perfect overlap!
      PrintFmt("Part 2: {}\n", std::ranges::min(zIndices));
    }
  }
}