#pragma once

namespace D24
{
  struct Mat3x3
  {
    double data[3][3];

    Vec3F64 operator* (const Vec3F64 &v) const
    {
      return
      {
        data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z,
        data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z,
        data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z,
      };
    }
  };


  double Determinant(const Mat3x3 &m)
  {
    return (m.data[0][0] * (m.data[1][1] * m.data[2][2] - m.data[2][1] * m.data[1][2]) -
            m.data[1][0] * (m.data[0][1] * m.data[2][2] - m.data[2][1] * m.data[0][2]) +
            m.data[2][0] * (m.data[0][1] * m.data[1][2] - m.data[1][1] * m.data[0][2]));
  }

  Mat3x3 Inverse(const Mat3x3 &m)
  {
    auto det = Determinant(m);
    double invDet = 1.0 / det;

    Mat3x3 out;
    out.data[0][0] = (m.data[1][1] * m.data[2][2] - m.data[2][1] * m.data[1][2]) * invDet;
    out.data[0][1] = (m.data[2][1] * m.data[0][2] - m.data[0][1] * m.data[2][2]) * invDet;
    out.data[0][2] = (m.data[0][1] * m.data[1][2] - m.data[1][1] * m.data[0][2]) * invDet;

    out.data[1][0] = (m.data[2][0] * m.data[1][2] - m.data[1][0] * m.data[2][2]) * invDet;
    out.data[1][1] = (m.data[0][0] * m.data[2][2] - m.data[2][0] * m.data[0][2]) * invDet;
    out.data[1][2] = (m.data[1][0] * m.data[0][2] - m.data[0][0] * m.data[1][2]) * invDet;

    out.data[2][0] = (m.data[1][0] * m.data[2][1] - m.data[2][0] * m.data[1][1]) * invDet;
    out.data[2][1] = (m.data[2][0] * m.data[0][1] - m.data[0][0] * m.data[2][1]) * invDet;
    out.data[2][2] = (m.data[0][0] * m.data[1][1] - m.data[1][0] * m.data[0][1]) * invDet;
    return out;
  }


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
    double A0x = double(stones[0].pos.x);
    double A0y = double(stones[0].pos.y);
    double A0z = double(stones[0].pos.z);
    double Avx = double(stones[0].vel.x);
    double Avy = double(stones[0].vel.y);
    double Avz = double(stones[0].vel.z);
    //
    // B0x + Bvx*u = Px + Qx*u
    // B0y + Bvy*u = Py + Qy*u
    // B0z + Bvz*u = Pz + Qz*u
    double B0x = double(stones[1].pos.x);
    double B0y = double(stones[1].pos.y);
    double B0z = double(stones[1].pos.z);
    double Bvx = double(stones[1].vel.x);
    double Bvy = double(stones[1].vel.y);
    double Bvz = double(stones[1].vel.z);
    //
    // C0x + Cvx*v = Px + Qx*v
    // C0y + Cvy*v = Py + Qy*v
    // C0z + Cvz*v = Pz + Qz*v
    double C0x = double(stones[3].pos.x);
    double C0y = double(stones[3].pos.y);
    double C0z = double(stones[3].pos.z);
    double Cvx = double(stones[3].vel.x);
    double Cvy = double(stones[3].vel.y);
    double Cvz = double(stones[3].vel.z);
    //
    // We can eliminiate t, u, and v and end up with 6 equations with 6 unknowns (Px, Py, Pz, Qx, Qy, Qz):
    // (Px - A0x) / (Avx - Qx) = (Py - A0y) / (Avy - Qy) = (Pz - A0z) / (Avz - Qz)
    // (Px - B0x) / (Bvx - Qx) = (Py - B0y) / (Bvy - Qy) = (Pz - B0z) / (Bvz - Qz)
    // (Px - C0x) / (Cvx - Qx) = (Py - C0y) / (Cvy - Qy) = (Pz - C0z) / (Cvz - Qz)

