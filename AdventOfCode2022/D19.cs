using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D19
  {
    class Blueprint
    {
      public int oreCostOre;
      public int clayCostOre;
      public int obsidianCostOre;
      public int obsidianCostClay;
      public int geodeCostOre;
      public int geodeCostObsidian;
    }

    [Flags]
    enum AllowedRobots 
    {
      Ore       = 0x01,
      Clay      = 0x02,
      Obsidian  = 0x04,
      Geode     = 0x08,

      None      = 0x00,
      All       = 0x0F,
    }

    static int Simulate(
      Blueprint bp, 
      int oreRobotCount, 
      int clayRobotCount, 
      int obsidianRobotCount, 
      int geodeRobotCount, 
      int oreCount, 
      int clayCount, 
      int obsidianCount, 
      int geodeCount, 
      AllowedRobots allowedRobots, 
      int timeRemaining)
    {
      int maxOreCost = Math.Max(bp.clayCostOre, Math.Max(bp.geodeCostOre, bp.obsidianCostOre));

      while (true)
      {
        // If we're only 1 from the end, there's no point in buying a robot because it won't finish in time to do anything.
        if (timeRemaining > 1)
        {
          int bestResult = 0;
          // We can buy a geode robot if we have its cost and we didn't skip buying it in the past.
          if ((allowedRobots & AllowedRobots.Geode) != 0 
            && obsidianCount >= bp.geodeCostObsidian 
            && oreCount >= bp.geodeCostOre)
          {
            allowedRobots &= ~AllowedRobots.Geode;
            bestResult = Simulate(
              bp, 
              oreRobotCount, 
              clayRobotCount, 
              obsidianRobotCount, 
              geodeRobotCount + 1, 
              oreCount - bp.geodeCostOre + oreRobotCount, 
              clayCount + clayRobotCount, 
              obsidianCount - bp.geodeCostObsidian + obsidianRobotCount, 
              geodeCount + geodeRobotCount, 
              AllowedRobots.All, 
              timeRemaining - 1);
          }

          // With obsidian robot, we have the additional condition that if we're making enough obsidian every minute
          //  to fulfill the geode robot's cost we never need to build one again.
          if (obsidianRobotCount < bp.geodeCostObsidian 
            && (allowedRobots & AllowedRobots.Obsidian) != 0 
            && clayCount >= bp.obsidianCostClay 
            && oreCount >= bp.obsidianCostOre)
          {
            allowedRobots &= ~AllowedRobots.Obsidian;
            bestResult = Math.Max(bestResult, Simulate(
              bp, 
              oreRobotCount, 
              clayRobotCount, 
              obsidianRobotCount + 1, 
              geodeRobotCount, 
              oreCount - bp.obsidianCostOre + oreRobotCount, 
              clayCount - bp.obsidianCostClay + clayRobotCount, 
              obsidianCount + obsidianRobotCount, 
              geodeCount + geodeRobotCount, 
              AllowedRobots.All, 
              timeRemaining - 1));
          }

          // Clay robot basically same as obsidian: if we aren't already producing enough and we can afford it and we didn't
          //  skip it
          if (clayRobotCount < bp.obsidianCostClay 
            && (allowedRobots & AllowedRobots.Clay) != 0 
            && oreCount >= bp.clayCostOre)
          {
            allowedRobots  &= ~AllowedRobots.Clay;
            bestResult = Math.Max(bestResult, Simulate(
              bp, 
              oreRobotCount, 
              clayRobotCount + 1, 
              obsidianRobotCount, 
              geodeRobotCount, 
              oreCount - bp.clayCostOre + oreRobotCount, 
              clayCount + clayRobotCount, 
              obsidianCount + obsidianRobotCount, 
              geodeCount + geodeRobotCount, 
              AllowedRobots.All, 
              timeRemaining - 1));
          }

          // Ore robot tests against the maximum ore cost of all robots (except itself because who cares?) to see if we should bother 
          //  building one
          if (oreRobotCount < maxOreCost 
            && (allowedRobots & AllowedRobots.Ore) != 0 
            && oreCount >= bp.oreCostOre)
          {
            allowedRobots  &= ~AllowedRobots.Ore;
            bestResult = Math.Max(bestResult, Simulate(
              bp, 
              oreRobotCount + 1, 
              clayRobotCount, 
              obsidianRobotCount, 
              geodeRobotCount, 
              oreCount - bp.oreCostOre + oreRobotCount, 
              clayCount + clayRobotCount, 
              obsidianCount + obsidianRobotCount, 
              geodeCount + geodeRobotCount, 
              AllowedRobots.All, 
              timeRemaining - 1));
          }

          if (allowedRobots != AllowedRobots.All)
          {
            // If we recursed into any robots, we also need to simulate "what if we chose to build nothing at all this round"
            //  in which case we disallow future building of any robots we had the option to build since there's never a 
            //  scenario in which we should wait to build a robot we can afford now, but do nothing else.
            bestResult = Math.Max(bestResult, Simulate(
              bp, 
              oreRobotCount, 
              clayRobotCount, 
              obsidianRobotCount, 
              geodeRobotCount, 
              oreCount + oreRobotCount, 
              clayCount + clayRobotCount, 
              obsidianCount + obsidianRobotCount, 
              geodeCount + geodeRobotCount, 
              allowedRobots, 
              timeRemaining - 1));
            return bestResult;
          }
        }

        // We had no choice on that round so just increment our resource counts and go again (unless we're
        //  out of time)
        oreCount += oreRobotCount;
        clayCount += clayRobotCount;
        obsidianCount += obsidianRobotCount;
        geodeCount += geodeRobotCount;
        timeRemaining--;
        if (timeRemaining == 0)
        {
          return geodeCount;
        }
      }
    }

    public static void Run(string input)
    {
      List<Blueprint> blueprints = new List<Blueprint>();
      Blueprint cur = null!;

      // Splitting this at "Each" just because the sample text had newlines before each "each" and the real one doesn't and I wanted to
      //  parse them the same
      foreach (var line in input.Split(new string[]{"\n", "Each"}, StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries)) 
      {
        if (line.StartsWith("Blueprint"))
        {
          cur = new Blueprint();
          blueprints.Add(cur);
        }
        else
        {
          var split = line.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
          switch (split[0])
          {
          case "ore":
            cur.oreCostOre = int.Parse(split[3]);
            break;
          case "clay":
            cur.clayCostOre = int.Parse(split[3]);
            break;
          case "obsidian":
            cur.obsidianCostOre = int.Parse(split[3]);
            cur.obsidianCostClay = int.Parse(split[6]);
            break;
          case "geode":
            cur.geodeCostOre = int.Parse(split[3]);
            cur.geodeCostObsidian = int.Parse(split[6]);
            break;
          default:
            Debug.Assert(false);
            break;
          }
        }
      }

      int index = 1;
      int totalP1Score = 0;
      foreach (var bp in blueprints)
      {
        int best = Simulate(bp, 1, 0, 0, 0, 0, 0, 0, 0, AllowedRobots.All, 24);
        totalP1Score += index * best;
        index++;
      }

      Console.WriteLine($"[P1] Score: {totalP1Score}");
    }
  }
}
