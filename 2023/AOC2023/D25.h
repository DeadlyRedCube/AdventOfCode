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

    std::map<Edge, s32> edgeFrequencies;
    for (auto &n : nodes)
    {
      for (auto &c : n.connections)
        { edgeFrequencies[{c, n.index}] = 0; }
    }

    // Pick a bunch of random pairs of nodes and track how often we see any given edge on the shortest path between
    //  them.
    std::random_device rd;
    std::mt19937_64 mt { rd() };
    {
      FixedArray<ssz> distances { nodes.Count() };
      std::queue<ssz> indexQueue;
      for (s32 iter = 0; iter < 200; iter++)
      {
        distances.Fill(std::numeric_limits<ssz>::max());

        // Pick a random pair of graph nodes
        ssz nodeA = ssz(mt() % usz(nodes.Count()));
        ssz nodeB = nodeA;
        while (nodeB == nodeA)
          { nodeB = ssz(mt() % usz(nodes.Count())); }

        // Do a BFS to find the shortest path from A to B
        distances[nodeA] = 0;
        indexQueue.push(nodeA);
        while (!indexQueue.empty())
        {
          ssz i = indexQueue.front();
          indexQueue.pop();

          for (auto c : nodes[i].connections)
          {
            if (c == nodeB)
            {
              // We reached the exit, we're done with this scan.
              indexQueue = {};
              break;
            }

            if (distances[c] != std::numeric_limits<ssz>::max())
              { continue; }

            distances[c] = distances[i] + 1;
            indexQueue.push(c);
          }
        }

        // Now trace back from nodeB to nodeA along the distances, incrementing all the edge frequencies.
        ssz cur = nodeB;
        while (cur != nodeA)
        {
          for (auto c : nodes[cur].connections)
          {
            if (distances[c] < distances[cur])
            {
              edgeFrequencies[{c, cur}]++;
              cur = c;
              break;
            }
          }
        }
      }
    }

    // Flatten the edges to make indexing them easier, then sort them in decreasing frequency.
    BoundedArray<Edge> edges { ssz(edgeFrequencies.size()) };
    for (auto [e, d] : edgeFrequencies)
      { edges.Append(e); }
    std::ranges::sort(edges, [&](auto &&a, auto &&b) { return edgeFrequencies[a] > edgeFrequencies[b]; });

    FixedArray<bool> filled { ssz(nodes.Count()) };
    ssz groupSize = 0;

    // Now iterate through the edges from highest frequency to lowest frequency checking every triple along the way
    //  to see if that is the triple that splits the graph. We'll iterate in such a way that we push the worst
    //  (lowest-frequency) node at the lowest rate, so we will exhaust all higher-frequency choices before pushing our
    //  worst-case node farther up.
    for (ssz i = 2; i < edges.Count(); i++)
    {
      // This is the highest-frequency node, always starts at 0, iterates next-slowest
      for (ssz j = 0; j < i - 1; j++)
      {
        // Then this is the middle node, exhausting all middle choices before moving the highest-frequency one up.
        for (ssz k = j + 1; k < i; k++)
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
              if (edges[i] == e || edges[j] == e || edges[k] == e)
                { continue; }

              q.push(c);
            }
          }

          if (fillCount < nodes.Count())
          {
            // We got two groups!
            groupSize = fillCount;
            goto endLoop; // if C++ has breaking out of outer loops I'd use that instead.
          }
        }
      }
    }

    endLoop:;

    // whatever group we found was one of two, and the other nodes are in the other group.
    PrintFmt("Part 1: {}\n", groupSize * (nodes.Count() - groupSize));
  }
}