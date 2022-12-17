using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D16
  {
    [DebuggerDisplay("{name}: {flowRate}")]
    class Valve
    {
      public string name;
      public int flowRate;

      public bool isOpen = false;
      public List<Valve> targets = new List<Valve>();
      public Dictionary<string, int> distances = new Dictionary<string, int>();
    }


    static Dictionary<string, Valve> valves = new Dictionary<string, Valve>();

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
      const int timeLimit = 30;
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

      long bestCost = BestCost(valves["AA"], 0, long.MaxValue, closedUsefulValves, timeLimit);

      long totalPressure = closedUsefulValves.Aggregate(0, (c, v) => (c + v.flowRate)) * timeLimit;

      Console.WriteLine($"Solution? {totalPressure - bestCost}");
    }
  }
}
