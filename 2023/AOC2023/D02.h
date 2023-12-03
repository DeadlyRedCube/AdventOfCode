#pragma once

void D02(const char *path)
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
          { assert(false); }
      }
    }

    printf(std::format("Game {}, Maximums: {}, {}, {}\n", gameIndex, maxR, maxG, maxB).c_str());

    if (maxR > targetR || maxG > targetG || maxB > targetB)
    {
      printf(std::format("Game {} not possible\n", gameIndex).c_str());
    }
    else
    {
      printf(std::format("Game {} YES\n", gameIndex).c_str());
      possibleCount += gameIndex;
    }

    powerSum += maxR * maxG * maxB;
    gameIndex++;
  }

  printf(std::format("Possible count {}\n", possibleCount).c_str());
  printf(std::format("Power: {}\n", powerSum).c_str());
}