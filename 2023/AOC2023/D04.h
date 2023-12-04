#pragma once

namespace D04
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    struct Card
    {
      UnboundedArray<int> sides[2];
      int copyCount = 1;
    };

    UnboundedArray<Card> cards;

    for (auto line : lines)
    {
      auto cardData = Split(line, ":", KeepEmpty::No)[1];

      auto sides = Split(cardData, "|", KeepEmpty::No);

      auto &card = cards.AppendNew();
      for (s32 i = 0; i < 2; i++)
      {
        auto &sideStr = sides[i];
        auto &cardSide = card.sides[i];

        for (auto num : Split(sideStr, " ", KeepEmpty::No))
        {
          char *end;
          auto v = std::strtol(num.c_str(), &end, 10);
          cardSide.Append(v);
        }
      }
    }

    int totalPoints = 0;
    for (s32 cardI = 0; cardI < cards.Count(); cardI++)
    {
      auto &card = cards[cardI];
      int v = 0;
      int n = 0;
      for (auto num : card.sides[0])
      {
        if (card.sides[1].Contains(num))
        {
          v = (v == 0) ? 1 : v * 2;
          n++;
        }
      }

      for (auto i = 1; i <= n; i++)
        { cards[cardI + i].copyCount += card.copyCount; }

      totalPoints += v;
    }

    int cardCount = 0;
    for (auto &card : cards)
      { cardCount += card.copyCount; }

    PrintFmt("Part 1: {}\n", totalPoints);
    PrintFmt("Part 2: {}\n", cardCount);
  }
}