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
    static int PriFromChar(char ch)
    {
      return (ch >= 'a') ? (ch - 'a' + 1) : (ch - 'A' + 27);
    }

    public static void Run(string text)
    {
      // Turn our text into a set of sacks (lists of characters) 
      var sacks = text.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
        .Select(x => x.Select(y => new {ch = y, pri = PriFromChar(y)}).ToList()).ToList();

      // Now that we have the sacks let's do the thing

      // Problem 1: find the common element in both halves (first and last) of each sack
      int priSum = 0;
      foreach (var sack in sacks)
      {
        var c1 = sack.Where((x, i) => i < sack.Count / 2);
        var c2 = sack.Where((x, i) => i >= sack.Count / 2);

        var intersect = c1.Intersect(c2).ToList();
        Debug.Assert(intersect.Count == 1);
        var v = intersect[0];

        Console.WriteLine($"Common element: {v.ch}, pri {v.pri}");
        priSum += v.pri;
      }

      Console.WriteLine($"Sum: {priSum}");

      // Problem 2: finding badges of groups (common element between sets of 3 lines)
      int badgePris = 0;
      for (int i = 0; i < sacks.Count; i += 3)
      {
        var badge = sacks[i + 0].Intersect(sacks[i + 1]).Intersect(sacks[i + 2]).ToList();
        Debug.Assert(badge.Count == 1);
        Console.WriteLine($"Badge for group: {badge[0].ch}");
        badgePris += badge[0].pri;
      }

      Console.Write($"Badge sum: {badgePris}");
    }
  }
}
