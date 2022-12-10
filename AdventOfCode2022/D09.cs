using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  [DebuggerDisplay("{X}, {Y}")]
  struct V2
  {
    public int X;
    public int Y;

    public void MoveToBeAdjacentTo(V2 h)
    {
      int xDir = h.X - X;
      int yDir = h.Y - Y;

      // if we're already adjacent (no more than 1 away along either axis), return
      if (Math.Max(Math.Abs(xDir), Math.Abs(yDir)) <= 1)
        { return; }

      Debug.Assert(Math.Abs(xDir) == 2 || Math.Abs(yDir) == 2);
      Debug.Assert(Math.Max(Math.Abs(xDir), Math.Abs(yDir)) == 2);

      xDir = Math.Clamp(xDir, -1, 1);
      yDir = Math.Clamp(yDir, -1, 1);

      X += xDir;
      Y += yDir;
    }
  }
  
  
  
  internal class D9
  {
    public static void RunP1(string input)
      => Run(input, 2);

    public static void RunP2(string input)
      => Run(input, 10);

    public static void Run(string input, int ropeCount)
    {
      HashSet<V2> tailSet = new HashSet<V2>();
      V2[] rope = new V2[ropeCount];

      tailSet.Add(rope.Last());

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        int xDir = 0;
        int yDir = 0;
        
        var split = line.Split(" ", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
        Debug.Assert(split.Length == 2);
        int stepCount = int.Parse(split[1]);

        switch (split[0])
        {
        case "R": xDir =  1; break;
        case "L": xDir = -1; break;
        case "U": yDir =  1; break;
        case "D": yDir = -1; break;
        default: Debug.Assert(false); break;
        }

        for (int i = 0; i < stepCount; i++)
        {
          rope[0].X += xDir;
          rope[0].Y += yDir;

          for (int j = 1; j < rope.Length; j++)
            { rope[j].MoveToBeAdjacentTo(rope[j - 1]); }

          tailSet.Add(rope.Last());
        }
      }

      Console.WriteLine($"Unique position count: {tailSet.Count}");
    }
  }
}
