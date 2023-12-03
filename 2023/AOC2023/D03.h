#pragma once

void D03(const char *path)
{
  Array2D<char> ary = MakeCharArray(ReadFileLines(path));

  Array2D<bool> partNeighbors = { ary.Width(), ary.Height() };
  partNeighbors.Fill(false);

  for (ssize_t y = 0; y < ary.Height(); y++)
  {
    for (ssize_t x = 0; x < ary.Width(); x++)
    {
      if (ary.Idx(x, y) != '.' && !std::isdigit(ary.Idx(x, y)))
      {
        for (int i = -1; i <= 1; i++)
        {
          for (int j = -1; j <= 1; j++)
          {
            auto x2 = std::clamp(x + i, ssize_t(0), ary.Width());
            auto y2 = std::clamp(y + j, ssize_t(0), ary.Height());
            partNeighbors.Idx(x2, y2) = true;
          }
        }
      }
    }
  }

  // Now spread part neighbors on digits left
  for (ssize_t y = 0; y < ary.Height(); y++)
  {
    for (ssize_t x = 0; x < ary.Width(); x++)
    {
      if (!partNeighbors.Idx(x, y) || !std::isdigit(ary.Idx(x, y)))
        { continue; }

      for (ssize_t x2 = x; x2 >= 0 && std::isdigit(ary.Idx(x2, y)); x2--)
      {
        partNeighbors.Idx(x2, y) = true;
      }
    }
  }

  // Finally find all the numbers!
  int64_t sum = 0;
  for (ssize_t y = 0; y < ary.Height(); y++)
  {
    for (ssize_t x = 0; x < ary.Width(); x++)
    {
      if (!std::isdigit(ary.Idx(x, y)) || !partNeighbors.Idx(x, y))
        { continue; }

      // This is a part-neighbor digit so parse as string
      char *end;
      int64_t v = 0;
      for (; x < ary.Width() && std::isdigit(ary.Idx(x, y)); x++)
      {
        v *= 10;
        v += ary.Idx(x, y) - '0';
      }

      PrintFmt("Number: {}\n", v);
      sum += v;
    }
  }

  PrintFmt("SUM: {}\n", sum);
}