using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D10
  {
    public static void Run(string input)
    {
      int x = 1;
      int cycleIndex = 0;

      var testCycleIndices = new int[] { 20, 60, 100, 140, 180, 220 };

      int strengthSum = 0;

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        // Run cycle 1 regardless
        bool add = line.StartsWith("addx");

        for (int i = 0; i < (add ? 2 : 1); i++)
        {
          cycleIndex++;
          if (testCycleIndices.Contains(cycleIndex))
          {
            strengthSum += cycleIndex * x;
            Console.WriteLine($"Cycle {cycleIndex}, Strength: {cycleIndex * x}");
          }
        }

        if (add)
        {
          x += int.Parse(line.Split(" ", StringSplitOptions.TrimEntries)[1]);
        }
      }

      Console.WriteLine($"Signal Sum: {strengthSum}");
    }
  }
}
