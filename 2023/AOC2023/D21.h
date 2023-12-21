#pragma once

namespace D21
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    Vec2S64 sPosition;
    bool foundS = false;
    for (ssz y = 0; y < grid.Height(); y++)
    {
      for (ssz x = 0; x < grid.Height(); x++)
      {
        if (grid[{x, y}] == 'S')
        {
          sPosition = {x, y};
          foundS = true;
          break;
        }
      }

      if (foundS)
        { break; }
    }

    // Now trace the distances through the graph
    enum class SpaceType
    {
      Unreached,
      Even,
      Odd,
    };

    Array2D<SpaceType> spaces = { grid.Width(), grid.Height() };
    spaces.Fill(SpaceType::Unreached);

    struct Step
    {
      Vec2S64 pos;
      s64 step = 0;
    };
    std::queue<Step> stepQueue;
    stepQueue.push({ .pos = sPosition, .step = 0 });

    struct Found
    {
      Vec2S64 pos;
      SpaceType type;

      bool operator==(const Found &) const = default;
      auto operator<=>(const Found &) const = default;
    };

    std::set<Found> foundSteps;

    Vec2S64 dirs[] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
    while (!stepQueue.empty())
    {
      auto step = stepQueue.front();
      stepQueue.pop();

      auto type = (step.step & 1) ? SpaceType::Odd : SpaceType::Even;

      if (!foundSteps.insert({ .pos = step.pos, .type = type }).second)
        { continue; }

      if (spaces[step.pos] != SpaceType::Even)
        { spaces[step.pos] = type; }

      if (step.step >= 64)
        { continue; }

      for (auto dir : dirs)
      {
        if (spaces.PositionInRange(step.pos + dir) && grid[step.pos + dir] != '#')
          { stepQueue.push({ step.pos + dir, step.step + 1 }); }
      }
    }

    // Count the evens
    s64 count = 0;
    for (auto &&f : foundSteps)
      { if (f.type == SpaceType::Even) { count++; } }

    PrintFmt("Part 1: {}\n", count);
  }
}