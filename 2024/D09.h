#pragma once


namespace D09
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    // This is a straight id-by-id list of what's in memory for part 1.
    std::vector<s32> ids;

    struct FileSpot
    {
      s32 id;
      s32 position;
      s32 length;
    };

    std::vector<FileSpot> fileSpots;

    // There are 10 possible length values (although we're ignoring 0, it still has a spot to make indexing easier),
    //  so we're going to bucket the free spots by their length. priority_queue has the "maximum" element first so use
    //  a "greater" comparator to reverse that and give us the minimum element as the top.
    std::priority_queue<s32, std::vector<s32>, std::greater<s32>> freeSpacePositionsByLength[10];

    auto line = ReadFileLines(path)[0];
    ids.reserve(line.length() * 9);
    for (auto [i, free] = std::tuple{ 0, false }; auto ch : line)
    {
      s32 s = ch - '0';
      if (s == 0)
      {
        // Ignore empty lists (except the free polarity changes)
        free = !free;
        continue;
      }

      s32 pos = s32(ids.size());

      // Build up the basic list for P1
      for (s32 j = 0; j < s; j++)
        { ids.push_back(free ? -1 : i); }

      // Now build the more complex structure for part 2, free spaces go into the free list bucketed by length, and
      //  file "spots" (which is a weird name but it's what I went with) go into an in-order list.
      if (free)
        { freeSpacePositionsByLength[s].push(pos); }
      else
      {
        fileSpots.push_back({ .id = i, .position = pos, .length = s });
        i++;
      }

      free = !free;
    }

    // If the last element was free space, remove all of it from the list.
    while (ids.back() < 0)
      { ids.pop_back(); }

    // Part 1, run through the list linearly moving one character at a time until the destination passes the remaining
    //  list size.
    {
      s32 dst = 0;
      while (ids[dst] >= 0)
        { dst++; }

      while (s32(ids.size()) > dst)
      {
        // Move one character
        ids[dst] = ids.back();
        ids.pop_back();

        // Remove any empty space at the end
        while (ids.back() < 0)
          { ids.pop_back(); }

        // Now bump the destination forward until we hit the end of the list or we hit another free spot.
        dst++;
        while (dst < s32(ids.size()) && ids[dst] >= 0)
          { dst++; }
      }
    }

    // Could have done this counting as we went but this was simpler.
    for (s32 i = 0; i < s32(ids.size()); i++)
      { p1 += i * ids[i]; }

    // Part 2: moving whole blocks
    for (auto &spot : fileSpots | std::views::reverse)
    {
      // Check any (non-empty) buckets at or after our spot's length to see which one is the nearest to the front
      //  (we will greedily insert into a larger space earlier in "memory").
      // views::enumerate is going to give us an [index, value] tuple which maps to [length, position] so we need to
      //  do the min using tup[1]
      auto [length, position] = std::ranges::min(
        freeSpacePositionsByLength
          | std::views::transform([](const auto &v)  { return v.empty() ? std::numeric_limits<s32>::max() : v.top(); })
          | std::views::enumerate
          | std::views::drop(spot.length),
        std::less{},
        [](const auto &tup) { return std::get<1>(tup); });

      // If we couldn't find a valid entry before our current spot, move on to the next one.
      if (position >= spot.position)
        { continue; }

      // Remove our free space from the list.
      freeSpacePositionsByLength[length].pop();

      spot.position = position;
      if (spot.length < length)
      {
        // If the spot didn't fill the whole entry, push the remainder into the corresponding new heap.
        length -= spot.length;
        position += spot.length;
        freeSpacePositionsByLength[length].push(position);
      }
    }

    // Use the arithmetic sum formula to add all of the positions this covers together, then multiply that sum by the
    //  id (as per the instructions).
    for (auto &spot : fileSpots)
      { p2 += s64((spot.position * 2 + spot.length - 1) * spot.length / 2) * spot.id; }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}