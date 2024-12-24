#pragma once


namespace D23
{
  // Every name is a two-character thing
  struct Name
  {
    char ch[2] {};

    Name(const std::string_view &s)
    : ch { s[0], s[1] }
    { }

    explicit operator u16() const
      { return u16(ch[0]) << 8 | ch[1]; }

    bool operator==(const Name &) const = default;
    auto operator<=>(const Name &) const = default;
  };
}


// Let the Name struct work with unordered_map and _set
template<>
struct std::hash<D23::Name>
{
  std::size_t operator()(const D23::Name &n) const noexcept
    { return std::hash<u16>{}(u16(n)); }
};


namespace D23
{
  void Run(const char *filePath)
  {
    s64 p1 = 0;

    // Parse the lines into a map of names to connections from earlier name to later name
    std::unordered_map<Name, std::unordered_set<Name>> connections;
    for (auto line : ReadFileLines(filePath))
    {
      auto a = Name{std::string_view{line}.substr(0, 2)};
      auto b = Name{std::string_view{line}.substr(3, 2)};

      if (a < b)
        { connections[a].insert(b); }
      else
        { connections[b].insert(a); }
    }

    // Part 1: Find all the triples with machines names that start with t
    for (auto &[k, vec] : connections)
    {
      for (auto &k2 : vec)
      {
        for (auto &k3 : connections[k2])
        {
          if (k.ch[0] != 't' && k2.ch[0] != 't' && k3.ch[0] != 't')
            { continue; }

          if (vec.contains(k3))
            { p1++; }
        }
      }
    }

    PrintFmt("P1: {}\n", p1);

    // Part 2: Find the largest connected group of machines

    std::vector<Name> largest;
    std::unordered_set<Name> connected;
    largest.reserve(32);
    connected.reserve(32);
    for (auto &[c0, c0Connections] : connections)
    {
      if (c0Connections.size() < largest.size())
        { continue; }

      // Start by assuming that every node connected to c0 is in the connected set
      connected.clear();
      connected.insert_range(c0Connections);
      for (auto c1 : c0Connections)
      {
        // It's possible we already eliminated this machine so don't count it if it's already not in the set.
        if (!connected.contains(c1))
          { continue; }

        for (auto c2 : c0Connections)
        {
          // Only consider c2 machiens that are still in the connected set and that are lexigraphically after c1 (since
          //  connections[c1] only contains lexigraphically later machines)
          if (c2 <= c1 || !connected.contains(c2))
            { continue; }

          // if c1 and c2 are not connected then c2 is not part of this large connection.
          if (!connections[c1].contains(c2))
          {
            connected.erase(c2);
            if (connected.size() < largest.size())
              { break; }
          }
        }

        if (connected.size() < largest.size())
          { break; }
      }

      if (connected.size() + 1 > largest.size()) // + 1 because connected doesn't include c0
      {
        // This is the new largest set.
        largest.clear();
        largest.push_back(c0);
        largest.append_range(connected);
      }
    }

    std::vector<char> chars;
    chars.reserve(largest.size() * 3);
    std::ranges::sort(largest);
    auto pass = largest
      | std::views::transform([](const Name &n) { return std::string_view{n.ch, 2}; })
      | std::views::join_with(',')
      | std::ranges::to<std::string>();
    PrintFmt("P2: {}\n", pass);
  }
}