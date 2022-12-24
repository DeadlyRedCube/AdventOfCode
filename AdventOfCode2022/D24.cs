using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D24
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

      public static bool operator==(Vec a, Vec b)
        => a.X == b.X && a.Y == b.Y;

      public static bool operator!=(Vec a, Vec b)
        => !(a == b);

      public static Vec WrapTo(Vec v, Vec bounds)
        => new Vec((v.X + bounds.X) % bounds.X, (v.Y + bounds.Y) % bounds.Y);

      public static Vec Abs(Vec v)
        => new Vec(Math.Abs(v.X), Math.Abs(v.Y));

      public static int ManhattanDistance(Vec a, Vec b)
        => Math.Abs(a.X - b.X) + Math.Abs(a.Y - b.Y);
    }

    enum Type
    {
      Expedition,
      Down,
      Left,
      Up,
      Right,
    }


    struct Object
    {
      public Vec pos;
      public Type type;
    }

    class Blizzards
    {
      public List<Object> blizzards;
      public HashSet<Vec> positions;
    }

    static readonly Vec[] PotentialMoves = new Vec[]
      { new Vec(1, 0), new Vec(0, 1), new Vec(0, 0), new Vec(0, -1), new Vec(-1, 0) };

    static readonly int[] PotentialMoveFlags = new int[]
      { 1 << (int)Type.Right, 1 << (int)Type.Down, 1, 1 << (int)Type.Left, 1 << (int)Type.Up };

    static readonly int[] HorzMoveAxes = new int[] { 0, 1, 2, 3, 4 };
    static readonly int[] VertMoveAxes = new int[] { 1, 0, 2, 4, 3 };

    #if false
    static void MinSteps(long roundIndex, Vec exp, Dictionary<long, Blizzards> blizzards, long[,] shortestWalks)
    {
      var bounds = new Vec(shortestWalks.GetLength(0), shortestWalks.GetLength(1));
      var target = bounds + new Vec(-1, -1);

      if(exp == target)
      {
        // Terminal case, we reached the end
        return;
      }

      // Move all the blizzards 
      Blizzards newBlizzards;
      if (!blizzards.TryGetValue(roundIndex, out newBlizzards))
      {
        newBlizzards = new Blizzards();
        newBlizzards.blizzards = new List<Object>();
        blizzards.Add(roundIndex, newBlizzards);

        foreach (var b in blizzards[roundIndex - 1].blizzards)
        {
          var newPos = b.type switch
          {
            Type.Up    => Vec.WrapTo(b.pos + new Vec( 0, -1), bounds),
            Type.Down  => Vec.WrapTo(b.pos + new Vec( 0,  1), bounds),
            Type.Left  => Vec.WrapTo(b.pos + new Vec(-1,  0), bounds),
            Type.Right => Vec.WrapTo(b.pos + new Vec( 1,  0), bounds),
            _ => throw new InvalidOperationException(),
          };

          newBlizzards.blizzards.Add(new Object { pos = newPos, type = b.type });
        }

        newBlizzards.positions = newBlizzards.blizzards.Select(v => v.pos).ToHashSet();
      }

      // Prioritize whichever direction takes us along the farther axis first
      var delta = Vec.Abs(target - exp);

      foreach (var i in (delta.X > delta.Y) ? HorzMoveAxes : VertMoveAxes)
      {
        var t = exp + PotentialMoves[i];
        if (t.X < 0 || t.Y < 0 || t.X >= bounds.X || t.Y >= bounds.Y)
          { continue; }

        if (newBlizzards.positions.Contains(t))
          { continue; }

        if (shortestWalks[t.X, t.Y] > 
        bestSteps = Math.Min(bestSteps, MinSteps(roundIndex + 1, t, blizzards, bounds, bestSteps));
      }

      return bestSteps;
    }
    #endif

    static int Walk(int startStep, Vec start, Vec target, List<Blizzards> blizzards, Vec bounds)
    {
      // Use a priority queue here so we do it breadth-first (all step 1s before all step 2s, etc). Mostly to eliminate duplicates
      var q = new PriorityQueue<Tuple<Vec, int>, int>();
      q.Enqueue(new Tuple<Vec, int>(start, startStep), startStep);

      // Speaking of eliminating duplicates, here's a hash set of ones that we've done so that we can not try them again.
      var tested = new HashSet<Tuple<Vec, int>>();

      int best = int.MaxValue;
      while (q.Count > 0)
      {
        (var exp, var curStep) = q.Dequeue();
        if (curStep >= best)
          { continue; }

        // Get (or calculate) the blizzards for the next step (and cache it for others at this step)
        while (curStep >= blizzards.Count)
        {
          var newb = new Blizzards();
          newb.blizzards = new List<Object>();
          foreach (var b in blizzards[blizzards.Count - 1].blizzards)
          {
            var newPos = b.type switch
            {
              Type.Up    => Vec.WrapTo(b.pos + new Vec( 0, -1), bounds),
              Type.Down  => Vec.WrapTo(b.pos + new Vec( 0,  1), bounds),
              Type.Left  => Vec.WrapTo(b.pos + new Vec(-1,  0), bounds),
              Type.Right => Vec.WrapTo(b.pos + new Vec( 1,  0), bounds),
              _ => throw new InvalidOperationException(),
            };

            newb.blizzards.Add(new Object { pos = newPos, type = b.type });
          }

          blizzards.Add(newb);
          newb.positions = newb.blizzards.Select(v => v.pos).ToHashSet();
        }

        var newBlizzards = blizzards[curStep];

        // Now see where we can step, prioritizing the direction that takes us along the axis
        //  that we're farther from the exit along
        var delta = Vec.Abs(target - exp);

        foreach (var i in (delta.X > delta.Y) ? HorzMoveAxes : VertMoveAxes)
        {
          var t = exp + PotentialMoves[i];
          if (t == target)
          {
            // We stepped to the target so stash our best and we don't have to test any farther from this position.
            best = Math.Min(curStep, best);
            break;
          }

          // Don't test it if it's out of bounds (unless we're waiting in place at the start, that's fine)
          if (t != exp && (t.X < 0 || t.Y < 0 || t.X >= bounds.X || t.Y >= bounds.Y))
            { continue; }

          // If there's a blizzard where we're going, don't
          if (newBlizzards.positions.Contains(t))
            { continue; }

          // Alright we have a valid move so enqueue it unless we already did so
          var tup = new Tuple<Vec, int>(t, curStep + 1);
          if (!tested.Contains(tup))
          {
            tested.Add(tup);
            q.Enqueue(tup, curStep + 1);
          }
        }
      }

      return best;
    }

    public static void Run(string input)
    {
      var lines = input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
      int height = lines.Length - 2;
      int width = lines[0].Length - 2;

      var blizzardStarts = lines
        .Select((line, y) => line
          .Select((ch, x) => (ch, x))
          .Where(v => v.ch != '#' && v.ch != '.')
          .Select(v => new Object 
            { 
              pos = new Vec(v.x - 1, y - 1), 
              type = v.ch switch
              {
                '^' => Type.Up,
                'v' => Type.Down,
                '<' => Type.Left,
                '>' => Type.Right,
                _ => throw new InvalidDataException(),
              },
            }))
          .Aggregate(Enumerable.Empty<Object>(), Enumerable.Concat);
      var expStartPos = new Vec(0, -1);
      var target = new Vec(width - 1, height);

      var blizzards = new List<Blizzards>();
      blizzards.Add(new Blizzards { blizzards = blizzardStarts.ToList(), positions = blizzardStarts.Select(v => v.pos).ToHashSet() });
      
      var bounds = new Vec(width, height);

      var best = Walk(1, expStartPos, target, blizzards, bounds);
      Console.WriteLine($"[P1] Best: {best}");

      // We need to walk back to the start then back to the end again. Easy peasy.
      best = Walk(best + 1, target, expStartPos, blizzards, bounds);
      best = Walk(best + 1, expStartPos, target, blizzards, bounds);
      Console.WriteLine($"[P2] Best: {best}");
    }
  }
}
