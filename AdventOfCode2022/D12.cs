using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D12
  {
    [DebuggerDisplay("[{X}, {Y}]")]
    struct Vec
    {
      public int X;
      public int Y;

      public static Vec operator+(Vec a, Vec b)
      {
        return new Vec{X = a.X + b.X, Y = a.Y + b.Y};
      }
    }

    public static void Run(string input)
    {
      int startX = 0, startY = 0;
      int endX = 0, endY = 0;
      List<List<int>> parsedGridLines = new List<List<int>>();
      {
        int y = 0;
        foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
        {
          var l = new List<int>();
          parsedGridLines.Add(l);
          int height = 0;
          int x = 0;
          foreach (var ch in line)
          {
            switch (ch)
            {
            case 'S':
              height = 0;
              startX = x;
              startY = y;
              break;

            case 'E':
              height = 'z' - 'a';
              endX = x;
              endY = y;
              break;

            default:
              height = ch - 'a';
              break;
            }

            x++;
            l.Add(height);
          }

          y++;
        }
      }

      int[,] elevations = new int[parsedGridLines[0].Count, parsedGridLines.Count];
      int[,] stepCount = new int[elevations.GetLength(0), elevations.GetLength(1)];

      for (int y = 0; y < parsedGridLines.Count; y++) 
      {
        Debug.Assert(parsedGridLines[y].Count == elevations.GetLength(0));

        for (int x = 0; x < parsedGridLines[y].Count; x++)
        {
          elevations[x, y] = parsedGridLines[y][x];
          stepCount[x, y] = int.MaxValue;
        }
      }

      // Yay okay now we have the grid and our starting point
      stepCount[endX, endY] = 0;

      Queue<Vec> queue = new Queue<Vec>();
      queue.Enqueue(new Vec{X = endX, Y = endY});

      while (queue.Count > 0)
      {
        var v = queue.Dequeue();

        int curElev = elevations[v.X, v.Y];

        foreach (Vec dir in new Vec[] { new Vec{X = -1, Y = 0}, new Vec{X = 1, Y = 0}, new Vec{X = 0, Y = -1}, new Vec{X = 0, Y = 1} })
        {
          var target = v + dir;

          if (target.X < 0 || target.Y < 0 || target.X >= elevations.GetLength(0) || target.Y >= elevations.GetLength(1))
            { continue; }

          if (curElev > elevations[target.X, target.Y] + 1)
            { continue; }

          int steps = stepCount[v.X, v.Y] + 1;
          if (steps < stepCount[target.X, target.Y])
          {
            stepCount[target.X, target.Y] = steps;
            queue.Enqueue(target);
          }
        }
      }


      // Okay now we're done
      int stepsToExit = stepCount[startX, startY];

      Console.WriteLine($"Steps to exit: {stepsToExit}");
    }
  }
}
