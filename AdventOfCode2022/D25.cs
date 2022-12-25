using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace AdventOfCode2022
{
  internal static class D25
  {
    static string ToSNAFU(long v)
    {
      var stack = new Stack<char>();
      while (v > 0)
      {
        switch (v % 5)
        {
        case 0: stack.Push('0'); break;
        case 1: stack.Push('1'); break;
        case 2: stack.Push('2'); break;
        case 3: stack.Push('='); v += 5; break;
        case 4: stack.Push('-'); v += 5; break;
        }

        v /= 5;
      }

      var b = new StringBuilder();
      while (stack.Count > 0)
      {
        b.Append(stack.Pop());
      }

      return b.ToString();
    }

    static long FromSNAFU(string s)
    {
      long number = 0;
      foreach (char ch in s)
      {
        number *= 5;
        number += ch switch
        {
          '0' => 0,
          '1' => 1,
          '2' => 2,
          '-' => -1,
          '=' => -2,
          _ => throw new Exception(),
        };
      }

      return number;
    }

    public static void Run(string input)
    {
      long sum = 0;
      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var number = FromSNAFU(line);
        sum += number;
      }

      Console.WriteLine($"Sum (SNAFU): {ToSNAFU(sum)}");
    }
  }
}
