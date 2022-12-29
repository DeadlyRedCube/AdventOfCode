using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D20
  {
    [DebuggerDisplay("{value} (oi: {originalIndex})")]
    struct D
    {
      public long value;
      public int originalIndex;
    }

    public static void RunP1(string input)
      => Run(input, 1, 1);
    
    public static void RunP2(string input)
      => Run(input, 811589153, 10);
    
    public static void Run(string input, long multiplier, int iterationCount)
    {
      var list = input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
        .Select((x, i) => new D { value = long.Parse(x) * multiplier, originalIndex = i })
        .ToList();

      // This is a lookup table to find the index of the thing that started at the given index. (Original code was doing a list.IndexOf
      //  and took almost 20 seconds to run, now it takes 500ms)
      var indexLookup = Enumerable.Range(0, list.Count).ToList();

      for (int mix = 0; mix < iterationCount; mix++)
      {
        for (int i = 0; i < list.Count; i++)
        {
          int listI = indexLookup[i];
          var v = list[listI];

          // Do this modulo list count - 1 because our element is effectively not in the list as it spins around in it
          //  (so the list has effectively one fewer entry)
          int offset = (int)(v.value % (list.Count - 1));

          // Add (list.Count - 1) inside the mod to prevent negatives
          int expected = (listI + offset + (list.Count - 1)) % (list.Count - 1);
        
          list.RemoveAt(listI);
          list.Insert(expected, v);

          // Update our lookup - there's probably a faster way to do this, but meh
          for (int j = 0; j < list.Count; j++)
            { indexLookup[list[j].originalIndex] = j; }
        }
      }

      int zeroIndex = list.IndexOf(list.Where(v => v.value == 0).First());
      long sum = 
          list[(1000 + zeroIndex) % list.Count].value 
        + list[(2000 + zeroIndex) % list.Count].value 
        + list[(3000 + zeroIndex) % list.Count].value;

      Console.WriteLine($"Sum: {sum}");
    }
  }
}
