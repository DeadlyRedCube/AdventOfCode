using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D18
  {
    struct Vec3
    {
      public Vec3() { }
      public Vec3(int x, int y, int z) { this.x = x; this.y = y; this.z = z; }
      public int x;
      public int y;
      public int z;

      public static Vec3 operator+(Vec3 a, Vec3 b)
        => new Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public static void Run(string input)
    {
      HashSet<Vec3> cubes = new HashSet<Vec3>();

      foreach (var line in input.Split('\n', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries)) 
      {
        var vals = line.Split(',');

        cubes.Add(new Vec3(int.Parse(vals[0]), int.Parse(vals[1]), int.Parse(vals[2])));
      }

      int sides = 0;
      foreach (var c in cubes)
      {
        foreach (var n in new Vec3[] { new Vec3(1, 0, 0), new Vec3(-1, 0, 0), new Vec3(0, 1, 0), new Vec3(0, -1, 0), new Vec3(0, 0, 1), new Vec3(0, 0, -1)})
        {
          if (!cubes.Contains(c + n))
          {
            sides++;
          }
        }
      }

      Console.WriteLine($"Sides: {sides}");
    }
  }
}
