using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D4
  {
    struct Range
    {
      public int Min;
      public int Max;
    }

    public static void Run(string input)
    {
      int fullOverlapCount = 0;
      int partialOverlapCount = 0;
      foreach(var line in input.Split('\n'))
      {
        if (line.Trim().Length == 0)
        {
          continue;
        }
        
        var strRanges = line.Split(',');

        var ranges = new Range[2];
        for (int i = 0; i < 2; i++)
        {
          var rangeEnds = strRanges[i].Split('-');

          ranges[i].Min = int.Parse(rangeEnds[0].Trim());
          ranges[i].Max = int.Parse(rangeEnds[1].Trim());
        }

        if ((ranges[0].Min <= ranges[1].Min && ranges[0].Max >= ranges[1].Max)
          || (ranges[1].Min <= ranges[0].Min && ranges[1].Max >= ranges[0].Max))
        {
          fullOverlapCount++;
          partialOverlapCount++;
          Console.WriteLine($"Ranges {ranges[0].Min}..{ranges[0].Max} and {ranges[1].Min}..{ranges[1].Max} fully overlap");
        }
        else if (ranges[0].Min > ranges[1].Max || ranges[0].Max < ranges[1].Min || ranges[1].Min > ranges[0].Max || ranges[1].Max < ranges[0].Min)
        {
          Console.WriteLine($"Ranges {ranges[0].Min}..{ranges[0].Max} and {ranges[1].Min}..{ranges[1].Max} are FULLY disjoint.");
        }
        else
        {
          Console.WriteLine($"Ranges {ranges[0].Min}..{ranges[0].Max} and {ranges[1].Min}..{ranges[1].Max} are partially disjoint.");
          partialOverlapCount++;
        }
      }

      Console.WriteLine($"Full overlaps: {fullOverlapCount}");
      Console.WriteLine($"Partial overlaps: {partialOverlapCount}");
    }
  }
}
