using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  struct V2
  {
    public int X;
    public int Y;

    public static int DistanceTo(V2 v1, V2 v2)
    {
      return Math.Max(Math.Abs(v1.X - v2.X), Math.Abs(v1.Y - v2.Y));
    }

    public void MoveToBeAdjacentTo(V2 h)
    {
      if (DistanceTo(this, h) <= 1)
        { return; }

      int xDir = h.X - this.X;
      int yDir = h.Y - this.Y;

      if (xDir == 0)
      {
        Debug.Assert(Math.Abs(yDir) == 2);
        Y += (yDir > 0) ? 1 : -1;
      }
      else if (yDir == 0) 
      {
        Debug.Assert(Math.Abs(xDir) == 2);
        X += (xDir > 0) ? 1 : -1;
      }
      else
      {
        Debug.Assert(Math.Abs(xDir) == 2 || Math.Abs(yDir) == 2);
        Debug.Assert(Math.Abs(xDir) + Math.Abs(yDir) == 3);

        xDir = (xDir > 0) ? 1 : -1;
        yDir = (yDir > 0) ? 1 : -1;

        X += xDir;
        Y += yDir;
      }
    }
  }
  
  
  
  internal class D9
  {
    public static void Run(string input)
    {
      HashSet<V2> tailSet = new HashSet<V2>();

      V2 head = new V2();
      V2 tail = new V2();

      tailSet.Add(tail);

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        int xDir = 0;
        int yDir = 0;
        
        var split = line.Split(" ", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
        Debug.Assert(split.Length == 2);
        int stepCount = int.Parse(split[1]);

        switch (split[0])
        {
        case "R": xDir = 1; break;
        case "L": xDir = -1; break;
        case "U": yDir = 1; break;
        case "D": yDir = -1; break;
        default: Debug.Assert(false); break;
        }

        for (int i = 0; i < stepCount; i++)
        {
          head.X += xDir;
          head.Y += yDir;

          tail.MoveToBeAdjacentTo(head);
          tailSet.Add(tail);
        }
      }

      Console.WriteLine($"Unique position count: {tailSet.Count}");
    }
  }
}
