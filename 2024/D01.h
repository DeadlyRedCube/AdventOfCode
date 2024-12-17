#pragma once


namespace D01
{
  void Run(const char *path)
  {
    auto lines = ReadFileLines(path);

    UnboundedArray<s64> leftList, rightList;
    std::unordered_map<s64, s32> leftCounts;
    std::unordered_map<s64, s32> rightCounts;

    for (auto [l, r] : lines
      | std::views::transform(
        [](auto &&line)
        {
          // Convert each line into a tuple of 2 s64s.
          return *std::begin(Split(line, " ", KeepEmpty::No)
            | std::views::transform([](const auto &str) { char *end; return s64(std::strtoll(str.c_str(), &end, 10)); })
            | std::views::pairwise);
        }))
    {
      // For part 1 we just need lists of left/right values
      leftList.Append(l);
      rightList.Append(r);

      // For part 2, tally how many of each unique value is in both the left and right lists.
      leftCounts[l]++;
      rightCounts[r]++;
    }

    // Sort them to correlate smallest to smallest, etc.
    std::ranges::sort(leftList);
    std::ranges::sort(rightList);

    s64 dist = Sum(std::views::zip_transform([](s64 l, s64 r) { return std::abs(l - r); }, leftList, rightList));
    PrintFmt("P1: {}\n", dist);

    // Each value in the left column's summation value is its own value multiplied by how many times it's in the right
    //  column, and since we broke the left side up by count as well, we can multiply that value by how many times it
    //  appeared on the left.
    s64 dist2 = Sum(leftCounts | std::views::transform([&](auto t) { auto [l, c] = t; return rightCounts[l] * l * c; }));
    PrintFmt("P2: {}\n", dist2);
  }
}