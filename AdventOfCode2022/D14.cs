using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D14
  {
    struct Vec
    {
      public Vec() {}
      public Vec(int x, int y) { X = x; Y = y; }
      public int X;
      public int Y;

      public static Vec operator+(Vec a, Vec b)
      {
        return new Vec{X = a.X + b.X, Y = a.Y + b.Y};
      }

      public static Vec operator-(Vec a, Vec b)
      {
        return new Vec{X = a.X - b.X, Y = a.Y - b.Y};
      }

      public void ClampTo1()
      {
        X = Math.Clamp(X, -1, 1);
        Y = Math.Clamp(Y, -1, 1);
      }
    }

    public static void PrintGrid(char[,] grid)
    {
      for (int y = 0; y < grid.GetLength(1); y++)
      {
        for (int x = 0; x < grid.GetLength(0); x++)
        {
          Console.Write(grid[x,y]);
        }
        Console.WriteLine();
      }

      Console.WriteLine("\n\n");
    }

    public static void Run(string input)
    {
      List<List<Vec>> sequences = new List<List<Vec>>();

      Vec sandEntry = new Vec {X = 500, Y = 0};
      Vec min = sandEntry;
      Vec max = sandEntry;
      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var l = new List<Vec>();
        sequences.Add(l);
        foreach (var s in line.Split("->", StringSplitOptions.TrimEntries))
        {
          var vs = s.Split(",", StringSplitOptions.TrimEntries);
          var v = new Vec { X = int.Parse(vs[0]), Y = int.Parse(vs[1]) };
          l.Add(v);

          min.X = Math.Min(min.X, v.X);
          min.Y = Math.Min(min.Y, v.Y);
          max.X = Math.Max(max.X, v.X);
          max.Y = Math.Max(max.Y, v.Y);
        }
      }

      var grid = new char[max.X - min.X + 1, max.Y - min.Y + 1];
      for (int y = 0; y < grid.GetLength(1); y++)
      {
        for (int x = 0; x < grid.GetLength(0); x++)
        {
          grid[x, y] = '.';
        }
      }

      foreach (var segments in sequences)
      {
        for (int i = 0; i < segments.Count - 1; i++)
        {
          var start = segments[i + 0] - min;
          var end = segments[i + 1] - min;

          var dir = (end - start);
          dir.ClampTo1();

          grid[start.X, start.Y] = '#';
          while (true)
          {
            start += dir;
            grid[start.X, start.Y] = '#';
            if (start.X == end.X && start.Y == end.Y)
              { break; }
          }
        }
      }

      Console.WriteLine("Start Grid:");
      PrintGrid(grid);

      int sandCount = 0;
      sandEntry -= min;
      while (true)
      {
        Vec p = sandEntry;

        while (true)
        {
          Vec pn = p;
          foreach (var delta in new Vec[] { new Vec(0, 1), new Vec(-1, 1), new Vec(1, 1)})
          {
            Vec t = p + delta;
            if (t.X < 0 || t.X >= grid.GetLength(0) || t.Y >= grid.GetLength(1))
            {
              goto DoneFilling;
            }

            if (grid[t.X, t.Y] == '.')
            {
              pn = t;
              break;
            }
          }

          if (pn.Y == p.Y)
          {
            // Came to rest
            grid[p.X, p.Y] = 'o';
            sandCount++;
            break;
          }

          p = pn;
        }

        //PrintGrid(grid);
        Console.Write("");
      }

DoneFilling:;
      Console.WriteLine($"There were {sandCount} grains before rest.");
    }
  }
}