    // (Px - A0x) * (Avy - Qy) = (Py - A0y) * (Avx - Qx)
    // (Px - A0x) * (Avz - Qz) = (Pz - A0z) * (Avx - Qx)
    //
    // (Px - B0x) * (Bvy - Qy) = (Py - B0y) * (Bvx - Qx)
    // (Px - B0x) * (Bvz - Qz) = (Pz - B0z) * (Bvx - Qx)
    //
    // (Px - C0x) * (Cvy - Qy) = (Py - C0y) * (Cvx - Qx)
    // (Px - C0x) * (Cvz - Qz) = (Pz - C0z) * (Cvx - Qx)
    //
    // Px = [(Py - A0y) * (Avx - Qx)] / [Avy - Qy] + A0x   =   [(Pz - A0z) * (Avx - Qx)] / [Avz - Qz] + A0x
    //
    // [Avz - Qz][Py - A0y] = [Avy - Qy][Pz - A0z]
    // [Bvz - Qz][Py - B0y] = [Bvy - Qy][Pz - B0z]
    // [Cvz - Qz][Py - C0y] = [Cvy - Qy][Pz - C0z]
    // Try to whittle down to Py here:
    //
    // Py = [Avy - Qy][Pz - A0z]/[Avz - Qz] + A0y
    //    = [Bvy - Qy][Pz - B0z]/[Bvz - Qz] + B0y
    //    = [Cvy - Qy][Pz - C0z]/[Cvz - Qz] + C0y
    //
    // [Avy - Qy][Pz - A0z]/[Avz - Qz] + A0y = [Bvy - Qy][Pz - B0z]/[Bvz - Qz] + B0y
    // [Avy - Qy][Pz - A0z]/[Avz - Qz] + A0y = [Cvy - Qy][Pz - C0z]/[Cvz - Qz] + C0y
    //
    // Pz = ([Bvy - Qy][Pz - B0z]/[Bvz - Qz] + B0y - A0y)(Avz - Qz)/(Avy - Qy) + A0z
    //    = ([Cvy - Qy][Pz - C0z]/[Cvz - Qz] + C0y - C0y)(Avz - Qz)/(Avy - Qy) + A0z
    //
    // i hate this
    //
    // Well technically I have solved for Px, Py, and Pz so let's put this frankenstein nightmare back together and see
    //
    // okay fuck it, we're gonna leverage more hailstones! So you get NOTHING, sample problem!
    //
    // [Avz - Qz][Py - A0y] = [Avy - Qy][Pz - A0z] =>
    // Py = [Avy - Qy][Pz - A0z]/[Avz - Qz] + A0y
    //    = [Bvy - Qy][Pz - B0z]/[Bvz - Qz] + B0y
    //
    // [Avy - Qy][Pz - A0z]/[Avz - Qz] + A0y = [Bvy - Qy][Pz - B0z]/[Bvz - Qz] + B0y
    // [Bvz - Qz][Avy - Qy][Pz - A0z] + [Bvz - Qz][Avz - Qz]A0y = [Avz - Qz][Bvy - Qy][Pz - B0z] + [Bvz - Qz][Avz - Qz]B0y
    // [Bvz - Qz][Avy - Qy][Pz - A0z] = [Avz - Qz][Bvy - Qy][Pz - B0z] + [Bvz - Qz][Avz - Qz](B0y - A0y)
    // [Bvz - Qz][Avy - Qy]Pz - [Bvz - Qz][Avy - Qy]A0z = [Avz - Qz][Bvy - Qy]Pz - [Avz - Qz][Bvy - Qy]B0z + [Bvz - Qz][Avz - Qz][B0y - A0y]
    // ([Bvz - Qz][Avy - Qy] - [Avz - Qz][Bvy - Qy])Pz = [Bvz - Qz][Avy - Qy]A0z - [Avz - Qz][Bvy - Qy]B0z + [Bvz - Qz][Avz - Qz][B0y - A0y]
    // Pz = ([Bvz - Qz][Avy - Qy]A0z - [Avz - Qz][Bvy - Qy]B0z + [Bvz - Qz][Avz - Qz][B0y - A0y]) / ([Bvz - Qz][Avy - Qy] - [Avz - Qz][Bvy - Qy])
    //    = ([Cvz - Qz][Avy - Qy]A0z - [Avz - Qz][Cvy - Qy]C0z + [Cvz - Qz][Avz - Qz][C0y - A0y]) / ([Cvz - Qz][Avy - Qy] - [Avz - Qz][Cvy - Qy])
    //
    // ffffff three to go and I think shit's about to go quadratic
    //
    /// ... gonna try a different way.
    //
    // BACK TO THIS ugh
    //
    // (Px - A0x) / (Avx - Qx) = (Py - A0y) / (Avy - Qy) = (Pz - A0z) / (Avz - Qz)
    // (Px - B0x) / (Bvx - Qx) = (Py - B0y) / (Bvy - Qy) = (Pz - B0z) / (Bvz - Qz)
    // (Px - C0x) / (Cvx - Qx) = (Py - C0y) / (Cvy - Qy) = (Pz - C0z) / (Cvz - Qz)

