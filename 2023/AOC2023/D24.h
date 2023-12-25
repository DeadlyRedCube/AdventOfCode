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

    // need to find a single position that will collide with every hailstone

    // for any given stone, A = A0 + Av*t, our ray (P + Qt) should have a matching t value.
    // With three stones, we have nine equations and nine unknowns (t, u, v, Px, Py, Pz, Qx, Qy, Qz), assuming that
    //  any solution for three will work for all:
    // A0x + Avx*t = Px + Qx*t
    // A0y + Avy*t = Py + Qy*t
    // A0z + Avz*t = Pz + Qz*t
    Vec3F64 A0 = { double(stones[0].pos.x), double(stones[0].pos.y), double(stones[0].pos.z) };
    Vec3F64 Av = { double(stones[0].vel.x), double(stones[0].vel.y), double(stones[0].vel.z) };

    // Plus 3 more equations each for stones B and C (the next two stones in the list)
    Vec3F64 B0 = { double(stones[1].pos.x), double(stones[1].pos.y), double(stones[1].pos.z) };
    Vec3F64 Bv = { double(stones[1].vel.x), double(stones[1].vel.y), double(stones[1].vel.z) };
    Vec3F64 C0 = { double(stones[2].pos.x), double(stones[2].pos.y), double(stones[2].pos.z) };
    Vec3F64 Cv = { double(stones[2].vel.x), double(stones[2].vel.y), double(stones[2].vel.z) };

    // We can eliminiate t, u, and v and end up with 6 equations with 6 unknowns (Px, Py, Pz, Qx, Qy, Qz):
    // (Px - A0x) / (Avx - Qx) = (Py - A0y) / (Avy - Qy) = (Pz - A0z) / (Avz - Qz)
    // (Px - B0x) / (Bvx - Qx) = (Py - B0y) / (Bvy - Qy) = (Pz - B0z) / (Bvz - Qz)
    // (Px - C0x) / (Cvx - Qx) = (Py - C0y) / (Cvy - Qy) = (Pz - C0z) / (Cvz - Qz)

    // Rearranging the Px/Py pairing:

    // Px * Avy - Px * Qy - A0x * Avy + A0x * Qy = Py * Avx - Py * Qx - A0y * Avx + A0y * Qx
    // (Px * Qy - Py * Qx) = (Px * Avy - Py * Avx) + (A0y * Avx - A0x * Avy) + (A0x * Qy - A0y * Qx)   {Equation 1}
    // (Px * Qy - Py * Qx) = (Px * Bvy - Py * Bvx) + (B0y * Bvx - B0x * Bvy) + (B0x * Qy - B0y * Qx)   {Equation 2}
    // (Px * Qy - Py * Qx) = (Px * Cvy - Py * Cvx) + (C0y * Cvx - C0x * Cvy) + (C0x * Qy - C0y * Qx)   {Equation 3}
    //
    // Note that this gets a common (Px * Qy - Py * Qx) on the left side of everything, and the right side of each is
    //  now just a linear equation.
    // Do the same for the Pz/Px and Py/Pz pairints:
    //
    // (Pz * Qx - Px * Qz) = (Pz * Avx - Px * Avz) + (A0x * Avz - A0z * Avx) + (A0z * Qx - A0x * Qz)   {Equation 4}
    // (Pz * Qx - Px * Qz) = (Pz * Bvx - Px * Bvz) + (B0x * Bvz - B0z * Bvx) + (B0z * Qx - B0x * Qz)   {Equation 5}
    // (Pz * Qx - Px * Qz) = (Pz * Cvx - Px * Cvz) + (C0x * Cvz - C0z * Cvx) + (C0z * Qx - C0x * Qz)   {Equation 6}
    //
    // (Py * Qz - Pz * Qy) = (Py * Avz - Pz * Avy) + (A0z * Avy - A0y * Avz) + (A0y * Qz - A0z * Qy)   {Equation 7}
    // (Py * Qz - Pz * Qy) = (Py * Bvz - Pz * Bvy) + (B0z * Bvy - B0y * Bvz) + (B0y * Qz - B0z * Qy)   {Equation 8}
    // (Py * Qz - Pz * Qy) = (Py * Cvz - Pz * Cvy) + (C0z * Cvy - C0y * Cvz) + (C0y * Qz - C0z * Qy)   {Equation 9}
    //
    // Setting equations 1 and 2 equal to each other (to cancel out the common (Px * Qy - Py * Qx) term) gives us:
    // [Avy - Bvy]Px - [Avx - Bvx]Py - [A0y - B0y]Qx + [A0x - B0x]Qy = (B0y * Bvx - B0x * Bvy) - (A0y * Avx - A0x * Avy)
    //
    // If we similarly pair equations [1, 3], [4, 5], [4, 6], [7, 8], and [7, 9] together and simplify we end up with
    //  a series of 6 straight-up linear equations, which can be more easily solved than the above nonsense.
    //
    // [Avy - Bvy]Px - [Avx - Bvx]Py - [A0y - B0y]Qx + [A0x - B0x]Qy = (B0y * Bvx - B0x * Bvy) - (A0y * Avx - A0x * Avy)
    // [Avy - Cvy]Px - [Avx - Cvx]Py - [A0y - C0y]Qx + [A0x - C0x]Qy = (C0y * Cvx - C0x * Cvy) - (A0y * Avx - A0x * Avy)
    // [Avx - Bvx]Pz - [Avz - Bvz]Px - [A0x - B0x]Qz + [A0z - B0z]Qx = (B0x * Bvz - B0z * Bvx) - (A0x * Avz - A0z * Avx)
    // [Avx - Cvx]Pz - [Avz - Cvz]Px - [A0x - C0x]Qz + [A0z - C0z]Qx = (C0x * Cvz - C0z * Cvx) - (A0x * Avz - A0z * Avx)
    // [Avz - Bvz]Py - [Avy - Bvy]Pz - [A0z - B0z]Qy + [A0y - B0y]Qz = (B0z * Bvy - B0y * Bvz) - (A0z * Avy - A0y * Avz)
    // [Avz - Cvz]Py - [Avy - Cvy]Pz - [A0z - C0z]Qy + [A0y - C0y]Qz = (C0z * Cvy - C0y * Cvz) - (A0z * Avy - A0y * Avz)
    //
    // For this code, we'll write it in matrix/vector notation:
    Mat<double, 6, 6> mat
    {
      {
        { Av.y - Bv.y,  Bv.x - Av.x,          0.0,  B0.y - A0.y,  A0.x - B0.x,          0.0 },
        { Av.y - Cv.y,  Cv.x - Av.x,          0.0,  C0.y - A0.y,  A0.x - C0.x,          0.0 },
        { Bv.z - Av.z,          0.0,  Av.x - Bv.x,  A0.z - B0.z,          0.0,  B0.x - A0.x },
        { Cv.z - Av.z,          0.0,  Av.x - Cv.x,  A0.z - C0.z,          0.0,  C0.x - A0.x },
        {         0.0,  Av.z - Bv.z,  Bv.y - Av.y,          0.0,  B0.z - A0.z,  A0.y - B0.y },
        {         0.0,  Av.z - Cv.z,  Cv.y - Av.y,          0.0,  C0.z - A0.z,  A0.y - C0.y },
      }
    };

    Vec<double, 6> h =
    {
      (B0.y * Bv.x - B0.x * Bv.y) - (A0.y * Av.x - A0.x * Av.y),
      (C0.y * Cv.x - C0.x * Cv.y) - (A0.y * Av.x - A0.x * Av.y),
      (B0.x * Bv.z - B0.z * Bv.x) - (A0.x * Av.z - A0.z * Av.x),
      (C0.x * Cv.z - C0.z * Cv.x) - (A0.x * Av.z - A0.z * Av.x),
      (B0.z * Bv.y - B0.y * Bv.z) - (A0.z * Av.y - A0.y * Av.z),
      (C0.z * Cv.y - C0.y * Cv.z) - (A0.z * Av.y - A0.y * Av.z),
    };

    // Where
    //         | Px |
    //         | Py |
    //   mat * | Pz | = h
    //         | Qx |
    //         | Qy |
    //         | Qz |
    //

    // Inverting the matrix and multiplying h with it gives us our desired P and Q values.
    auto inv = mat.Inverse();
    auto res = inv * h;

    s64 sum = s64(std::round(res[0]) + std::round(res[1]) + std::round(res[2]));
    PrintFmt("Part 2: {}\n", sum);
  }
}