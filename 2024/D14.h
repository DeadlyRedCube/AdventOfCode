#pragma once


namespace D14
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    struct Robot
    {
      Vec2S64 pos;
      Vec2S64 vel;
    };

    std::vector<Robot> bots;
    for (auto line : ReadFileLines(path))
    {
      auto toks = Split(line, " vp=,", KeepEmpty::No) | std::views::transform(AsS64) | std::ranges::to<std::vector>();

      bots.push_back({ .pos = { toks[0], toks[1] }, .vel = { toks[2], toks[3] }});
    }

    const s32 Width = bots.size() == 12 ? 11 : 101;
    const s32 Height = bots.size() == 12 ? 7 : 103;

    const s32 Seconds = 100;

    s64 quadrantCounts[4] = {};

    for (const auto &b : bots)
    {
      auto p = b.pos + b.vel * Seconds;
      p.x  = ((p.x % Width) + Width) % Width;
      p.y  = ((p.y % Height) + Height) % Height;

      if (p.x == Width / 2 || p.y == Height / 2)
        { continue; }

      s32 qi = (p.x < Width/2) ? 0 : 2;
      qi |= (p.y < Height/2) ? 0 : 1;
      quadrantCounts[qi]++;
    }

    Array2D<s32> grid { Width, Height };

    while (true)
    {
      p2++;
      if (p2 % 10000 == 0)
        { PrintFmt("{}\n", p2); }
      grid.Fill(0);
      for (auto &b : bots)
      {
        b.pos += b.vel;
        b.pos.x  = ((b.pos.x % Width) + Width) % Width;
        b.pos.y  = ((b.pos.y % Height) + Height) % Height;

        grid[b.pos]++;
      }

      bool test = true;
      for (auto c : grid.IterCoords<s32>())
      {
        if (grid[c] > 1)
          { test = false; }
      }

      if (test)
      {
        PrintFmt("\n\n\n------------ Step: {}\n", p2);
        for (auto c : grid.IterCoords<s32>())
        {
          if (c.x == 0)
            { fputs("\n", stdout); }
          fputs(grid[c] ? "#" : ".", stdout);
        }

        break;
      }
    }

    p1 = quadrantCounts[0] * quadrantCounts[1] * quadrantCounts[2] * quadrantCounts[3];

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}