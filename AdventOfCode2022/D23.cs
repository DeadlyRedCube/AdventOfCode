using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using static System.Net.Mime.MediaTypeNames;

namespace AdventOfCode2022
{
  internal static class D23
  {
    [DebuggerDisplay("{X}, {Y}")]
    struct Vec
    {
      public int X;
      public int Y;

      public Vec(int x, int y)
        { X = x; Y = y; }

      public static Vec operator+(Vec a, Vec b)
        => new Vec{X = a.X + b.X, Y = a.Y + b.Y};

      public static Vec operator-(Vec a, Vec b)
        => new Vec{X = a.X - b.X, Y = a.Y - b.Y};

      public static Vec Min(Vec a, Vec b)
        => new Vec(Math.Min(a.X, b.X), Math.Min(a.Y, b.Y));

      public static Vec Max(Vec a, Vec b)
        => new Vec(Math.Max(a.X, b.X), Math.Max(a.Y, b.Y));
    }


    struct Move
    {
      public Vec testA;
      public Vec testB;
      public Vec testC;
      public Vec target;
    }

    public static void Run(string input)
    {
      var moves = new Move[]
      {
        new Move
        {
          testA  = new Vec(-1, -1),
          testB  = new Vec( 0, -1),
          testC  = new Vec( 1, -1),
          target = new Vec( 0, -1),
        },
        new Move
        {
          testA  = new Vec(-1,  1),
          testB  = new Vec( 0,  1),
          testC  = new Vec( 1,  1),
          target = new Vec( 0,  1),
        },
        new Move
        {
          testA  = new Vec(-1, -1),
          testB  = new Vec(-1,  0),
          testC  = new Vec(-1,  1),
          target = new Vec(-1,  0),
        },
        new Move
        {
          testA  = new Vec( 1, -1),
          testB  = new Vec( 1,  0),
          testC  = new Vec( 1,  1),
          target = new Vec( 1,  0),
        },
      }.ToList();

      var ring = new Vec[]
      {
        new Vec(-1, -1),
        new Vec( 0, -1),
        new Vec( 1, -1),
        new Vec( 1,  0),
        new Vec( 1,  1),
        new Vec( 0,  1),
        new Vec(-1,  1),
        new Vec(-1,  0),
      };
      
      var elves = new HashSet<Vec>();
      {
        var lines = input.Split('\n', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
        for (int y = 0; y < lines.Length; y++)
        {
          for (int x = 0; x < lines[y].Length; x++)
          {
            if (lines[y][x] == '#')
            {
              elves.Add(new Vec(x + 1, y + 1));
            }
          }
        }
      }

      for (int round = 1;; round++)
      {
        // Step 1: scan
        var elfMoves = new Dictionary<Vec, Vec>();
        var destinations = new Dictionary<Vec, int>();
        foreach (var elf in elves)
        {
          bool wantsToMove = ring.Select(d => elf + d).Any(elves.Contains);
          if (wantsToMove)
          {
            foreach (var move in moves)
            {
              if ( !elves.Contains(elf + move.testA)
                && !elves.Contains(elf + move.testB)
                && !elves.Contains(elf + move.testC))
              {
                var dest = elf + move.target;
                elfMoves.Add(elf, dest);
                destinations.TryAdd(dest, 0);
                destinations[dest]++;
                break;
              }
            }
          }
        }

        if (round == 10)
        {
          var min = elves.Aggregate(elves.First(), (m, e) => Vec.Min(m, e));
          var max = elves.Aggregate(elves.First(), (m, e) => Vec.Max(m, e));
          int emptyCount = (max.X - min.X + 1) * (max.Y - min.Y + 1) - elves.Count;
          Console.WriteLine($"[P1] Empty Count: {emptyCount}");
        }

        if (elfMoves.Count == 0)
        {
          Console.WriteLine($"[P2] Round: {round}");
          break;
        }

        var newElves = new HashSet<Vec>();
        foreach (var elf in elves)
        {
          if (elfMoves.ContainsKey(elf) && destinations[elfMoves[elf]] == 1)
          {
            newElves.Add(elfMoves[elf]);
          }
          else
          {
            newElves.Add(elf);
          }
        }

        elves = newElves;
        var cycled = moves[0];
        moves.RemoveAt(0);
        moves.Add(cycled);

        #if false
        {
          var min = elves.Aggregate(elves.First(), (m, e) => Vec.Min(m, e));
          var max = elves.Aggregate(elves.First(), (m, e) => Vec.Max(m, e));

          Console.WriteLine("\n");
          for (int y = min.Y; y <= max.Y; y++)
          {
            for (int x = min.X; x <= max.X; x++)
            {
              Console.Write(elves.Contains(new Vec(x, y)) ? "#" : ".");
            }
            Console.WriteLine();
          }
        }
        #endif
      }
    }
  }
}
