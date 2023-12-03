using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D15
  {
    [DebuggerDisplay("({X}, {Y})")]
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

      public static int MDist(Vec a, Vec b)
      {
        return Math.Abs(a.X - b.X) + Math.Abs(a.Y - b.Y);
      }

      public static bool operator==(Vec a, Vec b)
        { return a.X == b.X && a.Y == b.Y; }

      public static bool operator!=(Vec a, Vec b)
        { return !(a == b); }
    }

    [DebuggerDisplay("Beacon @ {pos}")]
    class Beacon
    {
      public Vec pos;
    }

    [DebuggerDisplay("Sensor @ {pos}, Nearest: {nearest}")]
    class Sensor
    {
      public Vec pos;
      public Beacon nearest = null!;

      public int DistanceToNearest
      {
        get { return Vec.MDist(pos, nearest.pos); }
      }
    }


    [DebuggerDisplay("({a}, {b})")]
    struct Span
    {
      public Span() {}
      public Span(int a, int b) { this.a = a; this.b = b; }
      public int a;
      public int b;

      public Span? CombineWith(Span other)
      {
        if (b < other.a - 1 || a > other.b + 1)
          return null;

        return new Span(Math.Min(a, other.a), Math.Max(b, other.b));
      }
    }

    public static void Run(string input)
    {
      var beaconList = new List<Beacon>();
      var beaconLookup = new Dictionary<Vec, Beacon>();
      var sensors = new List<Sensor>();

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var ints = 
          line.Split(
            new char[] { ' ', '=', ',', ':'})
          .Where(x => int.TryParse(x, out var v))
          .Select(x => int.Parse(x))
          .ToList();

        Debug.Assert(ints.Count == 4);

        Beacon b;
        Vec beaconPos = new Vec(ints[2], ints[3]);
        if (beaconLookup.ContainsKey(beaconPos))
        {
          b = beaconLookup[beaconPos];
        }
        else
        {
          b = new Beacon { pos = beaconPos };
          beaconLookup.Add(beaconPos, b);
          beaconList.Add(b);
        }
        
        var s = new Sensor { pos = new Vec(ints[0], ints[1]), nearest = b };
        sensors.Add(s);
      }

      const int puzMax = 4_000_000;
      const int testY = 2_000_000;
      for (int y = 0; y <= puzMax; y++)
      {
        var spans = new List<Span>();
        foreach (var sens in sensors)
        {
          int nd = sens.DistanceToNearest;

          int rd = Math.Abs(sens.pos.Y - y);
          if (rd > nd)
            { continue; }

          var span = new Span { 
            a = sens.pos.X - (nd - rd),
            b = sens.pos.X + (nd - rd)};

          while (true)
          {
            bool overlapped = false;
            for (int i = 0; i < spans.Count; i++)
            {
              var c = span.CombineWith(spans[i]);
              if (c != null)
              {
                overlapped = true;
                spans.RemoveAt(i);
                span = c.Value;
                break;
              }
            }

            if (!overlapped)
            {
              spans.Add(span);
              break;
            }
          }
        }

        if (y == testY)
        {
          int count = 0;
          foreach (var s in spans)
          {
            count += s.b - s.a + 1;
          }

          foreach (var b in beaconList)
          {
            foreach (var s in spans)
            {
              if (b.pos.Y == y && b.pos.X >= s.a && b.pos.X <= s.b)
              {
                count--;
                break;
              }
            }
          }

          Console.WriteLine($"[P1] Count of {testY} positions with no beacon: {count}");
        }

        if (spans.Count > 1)
        {
          Debug.Assert(spans.Count == 2);
          var s = (spans[0].a < spans[1].a) ? spans[1] : spans[0];

          int x = s.a - 1;
          Console.WriteLine($"[P2] ({x}, {y}): Score is {((long)x) * puzMax + y}");
        }
      }
    }
  }
}