    // Rearranging:

    // Px * Avy - Px * Qy - A0x * Avy + A0x * Qy = Py * Avx - Py * Qx - A0y * Avx + A0y * Qx
    // (Px * Qy - Py * Qx) = (Px * Avy - Py * Avx) + (A0y * Avx - A0x * Avy) + (A0x * Qy - A0y * Qx)
    // (Px * Qy - Py * Qx) = (Px * Bvy - Py * Bvx) + (B0y * Bvx - B0x * Bvy) + (B0x * Qy - B0y * Qx)
    // (Px * Qy - Py * Qx) = (Px * Cvy - Py * Cvx) + (C0y * Cvx - C0x * Cvy) + (C0x * Qy - C0y * Qx)
    //
    // oh shit I think this might work, Do the same thing for Px/Pz:
    //
    // (Pz * Qx - Px * Qz) = (Pz * Avx - Px * Avz) + (A0x * Avz - A0z * Avx) + (A0z * Qx - A0x * Qz)
    // (Pz * Qx - Px * Qz) = (Pz * Bvx - Px * Bvz) + (B0x * Bvz - B0z * Bvx) + (B0z * Qx - B0x * Qz)
    // (Pz * Qx - Px * Qz) = (Pz * Cvx - Px * Cvz) + (C0x * Cvz - C0z * Cvx) + (C0z * Qx - C0x * Qz)

    // and then Py/Pz:
    //
    // (Py * Qz - Pz * Qy) = (Py * Avz - Pz * Avy) + (A0z * Avy - A0y * Avz) + (A0y * Qz - A0z * Qy)
    // (Py * Qz - Pz * Qy) = (Py * Bvz - Pz * Bvy) + (B0z * Bvy - B0y * Bvz) + (B0y * Qz - B0z * Qy)
    // (Py * Qz - Pz * Qy) = (Py * Cvz - Pz * Cvy) + (C0z * Cvy - C0y * Cvz) + (C0y * Qz - C0z * Qy)
    //
    // YESSSS okay now I think I can turn this into 6 boring-ass linear equations:
    // [Avy - Bvy]Px - [Avx - Bvx]Py - [A0y - B0y]Qx + [A0x - B0x]Qy = (B0y * Bvx - B0x * Bvy) - (A0y * Avx - A0x * Avy)
    // [Avy - Cvy]Px - [Avx - Cvx]Py - [A0y - C0y]Qx + [A0x - C0x]Qy = (C0y * Cvx - C0x * Cvy) - (A0y * Avx - A0x * Avy)
    // [Avx - Bvx]Pz - [Avz - Bvz]Px - [A0x - B0x]Qz + [A0z - B0z]Qx = (B0x * Bvz - B0z * Bvx) - (A0x * Avz - A0z * Avx)
    // [Avx - Cvx]Pz - [Avz - Cvz]Px - [A0x - C0x]Qz + [A0z - C0z]Qx = (C0x * Cvz - C0z * Cvx) - (A0x * Avz - A0z * Avx)
    // [Avz - Bvz]Py - [Avy - Bvy]Pz - [A0z - B0z]Qy + [A0y - B0y]Qz = (B0z * Bvy - B0y * Bvz) - (A0z * Avy - A0y * Avz)
    // [Avz - Cvz]Py - [Avy - Cvy]Pz - [A0z - C0z]Qy + [A0y - C0y]Qz = (C0z * Cvy - C0y * Cvz) - (A0z * Avy - A0y * Avz)
    //
    // Combine some terms to get:

