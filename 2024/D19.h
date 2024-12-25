#pragma once


namespace D19
{
  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto lines = ReadFileLines(filePath);

    // Make a mapping from the valid characters to '0' through '5' (why these and not just 0 - 5? because I didn't feel
    //  like moving out of strings and 0 is the null terminator)
    char chmap[256] {0};
    chmap['w'] = '0';
    chmap['u'] = '1';
    chmap['b'] = '2';
    chmap['r'] = '3';
    chmap['g'] = '4';

    // Build the trie (*is* this a trie? it is definitely my distantly half-remembered concept of one)
    struct TrieNode
    {
      std::array<s32, 5> children { -1, -1, -1, -1, -1 };
      bool validTerminal = false;
    };

    std::vector<TrieNode> nodes;

    // Start with a root node.
    nodes.emplace_back();

    size_t maxPatLength = 0;
    for (auto pat : Split(lines[0], " ,", KeepEmpty::No))
    {
      maxPatLength = std::max(maxPatLength, pat.size());
      TrieNode *cur = &nodes[0];

      // Scan all the way to the end of this pattern, adding nodes as necessary.
      for (auto i : pat | std::views::transform([&](char c) { return chmap[c] - '0'; }))
      {
        if (cur->children[i] < 0)
        {
          auto newIndex = s32(nodes.size());
          cur->children[i] = newIndex;
          nodes.emplace_back();
          cur = &nodes[newIndex];
        }
        else
          { cur = &nodes[cur->children[i]]; }
      }

      // This was the node at the end of the pattern so it's a valid place to stop.
      cur->validTerminal = true;
    }

    // Run all of our designs through the character remapping (and get the max string length to preallocate our vector)
    size_t maxLen = 0;
    for (auto &line : lines  | std::views::drop(2))
    {
      maxLen = std::max(maxLen, line.size());
      for (auto &c : line)
        { c = chmap[c]; }
    }

    std::vector<s64> waysToSucceedFromHere;
    waysToSucceedFromHere.resize(maxLen + 1);

    for (auto &design : lines | std::views::drop(2))
    {
      // Start with no ways to succeed from anywhere except literally off the end (if we reach the end, that's good!)
      std::ranges::fill(waysToSucceedFromHere, 0);
      waysToSucceedFromHere[design.size()] = 1;

      // Starting with the very last character in the design, see how many ways there are to match in the trie from
      //  here.
      for(s32 startIndex = s32(design.size() - 1); startIndex >= 0; startIndex--)
      {
        TrieNode *cur = &nodes[0];
        for (auto [i, c] : design
          | std::views::enumerate
          | std::views::drop(startIndex))
        {
          // If the cur trie node has no entry for this character we can't match.
          if (cur->children[c - '0'] < 0)
            { break; }

          cur = &nodes[cur->children[c - '0']];

          // If this is a valid place to end, count up how many ways to succeed there are starting just after the end
          //  of the matched value (since we start at the end and move backwards, we will fill in above as we go).
          if (cur->validTerminal)
            { waysToSucceedFromHere[startIndex] += waysToSucceedFromHere[i + 1]; }
        }
      }

      p1 += s32(waysToSucceedFromHere[0] != 0); // p1 just asks "is there *any* way to succeed?
      p2 += waysToSucceedFromHere[0]; // p2 counts up all the ways.
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}