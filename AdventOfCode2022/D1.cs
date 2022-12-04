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
      foreach (var striter in (input + "\n").Split('\n'))
      {
        var str = striter.Trim();
        if (str.Length == 0)
        {
          calCounts.Add(curCals);
          curCals = 0;
        }
        else
        {
          curCals += int.Parse(str);
        }
      }

      calCounts.Sort((x, y) => y.CompareTo(x));

      int totalCals = 0;
      Console.WriteLine("Top 3 elf counts:");
      for (int i = 0; i < 3; i++)
      {
        Console.WriteLine($"  {calCounts[i]}");
        totalCals += calCounts[i];
      }

      Console.WriteLine($"\nTotal: {totalCals}");
    }
  }
}