    double abvx = Avx - Bvx;
    double abvy = Avy - Bvy;
    double abvz = Avz - Bvz;

    double acvx = Avx - Cvx;
    double acvy = Avy - Cvy;
    double acvz = Avz - Cvz;

    double ab0x = A0x - B0x;
    double ab0y = A0y - B0y;
    double ab0z = A0z - B0z;

    double ac0x = A0x - C0x;
    double ac0y = A0y - C0y;
    double ac0z = A0z - C0z;

    double h0 = (B0y * Bvx - B0x * Bvy) - (A0y * Avx - A0x * Avy);
    double h1 = (C0y * Cvx - C0x * Cvy) - (A0y * Avx - A0x * Avy);
    double h2 = (B0x * Bvz - B0z * Bvx) - (A0x * Avz - A0z * Avx);
    double h3 = (C0x * Cvz - C0z * Cvx) - (A0x * Avz - A0z * Avx);
    double h4 = (B0z * Bvy - B0y * Bvz) - (A0z * Avy - A0y * Avz);
    double h5 = (C0z * Cvy - C0y * Cvz) - (A0z * Avy - A0y * Avz);
    // abvy*Px - abvx*Py - ab0y*Qx + ab0x*Qy = h0
    // acvy*Px - acvx*Py - ac0y*Qx + ac0x*Qy = h1
    // abvx*Pz - abvz*Px - ab0x*Qz + ab0z*Qx = h2
    // acvx*Pz - acvz*Px - ac0x*Qz + ac0z*Qx = h3
    // abvz*Py - abvy*Pz - ab0z*Qy + ab0y*Qz = h4
    // acvz*Py - acvy*Pz - ac0z*Qy + ac0y*Qz = h5


