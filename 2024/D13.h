#pragma once


namespace D13
{
  std::optional<s64> Cost(const Vec2S64 a, Vec2S64 b, Vec2S64 p)
  {
    // There's only one actual way to reach the prize because both buttons only increase and this is a series of two
    //  linear equations.
    // p.x = a.x * A + b.x * B
    // p.y = a.y * A + b.y * B

    // Solving for B we get the following:
    s64 BN = p.y * a.x - p.x * a.y;
    s64 BD = b.y * a.x - b.x * a.y;

    // If the number of B button presses required is non-integral we're out.
    if (BN % BD != 0)
      { return std::nullopt; }
    s64 B = BN / BD;

    // Now plug B back into A (which also needs to be integral)
    s64 AN = p.x - b.x * B;
    s64 AD = a.x;
    if (AN % AD != 0)
      { return std::nullopt; }
    s64 A = AN / AD;

    // 3 tokens per press of A + 1 token per press of B.
    return 3 * A + B;
  }


  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto file = ReadEntireFile(path);

    // Going to take advantage of std::strtol returning a pointer to the character past the end of what parsed to just
    //  parse everything in one go.
    char *next = const_cast<char *>(file.c_str());
    while (true)
    {
      Vec2S64 vs[3];

      // Find the next + in the string. If there isn't one, we're done.
      const char *firstX = strchr(next, '+');
      if (firstX == nullptr)
        { break; }

      // Now convert to numbers as we go, after four '+' characters then two '=' characters.
      vs[0].x = std::strtol(firstX + 1, &next, 10);
      vs[0].y = std::strtol(strchr(next, '+') + 1, &next, 10);
      vs[1].x = std::strtol(strchr(next, '+') + 1, &next, 10);
      vs[1].y = std::strtol(strchr(next, '+') + 1, &next, 10);
      vs[2].x = std::strtol(strchr(next, '=') + 1, &next, 10);
      vs[2].y = std::strtol(strchr(next, '=') + 1, &next, 10);

      // Calculate the cost for this for P1 (won't have a value if there's no integral number of buttons to reach it)
      if (auto c = Cost(vs[0], vs[1], vs[2]); c.has_value())
        { p1 += *c; }

      // Do the same thing but with *much* larger values for the prize position for P2.
      if (auto c = Cost(vs[0], vs[1], vs[2] + Vec2S64{10000000000000, 10000000000000}); c.has_value())
        { p2 += *c; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}