using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D6
  {
    public static void Run(string input, int len)
    {
      var set = new HashSet<char>();
      for (int i = 0; i < input.Length; i++) 
      {
        set.Clear();
        for (int j = 0; j < len; j++)
          set.Add(input[i + j]);

        if (set.Count == len)
        {
          Console.Write($"Marker: {i + len}");
          return;
        }
      }
    }

    public static void RunP1(string input)
      => Run(input, 4);

    public static void RunP2(string input)
      => Run(input, 14);
  }
}
