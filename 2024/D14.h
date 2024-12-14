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
    Array2D<bool> hasRobot { Width, Height };
    while (true)
    {
      p2++;

      hasRobot.Fill(false);

      // Step and wrap each bot and then update the grid to show the counts of them all.
      bool noOverlaps = true;
      usz i = 0;
      for (; i < bots.size(); i++)
      {
        // Move and wrap this bot (We can use the simpler wrap below rather than WrapAngle because none of the robot
        //  velocities are larger than the size of the grid)
        auto &b = bots[i];
        b.pos += b.vel;
        b.pos.x = (b.pos.x + Width) % Width;
        b.pos.y = (b.pos.y + Height) % Height;

        auto &has = hasRobot[b.pos];
        if (has)
        {
          // If we have a robot here already, that's an overlap. Break out of this loop (we can stop testing for
          //  overlap and just blast through the remaining robots in a tighter loop)
          noOverlaps = false;
          break;
        }

        // Mark that there's a robot here now.
        has = true;
      }

      if (noOverlaps)
      {
        #if 0
          // Print the visualization!
          PrintFmt("\n\n\n------------ Step: {}\n", p2);
          for (auto c : hasRobot.IterCoords<s32>())
          {
            if (c.x == 0)
              { fputs("\n", stdout); }
            fputs(hasRobot[c] ? "#" : ".", stdout);
          }
        #endif

        // There were no overlaps and it turns out the first no-overlap moment is when the tree is.
        break;
      }
      else
      {
        // We found that there was an overlap but we still have bots to process.
        for (++i; i < bots.size(); i++)
        {
          auto &b = bots[i];
          b.pos += b.vel;
          b.pos.x = (b.pos.x + Width) % Width;
          b.pos.y = (b.pos.y + Height) % Height;
        }
      }
    }

    PrintFmt("P2: {}\n", p2);
  }
}