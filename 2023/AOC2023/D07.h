#pragma once

namespace D07
{
  template <typename T>
  bool SortHands(const T &a, const T &b)
  {
    if (a.cards[0].count > b.cards[0].count)
      { return true; }

    if (a.cards[0].count < b.cards[0].count)
      { return false; }

    if (a.cards.Count() > 1 && b.cards.Count() > 1)
    {
      if (a.cards[1].count > b.cards[1].count)
        { return true; }

      if (a.cards[1].count < b.cards[1].count)
        { return false; }
    }

    for (int i = 0; i < 5; i++)
    {
      if (a.unsortedRanks[i] > b.unsortedRanks[i])
        { return true; }

      if (a.unsortedRanks[i] < b.unsortedRanks[i])
        { return false; }
    }

    return false;
  }


  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    struct Card
    {
      char c;
      int count = 0;
    };

    struct Hand
    {
      UnboundedArray<Card> cards;
      int bid;
      UnboundedArray<int> unsortedRanks;
    };


    std::map<char, int> cardRanks =
      { {'2', 2 }, {'3', 3 }, {'4', 4 }, {'5', 5 },{'6', 6 },{'7', 7 }, {'8', 8 }, {'9', 9 }, {'T', 10 }, {'J', 11 } ,{'Q', 12 } , {'K', 13 }, {'A', 14 } };

    UnboundedArray<Hand> hands;
    for (auto line : lines)
    {
      auto split = Split(line, " ", KeepEmpty::No);

      int bid = std::atoi(split[1].c_str());
      auto hand = split[0];

      Hand &h = hands.AppendNew();
      h.bid = bid;
      for (auto c : hand)
      {
        if (auto cardIndex = h.cards.FindFirst([c](auto crd) { return crd.c == c; }); cardIndex >= 0)
        {
          h.cards[cardIndex].count++;
        }
        else
        {
          h.cards.Append({ .c = c, .count = 1 });
        }

        h.unsortedRanks.Append(cardRanks[c]);
      }

      std::ranges::sort(h.cards, [](auto a, auto b) { return a.count > b.count; });
    }

    std::ranges::sort(hands, SortHands<Hand>);

    s64 sum = 0;
    for (int i = 0; i < hands.Count(); i++)
      { sum += hands[i].bid * (hands.Count() - i); }

    PrintFmt("Part 1: {}\n", sum);
  }
}