    // HAHAHAHAHAHA I DID IT (so far)
    //
    // Okay now that's 6 linear equations and 6 variable, right?
    //
    // Px = [h0 + abvx*Py + ab0y*Qx - ab0x*Qy]/abvy
    // Px = [h1 + acvx*Py + ac0y*Qx - ac0x*Qy]/acvy
    //
    // [h0 + abvx*Py + ab0y*Qx - ab0x*Qy]/abvy = [h1 + acvx*Py + ac0y*Qx - ac0x*Qy]/acvy
    // (acvy*abvx - abvy*acvx)*Py = [abvy*ac0y - acvy*ab0y]*Qx + [acvy*ab0x - abvy*ac0x]*Qy + [abvy*h1 - acvy*h0]
    //
    // -----------
    // Py = ([abvy*ac0y - acvy*ab0y]*Qx + [acvy*ab0x - abvy*ac0x]*Qy + [abvy*h1 - acvy*h0])/(acvy*abvx - abvy*acvx)
    // -----------
    //
    // [h4 + abvy*Pz + ab0z*Qy - ab0y*Qz]/abvz = [h5 + acvy*Pz + ac0z*Qy - ac0y*Qz]/acvz
    // (acvz*abvy - abvz*acvy)*Pz = [abvz*ac0z - acvz*ab0z]*Qy + [acvz*ab0y - abvz*ac0y)*Qz + [abvz*h5 - acvz*h4]
    //
    //
    // -----------
    // Pz = ([abvz*ac0z - acvz*ab0z]*Qy + [acvz*ab0y - abvz*ac0y)*Qz + [abvz*h5 - acvz*h4])/(acvz*abvy - abvz*acvy)
    // -----------
    //
    // [h2 + abvz*Px + ab0x*Qz - ab0z*Qx]/abvx = [h3 + acvz*Px + ac0x*Qz - ac0z*Qx]/acvx
    // (acvx*abvz - abvx*acvz)*Px = [abvx*ac0x - acvx*ab0x]*Qz + [acvx*ab0z - abvx*ac0z]*Qx + [abvx*h3 - acvx*h2]
    //
    // -----------
    // Px = ([abvx*ac0x - acvx*ab0x]*Qz + [acvx*ab0z - abvx*ac0z]*Qx + [abvx*h3 - acvx*h2])/(acvx*abvz - abvx*acvz)
    // Py = ([abvy*ac0y - acvy*ab0y]*Qx + [acvy*ab0x - abvy*ac0x]*Qy + [abvy*h1 - acvy*h0])/(acvy*abvx - abvy*acvx)
    // Pz = ([abvz*ac0z - acvz*ab0z]*Qy + [acvz*ab0y - abvz*ac0y)*Qz + [abvz*h5 - acvz*h4])/(acvz*abvy - abvz*acvy)
    // -----------
    //
    // Alright we can sub this shit in for Px, Py, and Pz to get everything in terms of Qx, Qy, and Qz
    // abvy*Px - abvx*Py - ab0y*Qx + ab0x*Qy = h0
    // abvx*Pz - abvz*Px - ab0x*Qz + ab0z*Qx = h2
    // abvz*Py - abvy*Pz - ab0z*Qy + ab0y*Qz = h4
    //
    // Make some more variables!
    // Px = (Pxz*Qz + Pxx*Qx + Pxc)/Pxd
    // Py = (Pyx*Qx + Pyy*Qy + Pyc)/Pyd
    // Pz = (Pzy*Qy + Pzz*Qz + Pzc)/Pzd
    auto Pxx = acvx*ab0z - abvx*ac0z;
    auto Pyy = acvy*ab0x - abvy*ac0x;
    auto Pzz = acvz*ab0y - abvz*ac0y;

    auto Pxz = abvx*ac0x - acvx*ab0x;
    auto Pzy = abvz*ac0z - acvz*ab0z;
    auto Pyx = abvy*ac0y - acvy*ab0y;

    auto Pxc = abvx*h3 - acvx*h2;
    auto Pyc = abvy*h1 - acvy*h0;
    auto Pzc = abvz*h5 - acvz*h4;

    auto Pxd = acvx*abvz - abvx*acvz;
    auto Pyd = acvy*abvx - abvy*acvx;
    auto Pzd = acvz*abvy - abvz*acvy;

    // abvy*[(Pxz*Qz + Pxx*Qx + Pxc)/Pxd] - abvx*[(Pyx*Qx + Pyy*Qy + Pyc)/Pyd] - ab0y*Qx + ab0x*Qy = h0
    // abvx*[(Pzy*Qy + Pzz*Qz + Pzc)/Pzd] - abvz*[(Pxz*Qz + Pxx*Qx + Pxc)/Pxd] - ab0x*Qz + ab0z*Qx = h2
    // abvz*[(Pyx*Qx + Pyy*Qy + Pyc)/Pyd] - abvy*[(Pzy*Qy + Pzz*Qz + Pzc)/Pzd] - ab0z*Qy + ab0y*Qz = h4
    //
    // okay this is just garbage math now but it's finally feeling doable
    //
    // ([abvy/Pxd]*Pxz)*Qz + ([abvy/Pxd]*Pxx - [abvx/Pyd]*Pyx - ab0y)*Qx + (ab0x - [abvx/Pyd]*Pyy)*Qy = h0 - [abvy/Pxd]*Pxc + [abvx/Pyd]*Pyc
    // ([abvx/Pzd]*Pzy)*Qy + ([abvx/Pzd]*Pzz - [abvz/Pxd]*Pxz - ab0x)*Qz + (ab0z - [abvz/Pxd]*Pxx)*Qx = h2 - [abvx/Pzd]*Pzc + [abvz/Pxd]*Pxc
    // ([abvz/Pyd]*Pyx)*Qx + ([abvz/Pyd]*Pyy - [abvy/Pzd]*Pzy - ab0z)*Qy + (ab0y - [abvy/Pzd]*Pzz)*Qz = h4 - [abvz/Pyd]*Pyc + [abvy/Pzd]*Pzc
    //
    // MOAR VARIABLES
    auto Qz0 = (abvy/Pxd)*Pxz;
    auto Qx0 = (abvy/Pxd)*Pxx - (abvx/Pyd)*Pyx - ab0y;
    auto Qy0 = ab0x - (abvx/Pyd)*Pyy;
    auto r0 = h0 - (abvy/Pxd)*Pxc + (abvx/Pyd)*Pyc;

