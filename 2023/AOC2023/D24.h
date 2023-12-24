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

    // We want 3 indices that have different x, y, and z velocity values for ... reasons that I'm too tired to figure
    //  out but doing this makes it not become a NaN at the end.
    // (Honestly I'm not sure that the problem isn't actually something else - should probably just be doing a
    //  denominator check on the divisions and moving on to different values if we get a (near) 0), but meh. It worked
    //  and I'm moving on.
    s32 s0Index = 0;
    s32 s1Index = 1;
    while (stones[s0Index].vel == stones[s1Index].vel )
      { s1Index++; }

    s32 s2Index = s1Index + 1;
    while (stones[s0Index].vel == stones[s2Index].vel || stones[s1Index].vel == stones[s2Index].vel)
      { s2Index++; }

    // for any given stone, A = A0 + Av*t, our ray (P + Qt) should have a matching t value.
    // With three stones, we have nine equations and nine unknowns (t, u, v, Px, Py, Pz, Qx, Qy, Qz), assuming that
    //  any solution for three will work for all:
    // A0x + Avx*t = Px + Qx*t
    // A0y + Avy*t = Py + Qy*t
    // A0z + Avz*t = Pz + Qz*t
    double A0x = double(stones[s0Index].pos.x);
    double A0y = double(stones[s0Index].pos.y);
    double A0z = double(stones[s0Index].pos.z);
    double Avx = double(stones[s0Index].vel.x);
    double Avy = double(stones[s0Index].vel.y);
    double Avz = double(stones[s0Index].vel.z);
    //
    // B0x + Bvx*u = Px + Qx*u
    // B0y + Bvy*u = Py + Qy*u
    // B0z + Bvz*u = Pz + Qz*u
    double B0x = double(stones[s1Index].pos.x);
    double B0y = double(stones[s1Index].pos.y);
    double B0z = double(stones[s1Index].pos.z);
    double Bvx = double(stones[s1Index].vel.x);
    double Bvy = double(stones[s1Index].vel.y);
    double Bvz = double(stones[s1Index].vel.z);
    //
    // C0x + Cvx*v = Px + Qx*v
    // C0y + Cvy*v = Py + Qy*v
    // C0z + Cvz*v = Pz + Qz*v
    double C0x = double(stones[s2Index].pos.x);
    double C0y = double(stones[s2Index].pos.y);
    double C0z = double(stones[s2Index].pos.z);
    double Cvx = double(stones[s2Index].vel.x);
    double Cvy = double(stones[s2Index].vel.y);
    double Cvz = double(stones[s2Index].vel.z);
    //
    // We can eliminiate t, u, and v and end up with 6 equations with 6 unknowns (Px, Py, Pz, Qx, Qy, Qz):
    // (Px - A0x) / (Avx - Qx) = (Py - A0y) / (Avy - Qy) = (Pz - A0z) / (Avz - Qz)
    // (Px - B0x) / (Bvx - Qx) = (Py - B0y) / (Bvy - Qy) = (Pz - B0z) / (Bvz - Qz)
    // (Px - C0x) / (Cvx - Qx) = (Py - C0y) / (Cvy - Qy) = (Pz - C0z) / (Cvz - Qz)

    // Rearranging the Px/Py pairing:

    // Px * Avy - Px * Qy - A0x * Avy + A0x * Qy = Py * Avx - Py * Qx - A0y * Avx + A0y * Qx
    // (Px * Qy - Py * Qx) = (Px * Avy - Py * Avx) + (A0y * Avx - A0x * Avy) + (A0x * Qy - A0y * Qx)
    // (Px * Qy - Py * Qx) = (Px * Bvy - Py * Bvx) + (B0y * Bvx - B0x * Bvy) + (B0x * Qy - B0y * Qx)
    // (Px * Qy - Py * Qx) = (Px * Cvy - Py * Cvx) + (C0y * Cvx - C0x * Cvy) + (C0x * Qy - C0y * Qx)
    //
    // Note that this gets a common (Px * Qy - Py * Qx) on the left side of everything, and the right side of each is
    //  now just a linear equation.
    // Do the same for the Pz/Px and Py/Pz pairints:
    //
    // (Pz * Qx - Px * Qz) = (Pz * Avx - Px * Avz) + (A0x * Avz - A0z * Avx) + (A0z * Qx - A0x * Qz)
    // (Pz * Qx - Px * Qz) = (Pz * Bvx - Px * Bvz) + (B0x * Bvz - B0z * Bvx) + (B0z * Qx - B0x * Qz)
    // (Pz * Qx - Px * Qz) = (Pz * Cvx - Px * Cvz) + (C0x * Cvz - C0z * Cvx) + (C0z * Qx - C0x * Qz)
    //
    // (Py * Qz - Pz * Qy) = (Py * Avz - Pz * Avy) + (A0z * Avy - A0y * Avz) + (A0y * Qz - A0z * Qy)
    // (Py * Qz - Pz * Qy) = (Py * Bvz - Pz * Bvy) + (B0z * Bvy - B0y * Bvz) + (B0y * Qz - B0z * Qy)
    // (Py * Qz - Pz * Qy) = (Py * Cvz - Pz * Cvy) + (C0z * Cvy - C0y * Cvz) + (C0y * Qz - C0z * Qy)
    //
    // This now turns into a series of 6 straight-up linear equations, which we can solve in, you know, the normal way.
    //  (don't worry I'm still gonna make it look difficult because it's late and I forget all the better math trickery
    //  to do this)
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

    // Now we're going to take each pair of those eliminate its initial P variable (leaving just the other)
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
    // Alright, now we can sub these into (half of) our original linear equations and rearrange in terms of Qx, Qy,
    //  and Qz, leaving us with three equations and three variables.
    // abvy*Px - abvx*Py - ab0y*Qx + ab0x*Qy = h0
    // abvx*Pz - abvz*Px - ab0x*Qz + ab0z*Qx = h2
    // abvz*Py - abvy*Pz - ab0z*Qy + ab0y*Qz = h4
    //
    // Make some more variables to make this easier
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
    // okay this is unintelligible garbage now but we're almost there:
    //
    // ([abvy/Pxd]*Pxz)*Qz + ([abvy/Pxd]*Pxx - [abvx/Pyd]*Pyx - ab0y)*Qx + (ab0x - [abvx/Pyd]*Pyy)*Qy
    //   = h0 - [abvy/Pxd]*Pxc + [abvx/Pyd]*Pyc
    // ([abvx/Pzd]*Pzy)*Qy + ([abvx/Pzd]*Pzz - [abvz/Pxd]*Pxz - ab0x)*Qz + (ab0z - [abvz/Pxd]*Pxx)*Qx
    //   = h2 - [abvx/Pzd]*Pzc + [abvz/Pxd]*Pxc
    // ([abvz/Pyd]*Pyx)*Qx + ([abvz/Pyd]*Pyy - [abvy/Pzd]*Pzy - ab0z)*Qy + (ab0y - [abvy/Pzd]*Pzz)*Qz
    //   = h4 - [abvz/Pyd]*Pyc + [abvy/Pzd]*Pzc
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

    // Alright after alllll that we can now solve for Qz, and then backsolve for everything else up the chain.
    double Qz = ((Qx1*Qy0 - Qx0*Qy1)*(Qx2*r0 - Qx0*r2) - (Qx2*Qy0 - Qx0*Qy2)*(Qx1*r0 - Qx0*r1))
      / ((Qx2*Qy0 - Qx0*Qy2)*(Qx0*Qz1 - Qx1*Qz0) - (Qx1*Qy0 - Qx0*Qy1)*(Qx0*Qz2 - Qx2*Qz0));

    double Qy = ((Qx0*Qz1 - Qx1*Qz0)*Qz + (Qx1*r0 - Qx0*r1))/(Qx1*Qy0 - Qx0*Qy1);

    double Qx = (r0 - Qy0*Qy - Qz0*Qz)/Qx0;

    double Px = (Pxz*Qz + Pxx*Qx + Pxc)/Pxd;
    double Py = (Pyx*Qx + Pyy*Qy + Pyc)/Pyd;
    double Pz = (Pzy*Qy + Pzz*Qz + Pzc)/Pzd;

    // Now, sum the (rounded, to deal with float precision issues) coordinates of the starting position together.
    //  The end. FINALLY.
    s64 sum = s64(std::round(Px)) + s64(std::round(Py)) + s64(std::round(Pz));
    PrintFmt("Part 2: {}\n", sum);
  }
}