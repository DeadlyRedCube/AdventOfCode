using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D3
  {
    public static void RunP1(string text)
    {
      var sacks = new List<List<char>[]>();
      foreach(var line in text.Split("\n")) 
      {
        if (line.Trim().Length == 0)
        {
          continue; 
        }

        var sack = new List<char>[] { new List<char>(), new List<char>() };
        var halves = new string[] { line.Trim().Substring(0, line.Length / 2), line.Trim().Substring(line.Length / 2) };
        for (int i = 0; i < 2; i++)
        {
          foreach (var ch in halves[i])
          {
            sack[i].Add(ch);
          }
        }

        sacks.Add(sack);
      }

      // Now that we have the sacks let's do the thing
      int priSum = 0;
      foreach (var sack in sacks)
      {
        // Split in half
        var c1 = sack[0]!;
        var c2 = sack[1]!;

        var intersect = c1.Intersect(c2).ToList();
        Debug.Assert(intersect.Count == 1);

        var ch = intersect[0];
        int pri = (ch >= 'a') ? (ch - 'a' + 1) : (ch - 'A' + 27);

        Console.WriteLine($"Common element: {ch}, pri {pri}");
        priSum += pri;
      }

      Console.WriteLine($"Sum: {priSum}");
    }
  }
}
