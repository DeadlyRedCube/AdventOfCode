#pragma once

namespace D23
{
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


  s32 FindLongestPath(ArrayView<Node> nodes, ArrayView<Edge> edges, s32 exitNodeIndex)
  {
    ASSERT(nodes.Count() <= 64); // Gonna use a 64-bit number to store a visited
    ASSERT(exitNodeIndex >= 0);

    struct PathFind
    {
      s32 currentNode = 0;
      s32 distanceTraveled = 0;
      u64 visitedNodes = 0;       // Keep a bitmask of visited nodes
    };


    // Start at node 0 (always the entrance)
    s32 bestDistance = 0;
    std::queue<PathFind> pathFindQueue;
    pathFindQueue.push({});

    while (!pathFindQueue.empty())
    {
      auto pf = pathFindQueue.front();
      pathFindQueue.pop();

      if (pf.currentNode == exitNodeIndex)
      {
        // We reached the exit so update our best travel distance.
        bestDistance = std::max(bestDistance, pf.distanceTraveled);
        continue;
      }

      pf.visitedNodes |= (u64(1) << pf.currentNode);

      for (auto ei : nodes[pf.currentNode].edgeIndices)
      {
        // Can't go up a one-way
        if (edges[ei].oneWay && edges[ei].startNodeIndex != pf.currentNode)
          { continue; }

        // Get the destination node index given our direction of travel.
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

    return bestDistance;
  }


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

    std::map<char, Vec2S32> oneWayDirection =
    {
      { '^', { 0, -1 } },
      { 'v', { 0, 1 } },
      { '>', { 1, 0 } },
      { '<', { -1, 0 } }
    };

    // Build a node graph! Every intersection (plus start and end) are nodes, the paths between are edges, which can
    //  be one-way.
    s32 exitNodeIndex = -1;
    while (!stepQueue.empty())
    {
      auto step = stepQueue.front();
      stepQueue.pop();

      // We may have approached this edge from another direction, so don't keep tracing if we've already done it.
      if (edgeLookup.contains(step.step))
        { continue; }

      Vec2S32 prevPos = nodes[step.startingNodeIndex].pos;
      Vec2S32 pos = step.step;

      bool oneWay = false;
      bool flip = false;

      s32 distance = 1;

      for (;;)
      {
        if (auto f = oneWayDirection.find(grid[pos]); f != oneWayDirection.end())
        {
          // Found a one-way, see if it's in the direction we want
          oneWay = true;
          if (pos + f->second == prevPos)
            { flip = true; } // This path is going the opposite direction as we're tracing it, so flip it when done
        }

        bool hasExit[4] {};
        s32 exitCount = 0;
        Vec2S32 lastExit = pos;
        for (s32 i = 0; i < 4; i++)
        {
          // Check for any ways out from this node that aren't the way we came in.
          if (grid.PositionInRange(pos + directions[i])
            && pos + directions[i] != prevPos
            && grid[pos + directions[i]] != '#')
          {
            hasExit[i] = true;
            lastExit = pos + directions[i];
            exitCount++;
          }
        }

        if (exitCount > 1 || pos.y == grid.Height() - 1)
        {
          // This had multiple exits so it's going to become a node in the graph (if it isn't already)
          s32 nodeIndex;
          if (auto n = nodeLookup.find(pos); n != nodeLookup.end())
            { nodeIndex = n->second; }
          else
          {
            // Was not already in the graph, so add a new one (and note if it's the exit)
            nodeIndex = s32(nodes.Count());
            nodes.Append({ pos });
            nodeLookup[pos] = nodeIndex;
            if (pos.y == grid.Height() - 1)
              { exitNodeIndex = nodeIndex; }
          }

          // Add a new edge
          s32 edgeIndex = s32(edges.Count());
          auto &edge = edges.AppendNew();

          edge.oneWay = oneWay;
          edge.startNodeIndex = step.startingNodeIndex;
          edge.endNodeIndex = nodeIndex;
          edge.distance = distance;

          // If we traced it the wrong way, swap start/end so that they're correctly ordered for one-way travel.
          if (flip)
            { std::swap(edge.startNodeIndex, edge.endNodeIndex); }

          // Update the ending nodes to know they connect through this edge.
          nodes[edge.startNodeIndex].edgeIndices.Append(edgeIndex);
          nodes[edge.endNodeIndex].edgeIndices.Append(edgeIndex);

          // Update our edge lookup so we don't have duplicates.
          edgeLookup[step.step] = edgeIndex;
          edgeLookup[prevPos] = edgeIndex;

          // If we added this node (it's at the end of the nodes list) trace all other exits.
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
          // Otherwise, this was just another step along the edge path. Move us along the path and continue to count
          //  our distance traveled.
          ASSERT(exitCount == 1);
          prevPos = pos;
          pos = lastExit;
          distance++;
        }
      }
    }

    PrintFmt("Part 1: {}\n", FindLongestPath(nodes, edges, exitNodeIndex));

    // For part 2, remove the one-ways
    for (auto &e : edges)
      { e.oneWay = false; }

    PrintFmt("Part 2: {}\n", FindLongestPath(nodes, edges, exitNodeIndex));
  }
}