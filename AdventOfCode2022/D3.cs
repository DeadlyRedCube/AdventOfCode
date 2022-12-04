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
        int pri = PriFromChar(ch);

        Console.WriteLine($"Common element: {ch}, pri {pri}");
        priSum += pri;
      }

      Console.WriteLine($"Sum: {priSum}");

      // Problem 2: finding badges of groups
      int badgePris = 0;
      for (int i = 0; i < sacks.Count; i += 3)
      {
        var s1 = sacks[i + 0][0].Union(sacks[i + 0][1]);
        var s2 = sacks[i + 1][0].Union(sacks[i + 1][1]);
        var s3 = sacks[i + 2][0].Union(sacks[i + 2][1]);
        
        var badge = s1.Intersect(s2).Intersect(s3).ToList();
        Debug.Assert(badge.Count == 1);
        Console.WriteLine($"Badge for group: {badge[0]}");
        badgePris += PriFromChar(badge[0]);
      }

      Console.Write($"Badge sum: {badgePris}");
    }
  }
}
