using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D5
  {
    struct Move
    {
      public int Count;
      public int SrcStack;
      public int DstStack;
    }

    public static void Run(string input, bool multipleAtOnce)
    {
      var stacks = new List<Stack<char>>();
      var moves = new List<Move>();
      foreach (var line in input.Split("\n"))
      {
        if (line.Contains('['))
        {
          for (int i = 0; i < line.Length; i++)
          {
            if (char.IsLetter(line[i]))
            {
              int stackIndex = (i - 1) / 4;

              while (stacks.Count <= stackIndex)
              {
                stacks.Add(new Stack<char>());
              }

              // Push into the stack - this is actually going to have them upside-down, we'll fix that later
              stacks[stackIndex].Push(line[i]);
            }
          }
        }
        else if (line.Contains("move"))
        {
          var m = line.Split(" ", StringSplitOptions.RemoveEmptyEntries);
          Debug.Assert(m[0] == "move" && m[2] == "from" && m[4] == "to");

          moves.Add(new Move { Count = int.Parse(m[1]), SrcStack = int.Parse(m[3]) - 1, DstStack = int.Parse(m[5]) - 1 });
        }
      }

      //Reverse our stacks
      for (int i = 0; i < stacks.Count; i++)
      {
        var oldStack = stacks[i];
        stacks[i] = new Stack<char>();
        while (oldStack.Count > 0)
        {
          stacks[i].Push(oldStack.Pop());
        }

        Console.Write("Stack: ");
        foreach (char c in stacks[i])
        {
          Console.Write($"{c} ");
        }
        Console.WriteLine("");
      }

      // Great now do the moves
      foreach (var move in moves)
      {
        if (multipleAtOnce)
        {
          var tempStack = new Stack<char>();
          for (int i = 0; i < move.Count; i++)
          {
            tempStack.Push(stacks[move.SrcStack].Pop());
          }

          for (int i = 0; i < move.Count; i++)
          {
            stacks[move.DstStack].Push(tempStack.Pop());
          }
        }
        else
        {
          for (int i = 0; i < move.Count; i++)
          {
            stacks[move.DstStack].Push(stacks[move.SrcStack].Pop());
          }
        }
      }

      Console.WriteLine("\n----- Shuffled\n");

      var b = new StringBuilder();
      for (int i = 0; i < stacks.Count; i++)
      {
        Console.Write("Stack: ");
        foreach (char c in stacks[i])
        {
          Console.Write($"{c} ");
        }
        Console.WriteLine("");

        b.Append(stacks[i].Peek());
      }

      Console.WriteLine($"\nResult: {b}");
    }
  }
}
