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
      string stream = input.Replace("\r", "").Replace("\n", "");
      var q = new Queue<char>();
      var set = new HashSet<char>();
      for (int i = 0; i < input.Length; i++) 
      {
        if (q.Count == len)
        {
          set.Clear();
          foreach (char c in q)
            set.Add(c);

          if (set.Count == len)
          {
            Console.Write($"Marker: {i}");
            return;
          }

          q.Dequeue();
        }
        
        q.Enqueue(input[i]);
      }
    }

    public static void RunP1(string input)
      => Run(input, 4);

    public static void RunP2(string input)
      => Run(input, 14);
  }
}
