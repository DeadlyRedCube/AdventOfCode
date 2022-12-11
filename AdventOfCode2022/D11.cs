// #define WRITE_ROUNDS

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D11
  {
    enum Op
    {
      Add,
      Multiply,
    }

    class Monkey
    {
      public List<long> items = new List<long>();
      public Op op;
      public int? operand;
      public int divisibleBy;
      public int trueTarget;
      public int falseTarget;

      public long inspectCount = 0;
    }

    public static void RunP1(string input)
      => Run(input, false);
    
    public static void RunP2(string input)
      => Run(input, true);
    
    static void Run(string input, bool part2)
    {
      // Give our modulo an extra 3 here because in part 1 we have an extra divide by 3 and so we actually need to effectively track divisibility by 9
      int modBoundary = 3;
      var monkeys = new List<Monkey>();
      foreach (var line in input.Split('\n', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var tokens = line.Split(" ", StringSplitOptions.RemoveEmptyEntries).ToList();
        switch (tokens[0])
        {
        case "Monkey":
          monkeys.Add(new Monkey());
          Debug.Assert(monkeys.Count - 1 == int.Parse(tokens[1].Replace(":", "")));
          break;

        case "Starting":
          for (int i = 2; i < tokens.Count; i++)
          {
            monkeys.Last().items.Add(int.Parse(tokens[i].Replace(",", "")));
          }

          break;

        case "Operation:":
          monkeys.Last().op = tokens[4] switch
          {
            "+" => Op.Add,
            "*" => Op.Multiply,
            _ => throw new Exception("RONG"),
          };

          monkeys.Last().operand = (tokens[5] == "old") ? null : int.Parse(tokens[5]);
          break;

        case "Test:":
          monkeys.Last().divisibleBy  = int.Parse(tokens[3]);
          modBoundary *= monkeys.Last().divisibleBy;
          break;

        case "If":
          {
            int val = int.Parse(tokens[5]);
            switch (tokens[1])
            {
            case "true:": monkeys.Last().trueTarget = val; break;
            case "false:": monkeys.Last().falseTarget = val; break;
            default: Debug.Assert(false); break;
            }
          }
          break;

        default:
          Debug.Assert(false);
          break;
        }
      }

      // Now do the thing!
      int RoundCount = part2 ? 10000 : 20;
      for (int round = 0; round < RoundCount; round++)
      {
        for (int monkeyIndex = 0; monkeyIndex < monkeys.Count; monkeyIndex++)
        {
#if WRITE_ROUNDS
          Console.WriteLine($"Monkey {monkeyIndex}");
#endif
          var monkey = monkeys[monkeyIndex];

          foreach (var itemWorry in monkey.items)
          {
#if WRITE_ROUNDS
            Console.WriteLine($"  Item worry: {itemWorry}");
#endif
            monkey.inspectCount++;
            long calcWorry = itemWorry;
            long worryMod = monkey.operand ?? calcWorry;
            calcWorry = monkey.op switch
            {
              Op.Add => calcWorry + worryMod,
              Op.Multiply => calcWorry * worryMod,
              _ => throw new Exception("RONG"),
            };

            if (!part2)
            {
              calcWorry /= 3;
            }

            calcWorry %= modBoundary;

#if WRITE_ROUNDS
            Console.WriteLine($"    New level: {calcWorry}");
#endif

            if ((calcWorry % monkey.divisibleBy) == 0)
            {
#if WRITE_ROUNDS
              Console.WriteLine($"    Divisible by {monkey.divisibleBy}! Thrown to monkey {monkey.trueTarget}");
#endif
              monkeys[monkey.trueTarget].items.Add(calcWorry);
            }
            else
            {
#if WRITE_ROUNDS
              Console.WriteLine($"    Not divisible by {monkey.divisibleBy}! Thrown to monkey {monkey.falseTarget}");
#endif
              monkeys[monkey.falseTarget].items.Add(calcWorry);
            }
          }

          monkey.items.Clear();
        }

#if WRITE_ROUNDS
        Console.Write($"\nEnd Round {round} results:");
        for (int monkeyIndex = 0; monkeyIndex < monkeys.Count; monkeyIndex++)
        {
          Console.Write($"\n  Monkey {monkeyIndex}:");
          var monkey = monkeys[monkeyIndex];

          foreach (var itemWorry in monkey.items)
          {
            Console.Write($" {itemWorry}");
          }
        }
        Console.WriteLine("");
#endif
      }

      // sort by activity
      monkeys.Sort((a, b) => (a.inspectCount > b.inspectCount) ? -1 : 1);

      Console.WriteLine($"\n[P1] Top-two monkey business score: {monkeys[0].inspectCount * monkeys[1].inspectCount}");
    }
  }
}
