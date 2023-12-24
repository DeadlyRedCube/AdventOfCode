#pragma once

namespace D23
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

    s32 entranceX = 0;
    for (ssz x = 0; x < grid.Width(); x++)
    {
      if (grid[{x, ssz(0)}] != '#')
      {
        entranceX = s32(x);
        break;
      }
    }

    struct Node
    {
      Vec2S32 pos;
      UnboundedArray<s32> edgeIndices;
    };

    struct Edge
    {
      bool oneWay = false;
      s32 startNodeIndex; // A one way node must be entered from this node (or else cannot be entered)
      s32 endNodeIndex;

      s32 distance = 0;
    };

    UnboundedArray<Node> nodes;
    UnboundedArray<Edge> edges;

    std::map<Vec2S32, s32> nodeLookup;
    std::map<Vec2S32, s32> edgeLookup;

    nodes.Append({.pos = {entranceX, 0}});
    nodeLookup[{entranceX, 0}] = 0;

    struct Step
    {
      s32 startingNodeIndex;
      Vec2S32 step;
    };
    std::queue<Step> stepQueue;
    stepQueue.push({0, { entranceX, 1}});

    const Vec2S32 directions[] = { { 0, -1 }, { 0, 1 }, { 1, 0 }, { -1, 0 } };

    std::map<char, Vec2S32> oneWayDirection = { { '^', { 0, -1 } }, { 'v', { 0, 1 } }, { '>', { 1, 0 } }, { '<', { -1, 0 } } };

    // Build a node graph!
    s32 exitNodeIndex = -1;
    while (!stepQueue.empty())
    {
      auto step = stepQueue.front();
      stepQueue.pop();

      // We may have approached this edge from another direction
      if (edgeLookup.contains(step.step))
        { continue; }

      Vec2S32 prevPos = nodes[step.startingNodeIndex].pos;
      Vec2S32 pos = step.step;

      bool oneWay = false;
      bool flip = false;

      s32 distance = 1;

      // Trace this
      for (;;)
      {
        if (auto f = oneWayDirection.find(grid[pos]); f != oneWayDirection.end())
        {
          // Found a one-way, see if it's in the direction we want
          oneWay = true;
          if (pos + f->second  == prevPos)
            { flip = true; }
        }

        bool hasExit[4] {};
        s32 exitCount = 0;
        Vec2S32 lastExit = pos;
        for (s32 i = 0; i < 4; i++)
        {
          if (grid.PositionInRange(pos + directions[i])
            && pos + directions[i] != prevPos
            && grid[pos + directions[i]] != '#')
          {
            hasExit[i] = true;
            lastExit = pos + directions[i];
            exitCount++;
          }
        }

        if (exitCount > 1 || pos.y == grid.Height() - 1) // If we found an intersection or we reached the bottom
        {
          s32 nodeIndex;
          if (auto n = nodeLookup.find(pos); n != nodeLookup.end())
            { nodeIndex = n->second; }
          else
          {
            nodeIndex = s32(nodes.Count());
            nodes.Append({ pos });
            nodeLookup[pos] = nodeIndex;
            if (pos.y == grid.Height() - 1)
              { exitNodeIndex = nodeIndex; }
          }

          s32 edgeIndex = s32(edges.Count());
          auto &edge = edges.AppendNew();

          edge.oneWay = oneWay;
          edge.startNodeIndex = step.startingNodeIndex;
          edge.endNodeIndex = nodeIndex;
          edge.distance = distance;
          if (flip)
            { std::swap(edge.startNodeIndex, edge.endNodeIndex); }

          nodes[edge.startNodeIndex].edgeIndices.Append(edgeIndex);
          nodes[edge.endNodeIndex].edgeIndices.Append(edgeIndex);

          edgeLookup[step.step] = edgeIndex;
          edgeLookup[prevPos] = edgeIndex;

          if (nodeIndex == nodes.Count() - 1)
          {
            for (s32 i = 0; i < 4; i++)
            {
              if (hasExit[i])
                { stepQueue.push({ nodeIndex, pos + directions[i] }); }
            }
          }

          break;
        }
        else
        {
          ASSERT(exitCount == 1);
          prevPos = pos;
          pos = lastExit;
          distance++;
        }
      }
    }

    // Could just brute force this first part?
    ASSERT(nodes.Count() <= 64); // Gonna use a 64-bit number to store a visited
    ASSERT(exitNodeIndex >= 0);
    //struct

    struct PathFind
    {
      s32 currentNode = 0;
      s64 distanceTraveled = 0;
      u64 visitedNodes = 0;
    };



    // Start at node 0
    s64 bestDistance = 0;
    std::queue<PathFind> pathFindQueue;
    pathFindQueue.push({});

    while (!pathFindQueue.empty())
    {
      auto pf = pathFindQueue.front();
      pathFindQueue.pop();

      if (pf.currentNode == exitNodeIndex)
      {
        bestDistance = std::max(bestDistance, pf.distanceTraveled);
        continue;
      }

      pf.visitedNodes |= (u64(1) << pf.currentNode);

      for (auto ei : nodes[pf.currentNode].edgeIndices)
      {
        // Can't go up a one-way
        if (edges[ei].oneWay && edges[ei].startNodeIndex != pf.currentNode)
          { continue; }

        s32 dest = (edges[ei].startNodeIndex == pf.currentNode) ? edges[ei].endNodeIndex : edges[ei].startNodeIndex;

        // Can't turn to a path we've reached before.
        if ((pf.visitedNodes & (u64(1) << dest)) != 0)
          { continue; }

        pathFindQueue.push(
          {
            .currentNode = dest,
            .distanceTraveled = pf.distanceTraveled + edges[ei].distance,
            .visitedNodes = pf.visitedNodes,
          });
      }
    }

    PrintFmt("Part 1: {}\n", bestDistance);
  }
}