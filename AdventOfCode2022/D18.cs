using System;
using System.Collections.Generic;
using System.Diagnostics;
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

        // Add to our cube list (and add 1 to make sure that we have a "0" side of everything that is guaranteed empty
        cubes.Add(new Vec3(int.Parse(vals[0]) + 1, int.Parse(vals[1]) + 1, int.Parse(vals[2]) + 1));
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

      Console.WriteLine($"[P1] Sides: {sides}");

      var maxDim = cubes.Aggregate(new Vec3(), (max, c) => 
        new Vec3(Math.Max(max.x, c.x), Math.Max(max.y, c.y), Math.Max(max.z, c.z)));
      Debug.Assert(!cubes.Any(c => Math.Min(Math.Min(c.x, c.y), c.z) < 1));

      var fullGrid = new int[maxDim.x + 2, maxDim.y + 2, maxDim.z + 2];

      foreach (var c in cubes)
      {
        // 1 means "this is a cube"
        fullGrid[c.x, c.y, c.z] = 1;
      }

      var q = new Queue<Vec3>(new Vec3[]{new Vec3()}); // Start a queue with 0,0,0 in it (a known outside-the-space voxel

      while (q.Count > 0)
      {
        var c = q.Dequeue();

        foreach (var n in new Vec3[] { new Vec3(1, 0, 0), new Vec3(-1, 0, 0), new Vec3(0, 1, 0), new Vec3(0, -1, 0), new Vec3(0, 0, 1), new Vec3(0, 0, -1)})
        {
          var t = c + n;
          if (t.x < 0 || t.y < 0 || t.z < 0 || t.x >= fullGrid.GetLength(0) || t.y >= fullGrid.GetLength(1) || t.z >= fullGrid.GetLength(2))
            { continue; }

          if (fullGrid[t.x, t.y, t.z] == 0)
          {
            // This now becomes a 2, which means "is reachable from outside"
            fullGrid[t.x, t.y, t.z] = 2;
            q.Enqueue(t);
          }
        }
      }

      // Now that we have a record of all of the outside vs. inside vs. solid, do our check again
      int outerSides = 0;
      foreach (var c in cubes)
      {
        foreach (var n in new Vec3[] { new Vec3(1, 0, 0), new Vec3(-1, 0, 0), new Vec3(0, 1, 0), new Vec3(0, -1, 0), new Vec3(0, 0, 1), new Vec3(0, 0, -1)})
        {
          var t = c + n;
          if (fullGrid[t.x, t.y, t.z] == 2)
          {
            outerSides++;
          }
        }
      }

      Console.WriteLine($"[P2] Sides: {outerSides}");
    }
  }
}
