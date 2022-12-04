using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D1P1
  {
    public static void Run(string input)
    {
      int mostCals = 0;
      int bestElf = 0;

      int curElf = 1;
      int curCals = 0;
      foreach (var striter in (input + "\n").Split('\n'))
      {
        var str = striter.Trim();
        if (str.Length == 0)
        {
          if (curCals > mostCals)
          {
            mostCals = curCals;
            bestElf = curElf;
          }

          curCals = 0;
          curElf++;
        }
        else
        {
          curCals += int.Parse(str);
        }
      }

      Console.WriteLine($"Best elf: {bestElf} with {mostCals} calories");
    }
  }
}
