using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Net.Mime.MediaTypeNames;

namespace AdventOfCode2022
{
  internal static class D16
  {
    [DebuggerDisplay("{name}: {flowRate}")]
    class Valve
    {
      public string name = "";
      public int flowRate;

      public bool isOpen = false;
      public List<Valve> targets = new List<Valve>();
      public Dictionary<string, int> distances = new Dictionary<string, int>();
    }


    static Dictionary<string, Valve> valves = new Dictionary<string, Valve>();


    static Valve StepTowards(Valve cur, Valve target, int stepCount)
    {
      for (int i = 0; i < stepCount; i++)
      {
        var next = cur;
        foreach (var dest in cur.targets)
        {
          if (dest.distances[target.name] < next.distances[target.name])
            { next = dest; }
        }

        cur = next;
      }

      return cur;
    }

    // Okay we're going to assume that B is always at their target (and has opened it) during this call for simplicity
    static long BestCost2(
      Valve curA, 
      Valve curB, 
      Valve targetA,
      long startCost, 
      long bestSoFar, 
      List<Valve> closedValves,
      int timeRemaining)
    {
      if (closedValves.Count == 0)
      {
        // Nothing more to do we're done
        return bestSoFar;
      }

      if (curA == targetA && !closedValves.Contains(targetA))
      {
        // Both are at their target positions and have opened their valves so we'll try each A starting point
        //  This could have been more efficient by eliminating any B starting points before this as a starting
        //  point for this round but it finished processing, so me.
        foreach (var t in closedValves)
        {
          var c = BestCost2(curA, curB, t, startCost, bestSoFar, closedValves, timeRemaining);
          if (c < bestSoFar)
          {
            bestSoFar = c;
          }
        }

        return bestSoFar;
      }

      // Every step we take without anything happening costs us this much flow rate savings
      int minuteCost = closedValves.Aggregate(0, (c, v) => (c + v.flowRate));
      
      // How many steps before A is open
      int aSteps = curA.distances[targetA.name] + 1;

      if (closedValves.Count == 1)
      {
        // There's nothing for B to do so we just run A until completion
        long cost = startCost + Math.Min(timeRemaining, aSteps) * minuteCost;
        if (cost < bestSoFar)
        {
          bestSoFar = cost;
        }

        return bestSoFar;
      }

      foreach (var targetB in closedValves.Where(v => v != targetA))
      {
        int bSteps = curB.distances[targetB.name] + 1;

        int stepCount = Math.Min(aSteps, bSteps);
        if (stepCount > timeRemaining)
        {
          // We won't finish either before time runs out so just cost us whatever pressure is left
          long cost = startCost + timeRemaining * minuteCost;
          if (cost < bestSoFar)
          {
            bestSoFar = cost;
          }

          continue;
        }

        long curCost = startCost + Math.Min(aSteps, bSteps) * minuteCost;
        if (curCost >= bestSoFar)
        {
          // Early out if we're already worse than our best case so far
          continue;
        }

        // Step A and B towards their targets
        var newA = StepTowards(curA, targetA, stepCount);
        var newB = StepTowards(curB, targetB, stepCount);

        // Recurse into our cost function, swapping A and B if A finished first (since we're assuming
        //  B is the finished one)
        // Also remove targetA and/or targetB from closedValves depending on which one (or both) get
        //  closed at the end of this step
        curCost = BestCost2(
          (aSteps < bSteps) ? newB : newA,
          (aSteps < bSteps) ? newA : newB,
          (aSteps < bSteps) ? targetB : targetA,
          curCost,
          bestSoFar,
          closedValves.Where(
            v => (aSteps > bSteps || v != targetA) && (bSteps > aSteps || v != targetB)).ToList(),
          timeRemaining - stepCount);

        if (curCost < bestSoFar)
        {
          bestSoFar = curCost;
        }
      }

      return bestSoFar;
    }

    static long BestCost(Valve curValve, long startCost, long bestSoFar, List<Valve> closedValves, int timeRemaining)
    {
      // every minute costs us the total flow rate of all of the closed valves
      int minuteCost = closedValves.Aggregate(0, (c, v) => (c + v.flowRate));
      Debug.Assert(!closedValves.Contains(curValve));
      
      foreach (var closedValve in closedValves)
      {
        // we're going to try to go to this one
        int minutes = curValve.distances[closedValve.name] + 1;

        // We won't reach it, ignore it.
        if (timeRemaining < minutes)
        { 
          long cost = startCost + (timeRemaining * minuteCost);
          if (cost < bestSoFar)
          {
            bestSoFar = cost;
          }

          continue;
        }

        long curCost = startCost + minutes * minuteCost;
        if (curCost > bestSoFar)
          { continue; }

        if (closedValves.Count > 1)
        {
          curCost = BestCost(
            closedValve, 
            curCost, 
            bestSoFar, 
            closedValves.Where(v => v != closedValve).ToList(), 
            timeRemaining - minutes);
        }

        if (curCost < bestSoFar)
        {
          bestSoFar = curCost;
        }
      }

      return bestSoFar;
    }

    public static void Run(string input)
    {
      foreach (var line in input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
      {
        var toks = line.Split(new char[]{' ', '=', ';', ','}, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        var name = toks[1];
        var flow = int.Parse(toks[5]);

        valves.Add(name, new Valve() { name=name, flowRate = flow });
      }

      foreach (var line in input.Split("\n", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
      {
        var toks = line.Split(new char[]{' ', '=', ';', ','}, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
        var name = toks[1];
        var v = valves[name];

        for (int i = 10; i < toks.Length; i++)
        {
          v.targets.Add(valves[toks[i]]);
        }

        foreach(var target in valves.Keys)
        v.distances.Add(target, int.MaxValue);
      }

      // Find the distances (steps) from each valve to each other valve
      foreach (var sourceName in valves.Keys)
      {
        Valve src = valves[sourceName];
        src.distances[sourceName] = 0;

        var q = new Queue<Valve>();
        q.Enqueue(src);
        while (q.Count > 0)
        {
          var curValve = q.Dequeue();

          int distance = src.distances[curValve.name];
          foreach (var targetValve in curValve.targets)
          {
            if (distance + 1 < src.distances[targetValve.name])
            {
              src.distances[targetValve.name] = distance + 1;
              q.Enqueue(targetValve);
            }
          }
        }
      }

      var closedUsefulValves = valves.Values.Where(v => v.flowRate > 0).ToList();
      closedUsefulValves.Sort((a, b) => b.flowRate.CompareTo(a.flowRate));

      const int p1TimeLimit = 30;
      long bestCost = BestCost(valves["AA"], 0, long.MaxValue, closedUsefulValves, p1TimeLimit);
      long totalPressure = closedUsefulValves.Aggregate(0, (c, v) => (c + v.flowRate)) * p1TimeLimit;
      Console.WriteLine($"[P1] Single-person best release: {totalPressure - bestCost}");

      const int p2TimeLimit = 26;
      totalPressure = closedUsefulValves.Aggregate(0, (c, v) => (c + v.flowRate)) * p2TimeLimit;
      bestCost = BestCost2(valves["AA"], valves["AA"], valves["AA"], 0, long.MaxValue, closedUsefulValves, p2TimeLimit);

      Console.WriteLine($"[P2] Two-agent best release: {totalPressure - bestCost}");
    }
  }
}
