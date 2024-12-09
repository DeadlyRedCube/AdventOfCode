#pragma once


namespace D09
{
  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto line = ReadFileLines(path)[0];

    struct FileSpot
    {
      std::optional<s32> index;
      s32 size;
    };

    std::vector<s32> ids;

    ssize_t firstFree = -1;
    s32 freeCount = 0;
    {
      s32 i = 0;
      bool f = false;
      for (auto ch : line)
      {
        s32 s = ch - '0';

        if (f && firstFree < 0)
          { firstFree = ssize_t(ids.size()); }

        for (s32 j = 0; j < s; j++)
          { ids.push_back(f ? -1 : i); }

        if (!f)
          { ++i; }

        freeCount += f ? 1 : 0;
        f = !f;
      }
    }

    s32 origFirstFree = firstFree;
    {
      auto p1IDs = ids;

      while (true)
      {
        while (p1IDs.back() < 0)
          { p1IDs.pop_back(); }

        if (firstFree >= ssize_t(p1IDs.size()))
          { break; }

        s32 id = p1IDs.back();
        p1IDs.pop_back();
        p1IDs[firstFree] = id;
        while (firstFree < ssize_t(p1IDs.size()) && p1IDs[firstFree] >= 0)
          { firstFree++; }
      }

      for(u32 i = 0; i < p1IDs.size(); i++)
      {
        p1 += s64(i) * p1IDs[i];
      }
    }

    PrintFmt("P1: {}\n", p1);

    for(ssize_t i = size_t(ids.size()) - 1; i >= 0; i--)
    {
      if (ids[i] < 0)
        { continue; }

      s32 scount = 1;
      while (i > 0 && ids[i - 1] == ids[i])
        { scount++; i--; }

      for (ssize_t j = 0; j < i; j++)
      {
        if (ids[j] >= 0)
          { continue; }

        s32 dcount = 0;
        while (ids[j] < 0)
          { dcount++; j++; }

        if (dcount >= scount)
        {
          j -= dcount;
          for (ssize_t k = 0; k < scount; k++)
          {
            ids[j + k] = ids[i + k];
            ids[i + k] = -1;
          }
          break;
        }
      }
    }

      for(u32 i = 0; i < ids.size(); i++)
      {
        if (ids[i] < 0)
          { continue; }
        p2 += s64(i) * ids[i];
      }

    PrintFmt("P2: {}\n", p2);
  }
}