    auto Qy1 = (abvx/Pzd)*Pzy;
    auto Qz1 = (abvx/Pzd)*Pzz - (abvz/Pxd)*Pxz - ab0x;
    auto Qx1 = ab0z - (abvz/Pxd)*Pxx;
    auto r1 = h2 - (abvx/Pzd)*Pzc + (abvz/Pxd)*Pxc;

    auto Qx2 = (abvz/Pyd)*Pyx;
    auto Qy2 = (abvz/Pyd)*Pyy - (abvy/Pzd)*Pzy - ab0z;
    auto Qz2 = ab0y - (abvy/Pzd)*Pzz;
    auto r2 = h4 - (abvz/Pyd)*Pyc + (abvy/Pzd)*Pzc;

    // Qz0*Qz + Qx0*Qx + Qy0*Qy = r0
    // Qy1*Qy + Qz1*Qz + Qx1*Qx = r1
    // Qx2*Qx + Qy2*Qy + Qz2*Qz = r2
    //
    // Qx = [r0 - Qy0*Qy - Qz0*Qz]/Qx0
    //    = [r1 - Qy1*Qy - Qz1*Qz]/Qx1
    //    = [r2 - Qy2*Qy - Qz2*Qz]/Qx2
    //
    // Qx1*r0 - Qx1*Qy0*Qy - Qx1*Qz0*Qz = Qx0*r1 - Qx0*Qy1*Qy - Qx0*Qz1*Qz
    // Qy = ([Qx0*Qz1 - Qx1*Qz0]Qz + [Qx1*r0 - Qx0*r1])/[Qx1*Qy0 - Qx0*Qy1]
    //    = ([Qx0*Qz2 - Qx2*Qz0]Qz + [Qx2*r0 - Qx0*r2])/[Qx2*Qy0 - Qx0*Qy2]
    //
    // ([Qx2*Qy0 - Qx0*Qy2][Qx0*Qz1 - Qx1*Qz0] - [Qx1*Qy0 - Qx0*Qy1][Qx0*Qz2 - Qx2*Qz0])Qz
    //   = [Qx1*Qy0 - Qx0*Qy1][Qx2*r0 - Qx0*r2] - [Qx2*Qy0 - Qx0*Qy2][Qx1*r0 - Qx0*r1]

    // THIS DOES NOT WORK AND I AM LOSING MY GODDAMN MIND
    //
    // where did i mess up
    //
    //
    double Qz = ((Qx1*Qy0 - Qx0*Qy1)*(Qx2*r0 - Qx0*r2) - (Qx2*Qy0 - Qx0*Qy2)*(Qx1*r0 - Qx0*r1))
      / ((Qx2*Qy0 - Qx0*Qy2)*(Qx0*Qz1 - Qx1*Qz0) - (Qx1*Qy0 - Qx0*Qy1)*(Qx0*Qz2 - Qx2*Qz0));
  }
}