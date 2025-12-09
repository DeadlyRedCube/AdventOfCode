export module D09;

import Util;

export namespace D09
{
  using V = Vec2<s32>;

  // It's fine if a line touches the perimeter of a grid rectangle, so this tests for the line to touch anything inside
  //  that perimeter.
  bool GridRectangleIntersectsLineExcludingPerimeter(V cA, V cB, V lA, V lB)
    { return lA.x < cB.x && lB.x > cA.x && lA.y < cB.y && lB.y > cA.y; }



  // Parse a line of text as a Vec2<s32>
  V AsV(const std::string &v)
  {
    char *end;
    V r;
    // Taking advantage of the fact that strtol stops at the first non-digit character to parse without doing a split
    // first.
    r.x = std::strtol(v.c_str(), &end, 10);
    r.y = std::strtol(end + 1, &end, 10);
    return r;
  }



  void Run(const char *path)
  {
    auto redTiles = ReadFileLines(path) | std::views::transform(&AsV) | std::ranges::to<std::vector>();

    std::vector<std::tuple<s64, V, V>> areas;
    areas.reserve((redTiles.size() - 1) * redTiles.size());

    for (usz aI = 0; aI < redTiles.size(); aI++)
    {
      for (usz bI = aI + 1; bI < redTiles.size(); bI++)
      {
        auto ul = Min(redTiles[aI], redTiles[bI]);
        auto lr = Max(redTiles[aI], redTiles[bI]);

        auto area = s64((lr.x - ul.x + 1)) * (lr.y - ul.y + 1);

        // Now push this into our vector as if the vector were a max heap.
        areas.push_back({area, ul, lr});
        std::ranges::push_heap(areas, {}, [](auto &&e) { return std::get<0>(e); });
      }
    }

    // It's a max heap so the largest area is guaranteed to be the first one.
    s64 area1 = std::get<0>(areas[0]);

    s64 area2 = 0;
    while (true)
    {
      auto [area, ul, lr] = areas[0];

      for (usz i = 0; i < redTiles.size(); i++)
      {
        auto pt1 = redTiles[i];
        auto pt2 = redTiles[(i + 1) % redTiles.size()];

        // Sort pt1 and pt2 (this is a fine way to do it because the lines are either purely horizontal or vertical so
        //  one coordinate will always match and the other will get sorted)
        std::tie(pt1, pt2) = std::tuple{Min(pt1, pt2), Max(pt1, pt2)};
        if (GridRectangleIntersectsLineExcludingPerimeter(ul, lr, pt1, pt2))
          { goto nope; } // effectively "continue outer loop", a construct that C++ doesn't have, sadly
      }

      area2 = area;
      break;

    nope:;
      std::ranges::pop_heap(areas, {}, [](auto &&e) { return std::get<0>(e); });
      areas.pop_back();
    }

    PrintFmt("Part 1: {}\n", area1);
    PrintFmt("Part 2: {}\n", area2);
  }
}
