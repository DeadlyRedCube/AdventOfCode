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

    public static void RunP1(string input)
      => Run(input, false);

    public static void RunP2(string input)
      => Run(input, true);

    static Stack<char> Reverse(Stack<char> chars) 
      => new Stack<char>(chars); // Construct a new stack using the original stack as an input (pushing in reverse order)

    public static void Run(string input, bool multipleAtOnce)
    {
      var stacks = new List<Stack<char>>();
      var moves = new List<Move>();
      foreach (var line in input.Split("\n"))
      {
        if (line.Contains('['))
        {
          // This line is part of the box diagram, scan for letters
          var stackElements = line.Select((ch, i) => new {ch, i = (i - 1) / 4}).Where(x => char.IsLetter(x.ch));
          var maxStackIndex = stackElements.Aggregate(0, (max, x) => Math.Max(max, x.i));

          // Ensure stacks is big enough for the number of stacks we need
          while (stacks.Count <= maxStackIndex)
            { stacks.Add(new Stack<char>()); }

          // Add our stack elements
          foreach (var e in stackElements)
            { stacks[e.i].Push(e.ch); }
        }
        else if (line.Contains("move"))
        {
          var m = line.Split(" ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
          Debug.Assert(m[0] == "move" && m[2] == "from" && m[4] == "to");
          moves.Add(new Move { Count = int.Parse(m[1]), SrcStack = int.Parse(m[3]) - 1, DstStack = int.Parse(m[5]) - 1 });
        }
      }

      //Reverse each stack (by constructing a new stack using the original stack as an IEnumerable input)
      for (int i = 0; i < stacks.Count; i++)
        { stacks[i] = Reverse(stacks[i]); }

      // Great now do the moves
      foreach (var move in moves)
      {
        if (multipleAtOnce) // Puzzle 2
        {
          // Doing this the lazy way, if we're moving multiple at once, pop them into a 
          //  temporary stack then pop from there to the new location (moving them effectively in order)
          var tempStack = new Stack<char>();
          for (int i = 0; i < move.Count; i++)
            { tempStack.Push(stacks[move.SrcStack].Pop()); }

          for (int i = 0; i < move.Count; i++)
            { stacks[move.DstStack].Push(tempStack.Pop()); }
        }
        else // Puzzle 1
        {
          // Pop from one place and push to the other. Easy peasy.
          for (int i = 0; i < move.Count; i++)
            { stacks[move.DstStack].Push(stacks[move.SrcStack].Pop()); }
        }
      }

      Console.WriteLine("\n----- Shuffled\n");

      var b = new StringBuilder();
      for (int i = 0; i < stacks.Count; i++)
      {
        Console.Write("Stack: ");
        foreach (char c in stacks[i])
          { Console.Write($"{c} "); }
        Console.WriteLine("");

        b.Append(stacks[i].Peek());
      }

      Console.WriteLine($"\nResult: {b}");
    }
  }
}
