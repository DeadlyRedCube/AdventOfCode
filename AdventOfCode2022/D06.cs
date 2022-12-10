using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D6
  {
    public static void Run(string inputStr, int len)
    {
      var input = inputStr.ToList();
      for (int i = 0; i < input.Count; i++) 
      {
        // Make a set out of the next "len" characters, if the count == len then they were
        //  all unique and we've found our marker
        if ((new HashSet<char>(input.GetRange(i, len))).Count == len)
        {
          Console.WriteLine($"Marker: {i + len}");
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
