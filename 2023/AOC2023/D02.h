#pragma once

namespace D02
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    int targetR = 12;
    int targetG = 13;
    int targetB = 14;

    int gameIndex = 1;
    int possibleCount = 0;
    int powerSum = 0;
    for (auto game : lines)
    {
      auto gameData = game.substr(game.find(':') + 1);

      int maxR = 0;
      int maxG = 0;
      int maxB = 0;

      for (auto round : Split(gameData, ";", KeepEmpty::No))
      {
        for (auto colorSet : Split(round, ",", KeepEmpty::No))
        {
          auto toks = Split(colorSet, " ", KeepEmpty::No);

          char *end;
          int count = std::strtol(toks[0].c_str(), &end, 10);

          if (toks[1] == "red")
            { maxR = std::max(maxR, count); }
          else if (toks[1] == "green")
            { maxG = std::max(maxG, count); }
          else if (toks[1] == "blue")
            { maxB = std::max(maxB, count); }
          else
            { ASSERT(false); }
        }
      }

      PrintFmt("Game {}, Maximums: {}, {}, {}\n", gameIndex, maxR, maxG, maxB);

      if (maxR > targetR || maxG > targetG || maxB > targetB)
      {
        PrintFmt("Game {} not possible\n", gameIndex);
      }
      else
      {
        PrintFmt("Game {} YES\n", gameIndex);
        possibleCount += gameIndex;
      }

      powerSum += maxR * maxG * maxB;
      gameIndex++;
    }

    PrintFmt("Possible count {}\n", possibleCount);
    PrintFmt("Power: {}\n", powerSum);
  }
}