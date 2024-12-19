#pragma once


namespace D19
{
  using Vec = Vec2<s64>;

  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();

  struct TrieNode
  {
    bool validTerminal = false;
    std::map<char, TrieNode> children;
  };

  s64 CountWays(std::string_view design, TrieNode *trie, std::vector<std::optional<s64>> &memo)
  {
    // If there's nothing left, there's one way to finish from here (it's finished)
    if (design.empty())
      { return 1; }


    // If we've already calculated this one, return the existing calculation.
    auto &countDest = memo[design.size()];
    if (countDest.has_value())
      { return *countDest; }

    countDest = 0;
    s64 &wayCount = *countDest;

    // Now scan through the trie looking for ways to proceed.
    TrieNode *cur = trie;
    for (auto [i, c] : design | std::views::enumerate)
    {
      // Find the entry for this character
      auto found = cur->children.find(c);
      if (found == cur->children.end())
        { return wayCount; } // Didn't find one so this is the end, return whatever number of ways we calculated.
      cur = &(*found).second;

      // If this is a valid ending point, recurse and find the number of ways valid from here.
      if (cur->validTerminal)
        { wayCount += CountWays(design.substr(i + 1), trie, memo); }
    }

    // Done!
    return wayCount;
  }


  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto lines = ReadFileLines(filePath);

    // Build the trie (*is* this a trie? it is definitely my distantly half-remembered concept of one)
    TrieNode root;
    for (auto pat : Split(lines[0], " ,", KeepEmpty::No))
    {
      TrieNode *cur = &root;

      // Scan all the way to the end of this pattern
      for (auto c : pat)
        { cur = &cur->children[c]; }

      // This is a valid ending spot.
      cur->validTerminal = true;
    }

    std::vector<std::optional<s64>> memo;
    for (auto &design : lines | std::views::drop(2))
    {
      memo.clear();
      memo.resize(design.size() + 1);
      auto count = CountWays(design, &root, memo);
      p1 += (count > 0) ? 1 : 0; // p1 just cares about whether it can be made any way.
      p2 += count;
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}