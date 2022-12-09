using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D1
  {
    public static void Run(string input)
    {
      int curCals = 0;
      var calCounts = new List<int>();
      foreach (var str in (input + "\n").Split('\n', StringSplitOptions.TrimEntries))
      {
        if (str.Length == 0)
        {
          // Empty line, so this is a break between elves, cache this elf's count and reset
          calCounts.Add(curCals);
          curCals = 0;
        }
        else
        {
          // It's a calorie input, add it to our counter
          curCals += int.Parse(str);
        }
      }

      calCounts.Sort((x, y) => y.CompareTo(x));

      // Output the calorie counts of the top 3 elves (while summing their calorie counts for the part 2 answer)
      Console.WriteLine("Top 3 elf counts:");

      int totalCals = 0;
      for (int i = 0; i < 3; i++)
      {
        Console.WriteLine($"  {calCounts[i]}");
        totalCals += calCounts[i];
      }

      Console.WriteLine($"\nTop-three total: {totalCals}");
    }
  }
}
