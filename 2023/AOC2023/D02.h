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
    int possibleIDSum = 0;
    int powerSum = 0;
    for (auto game : lines)
    {
      auto gameData = game.substr(game.find(':') + 1);

      int maxR = 0;
      int maxG = 0;
      int maxB = 0;

      for (auto colorSet : Split(gameData, ",;", KeepEmpty::No))
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
      }

      if (maxR <= targetR && maxG <= targetG && maxB <= targetB)
        { possibleIDSum += gameIndex; }

      powerSum += maxR * maxG * maxB;
      gameIndex++;
    }

    PrintFmt("Part 1: {}\n", possibleIDSum);
    PrintFmt("Part 2: {}\n", powerSum);
  }
}