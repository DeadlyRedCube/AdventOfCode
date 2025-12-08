export module D08;

import Util;

export namespace D08
{
  auto DistanceSqr(Vec3<s64> a, Vec3<s64> b)
  {
    auto d = b - a;
    return Dot(d, d);
  }


  void InsertSorted(auto &vec, auto &e, auto &&pred)
  {

  }


  void Run(const char *path)
  {
    using V = Vec3<s64>;

    // Read the lines in as a list of Vec3s
    auto vecs = ReadFileLines(path)
      | std::views::transform([](auto &&line)
        { auto vals = Split(line, ",", KeepEmpty::No); return Vec3{AsS64(vals[0]), AsS64(vals[1]), AsS64(vals[2])}; })
      | std::ranges::to<std::vector>();

    // Build a list of distances, sorted from smallest to largest
    std::vector<std::tuple<s64, V, V>> distances;
    for (usz i = 0; i < vecs.size(); i++)
    {
      for (usz j = i + 1; j < vecs.size(); j++)
        { distances.push_back(std::make_tuple(DistanceSqr(vecs[i], vecs[j]), vecs[i], vecs[j])); }
    }
    std::ranges::sort(distances);

    // Now, put all of the boxes in their own circuits and give each circuit a count of 1.
    std::map<V, u64> circuitIDs;
    std::map<u64, std::vector<V>> circuits;
    for (u64 i = 0; i < vecs.size(); i++)
    {
      circuitIDs[vecs[i]] = i;
      circuits[i].push_back(vecs[i]);
    }

    auto Connect =
      [&](auto &a, auto &b)
      {
        auto aIdx = circuitIDs[a];
        auto bIdx = circuitIDs[b];
        if (aIdx != bIdx)
        {
          // These aren't already in the same circuit so
          auto &circuitA = circuits[aIdx];
          auto &circuitB = circuits[bIdx];
          if (circuitA.size() >= circuitB.size())
          {
            circuitA.append_range(circuitB);
            for (auto &v : circuitB)
              { circuitIDs[v] = aIdx; }
            circuits.erase(bIdx);
            return circuitA.size();
          }
          else
          {
            circuitB.append_range(circuitA);
            for (auto &v : circuitA)
              { circuitIDs[v] = bIdx; }
            circuits.erase(aIdx);
            return circuitB.size();
          }
        }

        return circuits[aIdx].size();
      };

    const auto p1DistCount = (vecs.size() > 20) ? 1000u : 10u; // Take 10 from the sample, 1000 from the real input
    for (auto [d, a, b] : distances | std::views::take(p1DistCount))
      { std::ignore = Connect(a, b); }

    {
      // Get the sizes as a vector and rearrange it such that the first 3 elements are largest
      auto circuitSizes = std::views::values(circuits)
        | std::views::transform([](auto &&e) { return e.size(); })
        | std::ranges::to<std::vector>();
      std::ranges::nth_element(circuitSizes, circuitSizes.begin() + 2, std::greater{});

      PrintFmt("Part1: {}\n", circuitSizes[0] * circuitSizes[1] * circuitSizes[2]);
    }

    // We keep going!
    for (auto [d, a, b] : distances | std::views::drop(p1DistCount))
    {
      if (Connect(a, b) == vecs.size())
      {
        PrintFmt("Part 2: {}\n", a.x * b.x);
        break;
      }
    }
  }
}