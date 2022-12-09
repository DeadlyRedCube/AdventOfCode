using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  enum RPS
  {
    Rock,
    Paper,
    Scissors
  };


  enum Result
  {
    Loss,
    Tie,
    Win
  };

  internal static class D2
  {
    public static void RunP1(string input)
      => Run(input, true);

    public static void RunP2(string input)
      => Run(input, false);

    static Result ResultOf(RPS a, RPS b)
    {
      // I didn't come up with this on the day until after submission, but what this does is:
      // Gets the difference of the two (which will be in the range -2 to 2) 
      // Adds 3 (gets rid of negative results)
      //  at this point, any ties are 0, wins % 3 are 1, and losses % 3 are 2
      // However, we want losses to be 0, ties 1, and wins 2, so add one
      // Take that result modulo 3 and cast it to a DesiredResult
      return (Result)(((int)b - (int)a + 3 + 1) % 3);
    }

    public static void Run(string input, bool runPart1)
    {
      
      int points = 0;
      int roundIndex = 1;
      foreach (var line in input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
      {
        var values = line.Split(" ", StringSplitOptions.RemoveEmptyEntries);

        Debug.Assert(values.Length == 2);

        RPS enemyChoice = (RPS)(values[0][0] - 'A');

        RPS suggestedChoice;
        int resultPoints;
        
        if (runPart1)
        {
          // For part 1's puzzle we treat X Y Z as Rock, Paper, Scissors, and decide that's the result to throw
          //  in response. Result is 0 on a loss, 3 on tie, and 6 on win, so multiply our result value by 3.
          suggestedChoice = (RPS)(values[1][0] - 'X');
          resultPoints = 3 * (int)ResultOf(enemyChoice, suggestedChoice);
        }
        else
        {
          // for part 2, X Y and Z map to lose, tie, and win respectively, so use that to figure out what we throw
          Result desiredResult = (Result)(values[1][0] - 'X');
        
          // To figure out our choice, add our desired result, then subtract 1 (so that a tie is no change), then
          //  add 3 to eliminate negative values. Then take that all mod 3 to get a move.
          suggestedChoice = (RPS)(((int)enemyChoice + (int)desiredResult - 1 + 3) % 3);
          resultPoints = (int)(desiredResult) * 3;
        }

        int roundPoints = resultPoints + 1 + (int)suggestedChoice;
        points += roundPoints;

        Console.WriteLine($"Round {roundIndex}: {(RPS)enemyChoice} ({values[0]}) vs {(RPS)suggestedChoice} ({values[1]}). Result: {resultPoints}, {roundPoints}");
        roundIndex++;
      }

      Console.WriteLine($"------\nFinal result: {points} points");
    }
  }
}
