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
      public int value;
      public int originalIndex;
    }

    public static void Run(string input)
    {
      var list = input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
        .Select((x, i) => new D { value = int.Parse(x), originalIndex = i })
        .ToList();

      for (int i = 0; i < list.Count; i++)
      {
        int listI = list.IndexOf(list.Where(x => x.originalIndex == i).First());
        var v = list[listI];

        // Do this modulo list count - 1 because our element is effectively not in the list as it spins around in it
        //  (so the list has effectively one fewer entry)
        // We're going 
        int offset = Math.Sign(v.value) * (Math.Abs(v.value) % (list.Count - 1));

        // Add (list.Count - 1) inside the mod to prevent negatives
        int expected = (listI + offset + (list.Count - 1)) % (list.Count - 1);
        
        list.RemoveAt(listI);
        list.Insert(expected, v);
      }

      int zeroIndex = list.IndexOf(list.Where(v => v.value == 0).First());
      int sum = 
          list[(1000 + zeroIndex) % list.Count].value 
        + list[(2000 + zeroIndex) % list.Count].value 
        + list[(3000 + zeroIndex) % list.Count].value;

      Console.WriteLine($"[P1] Sum: {sum}");
    }
  }
}
