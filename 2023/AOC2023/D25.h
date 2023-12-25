#pragma once

namespace D25
{
  void Run(const char *path)
  {
    struct GraphNode
    {
      std::string name;
      std::set<ssz> connections;
      ssz index = -1;
    };

    UnboundedArray<GraphNode> nodes;
    std::map<std::string, ssz> nodeMap;

    // Helper to add a node with a given name into the node list and the map.
    auto EnsureNodeIndex = [&](std::string nodeName)
    {
      if (auto found = nodeMap.find(nodeName); found != nodeMap.end())
        { return found->second; }
      else
      {
        ssz nodeIndex = nodes.Count();
        auto &n = nodes.AppendNew();
        n.index = nodeIndex;
        n.name = nodeName;
        nodeMap[nodeName] = nodeIndex;
        return nodeIndex;
      }
    };

    // Parse the file
    for (auto line : ReadFileLines(path))
    {
      auto splits = Split(line, " :", KeepEmpty::No);

      auto name = splits[0];
      ssz nodeIndex = EnsureNodeIndex(name);

      for (ssz i = 1; i < ssz(splits.size()); i++)
      {
        auto c = splits[i];

        ssz cIndex = EnsureNodeIndex(c);
        nodes[cIndex].connections.insert(nodeIndex);
        nodes[nodeIndex].connections.insert(cIndex);
      }
    }

    // Make a set of all of the candidate edges in this graph.
    struct Edge
    {
      Edge(ssz aIn, ssz bIn)
        : a(std::min(aIn, bIn))
        , b(std::max(aIn, bIn))
        { }

      ssz a;
      ssz b;

      bool operator==(const Edge &) const = default;
      auto operator<=>(const Edge &) const = default;
    };

    std::set<Edge> edges;
    for (auto &n : nodes)
    {
      for (auto &c : n.connections)
        { edges.insert({ c, n.index }); }
    }

    // Now we're going to whittle down the number of edges we have to check, by removing edges that are part of any
    //  small cycles (increasing the small cycle threshold until we remove enough)
    {
      FixedArray<ssz> distances { nodes.Count() };
      BoundedArray<ssz> reachedIndices { nodes.Count() };
      std::queue<ssz> nodeQueue;

      u32 maxStep = 2;
      while (edges.size() > 100) // We can easily brute-force 100 edges
      {
        for (auto &n : nodes)
        {
          // Set up our structures
          reachedIndices.SetCount(0);
          distances.Fill(std::numeric_limits<ssz>::max());

          // Gonna start at our current node
          distances[n.index] = 0;
          nodeQueue.push(n.index);

          while (!nodeQueue.empty())
          {
            ssz i = nodeQueue.front();
            nodeQueue.pop();

            for (auto c : nodes[i].connections)
            {
              // If we've already found a closer distance, do nothing.
              if (distances[c] < distances[i] + 1)
                { continue; }

              // If this is our first time reaching this node, add it to the reached list.
              if (distances[c] == std::numeric_limits<ssz>::max())
                { reachedIndices.Append(c); }

              // Update the distance and, if we can take more steps, add it to the queue.
              distances[c] = distances[i] + 1;
              if (distances[c] < maxStep)
                { nodeQueue.push(c); }
            }
          }

          // Once we've built up our distance list, we can remove any edges from nodes that have more than one
          //  connection to nodes with a lower or equal distance to itself.
          for (auto i : reachedIndices)
          {
            ssz lowerCount = 0;
            for (auto c : nodes[i].connections)
            {
              if (distances[c] <= distances[i])
                { lowerCount++; }
            }

            if (lowerCount > 1)
            {
              for (auto c : nodes[i].connections)
              {
                if (distances[c] <= distances[i])
                  { edges.erase({i, c}); }
              }
            }
          }
        }

        maxStep++;
      }
    }

    // Flatten the edges to make indexing them easier.
    auto edgeVec = edges | std::ranges::to<std::vector>();

    ssz groupSize = 0;

    // Preallocate a buffer to make this step faster
    FixedArray<bool> filled { ssz(nodes.Count()) };

    // Now, for the edges that are left, brute-force check every triple of edges to see if they're the correct
    //  ones to remove.
    for (usz i = 0; i < edgeVec.size(); i++)
    {
      for (usz j = i + 1; j < edgeVec.size(); j++)
      {
        for (usz k = j + 1; k < edgeVec.size(); k++)
        {
          filled.Fill(false);
          std::queue<ssz> q;
          q.push(nodes[0].index);
          ssz fillCount = 0;

          // Spider out from node 0 to see how many we fill.
          while (!q.empty())
          {
            auto n = q.front();
            q.pop();

            if (filled[nodes[n].index])
              { continue; }

            filled[nodes[n].index] = true;
            fillCount++;

            for (auto c : nodes[n].connections)
            {
              // Don't follow edges if they're one of our three.
              Edge e = { n, c };
              if (edgeVec[i] == e || edgeVec[j] == e || edgeVec[k] == e)
                { continue; }

              q.push(c);
            }
          }

          if (fillCount < nodes.Count())
          {
            // We got two groups!
            groupSize = fillCount;
            break;
          }
        }

        if (groupSize > 0)
          { break; }
      }

      if (groupSize > 0)
        { break; }
    }

    // whatever group we found was one of two, and the other nodes are in the other group.
    PrintFmt("Part 1: {}\n", groupSize * (nodes.Count() - groupSize));
  }
}