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

      var lineXInRangeOfSensors = new HashSet<int>();

      const int y = 2000000;

      foreach (var s in sensors)
      {
        int nd = s.DistanceToNearest;

        int rd = Math.Abs(s.pos.Y - y);
        if (rd > nd)
          { continue; }

        int span = nd - rd;
        for (int x = s.pos.X - span; x <= s.pos.X + span; x++)
        {
          lineXInRangeOfSensors.Add(x);
        }
      }

      foreach (var b in beaconList)
      {
        if (b.pos.Y == y)
        {
          lineXInRangeOfSensors.Remove(b.pos.X);
        }
      }
    }
  }
}
