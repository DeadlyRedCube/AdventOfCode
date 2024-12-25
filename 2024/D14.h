#pragma once


namespace D14
{
  void Run(const char *path)
  {
    s32 p1 = 0;
    s32 p2 = 0;

    // Each robot is a position and a velocity.
    struct Robot
    {
      Vec2S32 pos;
      Vec2S32 vel;
      s32 lastMoveTime = 0;
    };

    std::vector<Robot> bots;
    for (auto line : ReadFileLines(path))
    {
      auto toks = Split(line, " vp=,", KeepEmpty::No) | std::views::transform(AsS32) | std::ranges::to<std::vector>();
      bots.push_back({ .pos = { toks[0], toks[1] }, .vel = { toks[2], toks[3] }});
    }

    // Choose width based on whether it's the sample problem or not.
    const s32 Width = bots.size() == 12 ? 11 : 101;
    const s32 Height = bots.size() == 12 ? 7 : 103;

    const s32 P1Seconds = 100;

    s32 quadrantCounts[4] = {};
    for (const auto &b : bots)
    {
      // Move p to exactly where it'll be in the correct number of seconds and then wrap the integers so that they're
      //  in range (just push it into the correct place then wrap it into place)
      auto p = b.pos + b.vel * P1Seconds;
      p.x = WrapIndex(p.x, Width);
      p.y = WrapIndex(p.y, Height);

      // If it's right on a midpoint, ignore it for quadrant counting.
      if (p.x == Width / 2 || p.y == Height / 2)
        { continue; }

      // Build an index (doesn't matter which is which) to put each bot into.
      s32 qi = (p.x < Width/2) ? 0 : 2;
      qi |= (p.y < Height/2) ? 0 : 1;
      quadrantCounts[qi]++;
    }

    p1 = quadrantCounts[0] * quadrantCounts[1] * quadrantCounts[2] * quadrantCounts[3];
    PrintFmt("P1: {}\n", p1);

    // Make a grid that is true when a robot is occupying a space.
    Array2D<s32> lastSeenRobotIndex { Width, Height };
    while (true)
    {
      p2++;

      // Step and wrap each bot and then update the grid to show the counts of them all.
      bool noOverlaps = true;
      usz i = 0;
      for (; i < bots.size(); i++)
      {
        auto &b = bots[i];
        s32 delta = p2 - b.lastMoveTime;
        b.pos += b.vel * delta;
        b.pos.x = WrapIndex(b.pos.x, Width);
        b.pos.y = WrapIndex(b.pos.y, Height);
        b.lastMoveTime = p2;

        auto &lastIndex = lastSeenRobotIndex[b.pos];
        if (lastIndex == p2)
        {
          // If we have a robot here already, that's an overlap and we can stop updating this turn.
          noOverlaps = false;
          break;
        }

        // Mark that there's a robot here now.
        lastIndex = p2;
      }

      if (noOverlaps)
        { break; } // There were no overlaps and it turns out the first no-overlap moment is when the tree is.
    }

    PrintFmt("P2: {}\n", p2);
  }
}