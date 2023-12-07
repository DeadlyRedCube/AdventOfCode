#pragma once

namespace D07
{
  template <typename T>
  bool SortHands(const T &a, const T &b)
  {
    // If we have an unequal first card count, use that result to sort.
    if (auto cmp = (a.groupedCards[0].count <=> b.groupedCards[0].count); cmp != 0)
      { return cmp == std::strong_ordering::greater; }

    // It's possible we have matching 5s due to Jokers, so make sure we have enough cards here to check
    if (a.groupedCards.Count() > 1 && b.groupedCards.Count() > 1)
    {
      // If the second card count is unequal, use that result to sort.
      if (auto cmp = (a.groupedCards[1].count <=> b.groupedCards[1].count); cmp != 0)
        { return cmp == std::strong_ordering::greater; }
    }

    // If we got here then the hands are the same type, sort by unsorted card rank even though that's absolutely
    //  not what I did on the first attempt.
    for (int i = 0; i < 5; i++)
    {
      if (auto cmp = (a.unsortedRanks[i] <=> b.unsortedRanks[i]); cmp != 0)
        { return cmp == std::strong_ordering::greater; }
    }

    ASSERT(false);
    return false;
  }


  void Run(const char *path)
  {
    struct CardGrouping
    {
      char c;
      int count = 0;
    };

    struct Hand
    {
      UnboundedArray<CardGrouping> groupedCards;
      int bid;
      UnboundedArray<int> unsortedRanks;
    };

    std::map<char, int> cardRanks =
    {
      {'2', 2 }, {'3', 3 }, {'4', 4 }, {'5', 5 },{'6', 6 },{'7', 7 }, {'8', 8 }, {'9', 9 }, {'T', 10 },
      {'J', 11 } ,{'Q', 12 } , {'K', 13 }, {'A', 14 }
    };

    UnboundedArray<Hand> hands;
    for (const auto &line : ReadFileLines(path))
    {
      auto split = Split(line, " ", KeepEmpty::No);

      Hand &h = hands.AppendNew();
      h.bid = std::atoi(split[1].c_str());

      // Group our cards by type
      for (auto c : split[0])
      {
        // Increment the card count if we find a duplicate card, otherwise add a new card with a count of 1.
        if (auto cardIndex = h.groupedCards.FindFirst([c](auto crd) { return crd.c == c; }); cardIndex >= 0)
          { h.groupedCards[cardIndex].count++; }
        else
          { h.groupedCards.Append({ .c = c, .count = 1 }); }

        // We also need to keep track of the card ranks in unsorted order.
        h.unsortedRanks.Append(cardRanks[c]);
      }

      // Sort card groupings by counts (largest count wins, card rank doesn't matter).
      std::ranges::sort(h.groupedCards, [](auto a, auto b) { return a.count > b.count; });
    }

    // Now sort the hands
    std::ranges::sort(hands, SortHands<Hand>);

    s64 sum = 0;
    for (int i = 0; i < hands.Count(); i++)
      { sum += hands[i].bid * (hands.Count() - i); }
    PrintFmt("Part 1: {}\n", sum);

    // For part 2, jokers are wild so take any joker groups and add their count to the best non-joker card group.
    for (auto &h : hands)
    {
      if (h.groupedCards.Count() > 1)
      {
        if (auto jokerIndex = h.groupedCards.FindFirst([](auto c) { return c.c == 'J'; }); jokerIndex >= 0)
        {
          h.groupedCards[(jokerIndex == 0) ? 1 : 0].count += h.groupedCards[jokerIndex].count;
          h.groupedCards[jokerIndex].count = 0;
          std::ranges::sort(h.groupedCards, [](auto a, auto b) { return a.count > b.count; });
        }
      }

      // Jokers are now the worst-ranked card.
      std::ranges::replace(h.unsortedRanks, cardRanks['J'], 1);
    }

    // Re-sort (will put joker groups at the end as they now have a count of 0)
    std::ranges::sort(hands, SortHands<Hand>);

    // Same sum process as part 1.
    sum = 0;
    for (int i = 0; i < hands.Count(); i++)
      { sum += hands[i].bid * (hands.Count() - i); }
    PrintFmt("Part 2: {}\n", sum);
  }
}
