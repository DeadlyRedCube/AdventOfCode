using System.Diagnostics;

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

      public static bool operator==(Vec a, Vec b)
        => a.X == b.X && a.Y == b.Y;

      public static bool operator!=(Vec a, Vec b)
        => !(a == b);
    }

    class Move
    {
      public Vec[] tests = new Vec[0];
      public Vec target;
    }

    public static void Run(string input)
    {
      var moves = new Move[]
      {
        new Move { tests = new Vec[] { new Vec(-1, -1), new Vec( 0, -1), new Vec( 1, -1) }, target = new Vec( 0, -1), },
        new Move { tests = new Vec[] { new Vec(-1,  1), new Vec( 0,  1), new Vec( 1,  1) }, target = new Vec( 0,  1), },
        new Move { tests = new Vec[] { new Vec(-1, -1), new Vec(-1,  0), new Vec(-1,  1) }, target = new Vec(-1,  0), },
        new Move { tests = new Vec[] { new Vec( 1, -1), new Vec( 1,  0), new Vec( 1,  1) }, target = new Vec( 1,  0), },
      }.AsReadOnly();

      int initialMoveIndex = 0;

      // get the x, y coordinates for all 8 directions (centered around 0, 0)
      //  (yeah this is a nightmarish assembly but I'm trying to push LINQ a bit and it was less LOC so here we gooooooo)
      var ring = Enumerable.Range(-1, 3)
        .Select(y => Enumerable.Range(-1, 3).Select(x => new Vec(x, y)))
        .Aggregate(Enumerable.Empty<Vec>(), Enumerable.Concat)
        .Where(v => v != new Vec(0, 0))
        .ToArray().AsReadOnly();
      
      // Convert all of the '#' marks in the input into elf coordinates
      //  (ditto the above nightmarishness)
      var elves = input
        .Split('\n')
        .Select((line, y) => line.Select((ch, x) => (ch, x)).Where(v => v.ch == '#').Select(v => new Vec(v.x, y)))
        .Aggregate(Enumerable.Empty<Vec>(), Enumerable.Concat)
        .ToHashSet();

      for (int round = 1;; round++, initialMoveIndex++)
      {
        // Step 1: scan for moves
        var elfMoves = new Dictionary<Vec, Vec>();
        var destinations = new Dictionary<Vec, int>();

        foreach (var elf in elves)
        {
          // An elf only wants to move if it's near any other elves
          if (ring.Select(d => elf + d).Any(elves.Contains))
          {
            // Iterate through our moves, rotated via our initial move index
            foreach (var move in Enumerable.Range(initialMoveIndex, 4).Select(i => moves[i % 4]))
            {
              // If none of the test directions contain an elf, we'll move this way
              if (!move.tests.Select(d => elf + d).Any(elves.Contains))
              {
                // We'll move!
                elfMoves.Add(elf, elf + move.target);
                destinations.TryAdd(elfMoves[elf], 0);
                destinations[elfMoves[elf]]++;
                break;
              }
            }
          }
        }

        if (elfMoves.Count == 0)
        {
          // Nobody wanted to move so we're done!
          Console.WriteLine($"[P2] Round: {round}");
          break;
        }

        // Update all of the elves in the set (moving them if they have a move) by creating a new hash set
        elves = elves.Select(e => (elfMoves.ContainsKey(e) && destinations[elfMoves[e]] == 1) ? elfMoves[e] : e).ToHashSet();

        if (round == 10)
        {
          var min = elves.Aggregate(elves.First(), Vec.Min);
          var max = elves.Aggregate(elves.First(), Vec.Max);
          Console.WriteLine($"[P1] Empty Count: {(max.X - min.X + 1) * (max.Y - min.Y + 1) - elves.Count}");
        }
      }
    }
  }
}
