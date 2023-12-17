#pragma once

namespace D17
{
  enum class Dir
  {
    N,
    S,
    E,
    W
  };

  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    struct Graph
    {
      Vec2S32 pos;
      s32 heatLoss;
      UnboundedArray<ssz> outs;
    };
    UnboundedArray<Graph> graph;

    {
      struct SSpace
      {
        bool operator==(const SSpace &) const = default;
        auto operator<=>(const SSpace &) const = default;

        Vec2S32 pos;
        Dir d;
        s32 remainingDir;
      };



      // Map from SSpace to the corresponding graph node
      std::map<SSpace, ssz> spaceCache;

      struct QueueEntry
      {
        SSpace space;
        ssz originatingIndex = -1;
      };

      UnboundedArray<QueueEntry> graphQueue;

      graphQueue.Append({ .space = { .pos = { 0, 0 }, .d = Dir::E, .remainingDir = 3 }, .originatingIndex = -1 });
      graphQueue.Append({ .space = { .pos = { 0, 0 }, .d = Dir::S, .remainingDir = 3 }, .originatingIndex = -1 });

      Dir leftTurnFromDir[] = { Dir::W, Dir::E, Dir::N, Dir::S };
      Dir rightTurnFromDir[] = { Dir::E, Dir::W, Dir::S, Dir::N };

      Vec2S32 dirAdd[] = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };

      while (!graphQueue.IsEmpty())
      {
        QueueEntry e = graphQueue[0];
        graphQueue.RemoveAt(0);

        if (auto it = spaceCache.find(e.space); it != spaceCache.end())
        {
          if (e.originatingIndex >= 0)
          {
            // ASSERT(!graph[e.originatingIndex].outs.Contains(it->second));
            graph[e.originatingIndex].outs.Append(it->second);
          }

          continue;
        }

        // We have a new node in the graph
        ssz graphIndex = graph.Count();
        graph.Append({ .pos = e.space.pos, .heatLoss = grid[e.space.pos] - '0' });
        spaceCache[e.space] = graphIndex;

        if (e.originatingIndex >= 0)
          { graph[e.originatingIndex].outs.Append(graphIndex); }

        // We haven't been in this spot before, but we have (at most) 3 options
        if (e.space.remainingDir > 0)
        {
          // Only can move forward if we have enough forward moves
          auto p = e.space.pos + dirAdd[s32(e.space.d)];
          if (grid.PositionInRange(p))
            { graphQueue.Append({ .space = { .pos = p, .d = e.space.d, .remainingDir = e.space.remainingDir - 1}, .originatingIndex = graphIndex }); }
        }

        {
          auto nd = leftTurnFromDir[s32(e.space.d)];
          auto p = e.space.pos + dirAdd[s32(nd)];
          if (grid.PositionInRange(p))
            { graphQueue.Append({ .space = { .pos = p, .d = nd, .remainingDir = 2}, .originatingIndex = graphIndex }); }
        }

        {
          auto nd = rightTurnFromDir[s32(e.space.d)];
          auto p = e.space.pos + dirAdd[s32(nd)];
          if (grid.PositionInRange(p))
            { graphQueue.Append({ .space = { .pos = p, .d = nd, .remainingDir = 2}, .originatingIndex = graphIndex }); }
        }
      }
    }

    FixedArray<s64> heatLoss { graph.Count() };
    heatLoss.Fill(std::numeric_limits<s64>::max());

    heatLoss[0] = 0;
    heatLoss[1] = 0;

    // Now do a standard distance fill using the heat loss values as we go using our two valid starting configurations
    UnboundedArray<ssz> indices;
    indices.Append(0);
    indices.Append(1);

    while (!indices.IsEmpty())
    {
      auto i = indices[0];
      indices.RemoveAt(0);

      for (auto next : graph[i].outs)
      {
        ssz l = heatLoss[i] + graph[next].heatLoss;
        if (l < heatLoss[next])
        {
          heatLoss[next] = l;
          indices.Append(next);
        }
      }
    }

    // Find all the exit nodes
    s64 minHeatLoss = std::numeric_limits<s64>::max();
    for (ssz i = 0; i < graph.Count(); i++)
    {
      auto &e = graph[i];
      if (e.pos.x == grid.Width() - 1 && e.pos.y == grid.Height() - 1)
        { minHeatLoss = std::min(minHeatLoss, heatLoss[i]); }
    }

    PrintFmt("{}\n", minHeatLoss);
  }
}