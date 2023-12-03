#pragma once

int64_t D03GetNumberAt(const Array2D<char> &ary, ssize_t x, ssize_t y)
{
  while (x > 0 && std::isdigit(ary.Idx(x - 1, y)))
    { x--; }

  int64_t v = 0;
  for (; x < ary.Width() && std::isdigit(ary.Idx(x, y)); x++)
  {
    v *= 10;
    v += ary.Idx(x, y) - '0';
  }

  return v;
}

void D03(const char *path)
{
  Array2D<char> ary = ReadFileAsCharArray(path);

  Array2D<bool> partNeighbors = { ary.Width(), ary.Height() };
  partNeighbors.Fill(false);

  int64_t gearSum = 0;
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

        if (ary.Idx(x, y) == '*')
        {
          int neighboringNumberCount = 0;
          int64_t gearVal = 1;
          if (x >= 0 && std::isdigit(ary.Idx(x - 1, y)))
          {
            gearVal *= D03GetNumberAt(ary, x - 1, y);
            neighboringNumberCount++;
          }

          if (x < ary.Width() && std::isdigit(ary.Idx(x + 1, y)))
          {
            gearVal *= D03GetNumberAt(ary, x + 1, y);
            neighboringNumberCount++;
          }

          if (y >= 0)
          {
            auto y2 = y - 1;
            if (std::isdigit(ary.Idx(x, y2)))
            {
              gearVal *= D03GetNumberAt(ary, x, y2);
              neighboringNumberCount++;
            }
            else
            {
              if (x >= 0 && std::isdigit(ary.Idx(x - 1, y2)))
              {
                gearVal *= D03GetNumberAt(ary, x - 1, y2);
                neighboringNumberCount++;
              }

              if (x < ary.Width() && std::isdigit(ary.Idx(x + 1, y2)))
              {
                gearVal *= D03GetNumberAt(ary, x + 1, y2);
                neighboringNumberCount++;
              }
            }
          }

          if (y < ary.Height())
          {
            auto y2 = y + 1;
            if (std::isdigit(ary.Idx(x, y2)))
            {
              gearVal *= D03GetNumberAt(ary, x, y2);
              neighboringNumberCount++;
            }
            else
            {
              if (x >= 0 && std::isdigit(ary.Idx(x - 1, y2)))
              {
                gearVal *= D03GetNumberAt(ary, x - 1, y2);
                neighboringNumberCount++;
              }

              if (x < ary.Width() && std::isdigit(ary.Idx(x + 1, y2)))
              {
                gearVal *= D03GetNumberAt(ary, x + 1, y2);
                neighboringNumberCount++;
              }
            }
          }

          if (neighboringNumberCount == 2)
            { gearSum += gearVal; }
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
        { partNeighbors.Idx(x2, y) = true; }
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
      int64_t v = 0;
      for (; x < ary.Width() && std::isdigit(ary.Idx(x, y)); x++)
      {
        v *= 10;
        v += ary.Idx(x, y) - '0';
      }

      sum += v;
    }
  }

  PrintFmt("SUM: {}\n", sum);
  PrintFmt("GearSum: {}\n", gearSum);
}