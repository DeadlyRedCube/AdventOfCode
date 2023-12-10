#pragma once

namespace D10
{
  template <std::integral T>
  s32 CountSetBits(T v)
  {
    s32 c = 0;
    for (c = 0; v != 0; c++)
    {
      v &= v - 1;
    }

    return c;
  }


  void Run(const char *path)
  {
    auto charGrid = ReadFileAsCharArray(path);

    enum Exits
    {
      N = 1,
      S = 2,
      E = 4,
      W = 8,

      None = 0,
    };

    Vec2S32 entryPoint;

    Array2D<Exits> graph { charGrid.Width(), charGrid.Height() };
    graph.Fill(Exits::None);

    for (auto y = 0; y < charGrid.Height(); y++)
    {
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        switch (charGrid.Idx(x, y))
        {
        case '|':
          graph.Idx(x, y) = Exits(N | S);
          break;
        case '-':
          graph.Idx(x, y) = Exits(E | W);
          break;
        case 'L':
          graph.Idx(x, y) = Exits(N | E);
          break;
        case 'J':
          graph.Idx(x, y) = Exits(N | W);
          break;
        case '7':
          graph.Idx(x, y) = Exits(S | W);
          break;
        case 'F':
          graph.Idx(x, y) = Exits(S | E);
          break;
        case '.':
          break;
        case 'S':
          entryPoint = { x, y };
          break;
        default:
          ASSERT(false);
        }

        if (x == 0)
          { graph.Idx(x, y) = Exits(graph.Idx(x, y) & ~W); }

        if (x == charGrid.Width() - 1)
          { graph.Idx(x, y) = Exits(graph.Idx(x, y) & ~E); }

        if (y == 0)
          { graph.Idx(x, y) = Exits(graph.Idx(x, y) & ~N); }

        if (y == charGrid.Height() - 1)
          { graph.Idx(x, y) = Exits(graph.Idx(x, y) & ~S); }
      }
    }

    if (graph.Idx(entryPoint.x - 1, entryPoint.y) & E)
      { graph.Idx(entryPoint.x, entryPoint.y) = Exits(graph.Idx(entryPoint.x, entryPoint.y) | W); }
    if (graph.Idx(entryPoint.x + 1, entryPoint.y) & W)
      { graph.Idx(entryPoint.x, entryPoint.y) = Exits(graph.Idx(entryPoint.x, entryPoint.y) | E); }
    if (graph.Idx(entryPoint.x, entryPoint.y - 1) & S)
      { graph.Idx(entryPoint.x, entryPoint.y) = Exits(graph.Idx(entryPoint.x, entryPoint.y) | N); }
    if (graph.Idx(entryPoint.x, entryPoint.y + 1) & N)
      { graph.Idx(entryPoint.x, entryPoint.y) = Exits(graph.Idx(entryPoint.x, entryPoint.y) | S); }

    UnboundedArray<Vec2S32> deadEnds;
    bool keepRunning = true;
    while (keepRunning)
    {
      keepRunning = false;
      for (auto y = 0; y < charGrid.Height(); y++)
      {
        for (auto x = 0; x < charGrid.Width(); x++)
        {
          Vec2S32 p = {x, y};

          auto &exits = graph.Idx(p.x, p.y);
          if (exits == Exits::None)
            { continue; }

          if ((exits & W) != 0)
          {
            Vec2S32 n = { p.x - 1, p.y };
            if ((graph.Idx(n.x, n.y) & E) == 0)
              { exits = Exits(exits & ~W); }
          }

          if ((exits & E) != 0)
          {
            Vec2S32 n = { p.x + 1, p.y };
            if ((graph.Idx(n.x, n.y) & W) == 0)
              { exits = Exits(exits & ~E); }
          }

          if ((exits & N) != 0)
          {
            Vec2S32 n = { p.x, p.y - 1 };
            if ((graph.Idx(n.x, n.y) & S) == 0)
              { exits = Exits(exits & ~N); }
          }

          if ((exits & S) != 0)
          {
            Vec2S32 n = { p.x, p.y + 1};
            if ((graph.Idx(n.x, n.y) & N) == 0)
              { exits = Exits(exits & ~S); }
          }

          if (CountSetBits(s32(exits)) < 2)
          {
            exits = Exits::None;
            keepRunning = true;
          }
        }
      }
    }

    Array2D<s32> distances { charGrid.Width(), charGrid.Height() };
    distances.Fill(std::numeric_limits<s32>::max());

    UnboundedArray<Vec2S32> positions;
    positions.Append(entryPoint);
    distances.Idx(entryPoint.x, entryPoint.y) = 0;

    while (!positions.IsEmpty())
    {
      auto p = positions[FromEnd(-1)];
      positions.SetCount(positions.Count() - 1);

      auto exits = graph.Idx(p.x, p.y);
      auto neighboringDist = distances.Idx(p.x, p.y) + 1;

      if ((exits & W) != 0)
      {
        Vec2S32 n = { p.x - 1, p.y };
        if (neighboringDist < distances.Idx(n.x, n.y))
        {
          distances.Idx(n.x, n.y) = neighboringDist;
          positions.Append(n);
        }
      }

      if ((exits & E) != 0)
      {
        Vec2S32 n = { p.x + 1, p.y };
        if (neighboringDist < distances.Idx(n.x, n.y))
        {
          distances.Idx(n.x, n.y) = neighboringDist;
          positions.Append(n);
        }
      }

      if ((exits & N) != 0)
      {
        Vec2S32 n = { p.x, p.y - 1 };
        if (neighboringDist < distances.Idx(n.x, n.y))
        {
          distances.Idx(n.x, n.y) = neighboringDist;
          positions.Append(n);
        }
      }

      if ((exits & S) != 0)
      {
        Vec2S32 n = { p.x, p.y + 1};
        if (neighboringDist < distances.Idx(n.x, n.y))
        {
          distances.Idx(n.x, n.y) = neighboringDist;
          positions.Append(n);
        }
      }
    }

    auto maxDist = 0;
    for (auto y = 0; y < charGrid.Height(); y++)
    {
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        auto dist = distances.Idx(x, y);
        if (dist != std::numeric_limits<s32>::max())
          { maxDist = std::max(dist, maxDist); }
      }
    }
    PrintFmt("Part 1: {}\n", maxDist);


    // Painter's algorithm!
    s32 containedCount = 0;

    Array2D<s32> contained { charGrid.Width(), charGrid.Height() };
    contained.Fill(0);
    for (auto y = 0; y < charGrid.Height(); y++)
    {
      for (auto x = 0; x < charGrid.Width(); x++)
      {
        if (graph.Idx(x, y) != Exits::None)
          { continue; }
        s32 nCount = 0;
        s32 sCount = 0;
        for (auto i = 0; i < x; i++)
        {
          if ((graph.Idx(i, y) & N) != 0)
            { nCount++; }

          if ((graph.Idx(i, y) & S) != 0)
            { sCount++; }
        }

        s32 crossingCount = std::min(nCount, sCount);
        if ((crossingCount & 1) != 0)
        {
          containedCount++;
          contained.Idx(x, y) = 1;
        }
      }
    }

    PrintFmt("Part 2: {}\n", containedCount);
  }
}