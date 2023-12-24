#pragma once

namespace D24
{
  void Run(const char *path)
  {
    struct Stone
    {
      Vec3S64 pos;
      Vec3S64 vel;
    };

    UnboundedArray<Stone> stones;
    for (auto line : ReadFileLines(path))
    {
      auto splits = Split(line, " ,@", KeepEmpty::No);

      stones.Append(
        {
          .pos = { StrToNum(splits[0]), StrToNum(splits[1]), StrToNum(splits[2]) },
          .vel = { StrToNum(splits[3]), StrToNum(splits[4]), StrToNum(splits[5])},
        });
    }


    const s64 minC = 200'000'000'000'000;
    const s64 maxC = 400'000'000'000'000;
    s32 intersect = 0;
    for (s32 i = 0; i < stones.Count(); i++)
    {
      for (s32 j = i + 1; j < stones.Count(); j++)
      {
        auto &si = stones[i];
        auto &sj = stones[j];

        double cx = double(si.pos.x);
        double cy = double(si.pos.y);
        double dx = double(si.vel.x);
        double dy = double(si.vel.y);

        double ax = double(sj.pos.x);
        double ay = double(sj.pos.y);
        double bx = double(sj.vel.x);
        double by = double(sj.vel.y);

        double det = (dx*by - dy*bx);
        if (det == 0)
          { continue; }

        double u = (bx*(cy-ay) + by*(ax-cx))/det;
        double t = (dx*(cy-ay) + dy*(ax-cx))/det;

        double x = ax + bx * t;
        double y = ay + by * t;

        if (u < 0 || t < 0)
          { continue; }

        if (x >= double(minC) && x <= double(maxC) && y >= double(minC) && y <= double(maxC))
          { intersect++; }
      }
    }

    PrintFmt("Part 1: {}\n", intersect);
  }
}