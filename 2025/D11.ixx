export module D11;

import Util;

export namespace D11
{
  void Run(const char *path)
  {
    enum class Moves { None = 0x00, DAC = 0x01, FFT = 0x02, All = 0x03 };

    struct Node
    {
      std::vector<std::string> outputs;

      // For each node we keep a tally of how many paths to "out" are there under this that:
      //  0: don't pass through DAC or FFT
      //  1: pass through only DAC
      //  2: pass through only FFT
      //  3: pass through both DAC and FFT
      // (These correspond to the "Moves" enum values above)
      std::optional<std::array<s64, 4>> tallies;
    };

    // Parse!
    std::map<std::string, Node> graph;
    for (auto line : ReadFileLines(path))
    {
      auto vals = Split(line, " :", KeepEmpty::No);
      graph[vals[0]].outputs = vals | std::views::drop(1) | std::ranges::to<std::vector>();
    }

    // Out counts as "one way to get to out" (and does not have dac or fft under it
    graph["out"].tallies = {1, 0, 0, 0 };

    auto Traverse =
      [&](const std::string &startName)
      {
        // Find our starting node (and mark it as prev so we know how to start the non-recursive-but-totally-recursive
        //  traversal)
        auto prev = graph.find(startName);
        if (prev->second.tallies.has_value())
          { return; }

        // track where we are in traversal so we can propagate moves back up.
        std::vector<decltype(prev)> depth;
        depth.push_back(prev);

        auto PropagateTallies = // lambdas on lambdas up in here
          [](auto &src, auto &dst)
          {
            // If the element we're propagating into is one of the special ones, we want to escalate any moves from
            //  below them up into the "these moves are under the special one" tally slots
            Moves moves = Moves::None;
            if (dst->first == "dac")
              { moves |= Moves::DAC; }
            else if (dst->first == "fft")
              { moves |= Moves::FFT; }

            auto &srcTallies = *src->second.tallies;
            auto &dstTallies = *dst->second.tallies;
            for (s32 i = 0; i < 4; i++)
              { dstTallies[usz(s32(moves) | i)] += srcTallies[usz(i)]; }
          };

        while (true)
        {
          auto &cur = depth.back();

          // figure out what the next node is from here
          auto nextName = std::string{};
          if (prev == cur)
            { nextName = cur->second.outputs[0]; } // Prev was this node so start at the first child
          else
          {
            // Get the next entry in the node and then the node after it (which is where we're going next)
            auto it = std::ranges::find(cur->second.outputs, prev->first);
            ++it;
            if (it == cur->second.outputs.end())
            {
              // There's no next node so move back up the tree
              prev = cur;
              depth.pop_back();
              if (depth.empty())
                { break; } // No more stack, we're done!
              PropagateTallies(prev, depth.back());
              continue;
            }

            nextName = *it;
          }

          auto next = graph.find(nextName);

          // Update prev so that our next loop starts from here.
          prev = next;

          if (next->second.tallies.has_value())
          {
            // We already spotted this node so we can just use its existing tallies
            PropagateTallies(next, cur);
            continue;
          }

          // Initialize its tallies so that they're in place when we need them.
          next->second.tallies = { 0, 0, 0, 0 };

          // Time to "recurse" into this node
          depth.push_back(next);
        }
      };

    Traverse("you");
    PrintFmt("Part 1: {}\n", *std::ranges::fold_left_first(*graph["you"].tallies, std::plus{}));

    Traverse("svr");
    PrintFmt("Part 2: {}\n", (*graph["svr"].tallies)[usz(Moves::All)]);
  }
}