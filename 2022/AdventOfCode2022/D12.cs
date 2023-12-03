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

      public Vec(int x, int y)
        { X = x; Y = y; }

      public static Vec operator+(Vec a, Vec b)
        => new Vec{X = a.X + b.X, Y = a.Y + b.Y};
    }

    public static void Run(string input)
    {
      // Find the first instance of '\n', which is our grid width, then remove all newlines and calc height
      int width = input.Replace("\r", "").IndexOf('\n');
      input = input.Replace("\r", "").Replace("\n", "");
      int height = input.Length / width;

      // Find the start and end positions, then replace them with their (given) elevations
      var start = new Vec(input.IndexOf('S') % width, input.IndexOf('S') / width);
      var end = new Vec(input.IndexOf('E') % width, input.IndexOf('E') / width);
      input = input.Replace('S', 'a').Replace('E', 'z');

      // Start our step count array with max distance in all spaces
      int[] stepCountArray = Enumerable.Repeat(int.MaxValue, width * height).ToArray();

      // Some lambdas to make it less error-prone to do the 2D lookup in the 1D enumerables
      var Elevation = (Vec v) => input[v.Y * width + v.X];
      var StepCount = (Vec v) => stepCountArray[v.Y * width + v.X];
      var SetStepCount = (Vec v, int steps) => { stepCountArray[v.Y * width + v.X] = steps; };

      // We're going to step backwards from the ending, so start the ending at a step count of 0
      SetStepCount(end, 0);

      // We're going to spider out from the end and (ultimately) fill any reachable spaces.
      Queue<Vec> queue = new Queue<Vec>();
      queue.Enqueue(end);
      while (queue.Count > 0)
      {
        var v = queue.Dequeue();
        foreach (Vec dir in new Vec[] { new Vec(-1, 0), new Vec(1, 0), new Vec(0, -1), new Vec(0, 1) })
        {
          var target = v + dir;

          // Ignore out of range indices
          if (target.X < 0 || target.Y < 0 || target.X >= width || target.Y >= height)
            { continue; }

          // We can't have stepped "up" to our exit if the elevation increase was more than one, so skip this
          if (Elevation(v) > Elevation(target) + 1)
            { continue; }

          // Test our step count and, if we can step here cheaper than has been done, enqueue the destination so
          //  we keep spidering.
          int steps = StepCount(v) + 1;
          if (steps < StepCount(target))
          {
            SetStepCount(target, steps);
            queue.Enqueue(target);
          }
        }
      }

      // Now we know the step count ot the exit (which is how many steps from end to start)
      Console.WriteLine($"[P1] Steps to exit: {StepCount(start)}");

      // Find the smallest distance to exit where the elevation is 'a'
      int minElevationAStepCount = input
        .Select((x, i) => (x, i, stepCountArray[i]))
        .Where(v => (v.x == 'a'))
        .Aggregate(int.MaxValue, (min, v) => min = Math.Min(min, v.Item3));

      Console.WriteLine($"[P2] Steps to bestLowStart: {minElevationAStepCount}");
    }
  }
}
