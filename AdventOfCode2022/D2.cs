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

  internal static class D2
  {
    public static void Run(string input)
    {
      // R P S: A B C
      // R P S: X Y Z

      int points = 0;
      int roundIndex = 1;
      foreach (var line in input.Split("\n"))
      {
        var values = line.Trim().Split(" ", StringSplitOptions.RemoveEmptyEntries);
        if (values.Length == 0)
        {
          continue;
        }

        Debug.Assert(values.Length == 2);

        RPS enemyChoice = (RPS)(values[0][0] - 'A');
        RPS suggestedChoice = (RPS)(values[1][0] - 'X');

        int resultPoints = 0;
        if (enemyChoice == suggestedChoice)
        {
          resultPoints = 3;
        }
        else if ((enemyChoice != RPS.Rock || suggestedChoice != RPS.Scissors)
          && (suggestedChoice > enemyChoice || enemyChoice == RPS.Scissors && suggestedChoice == RPS.Rock))
        {
          resultPoints = 6;
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
