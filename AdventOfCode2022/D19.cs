using System.Resources;

namespace AdventOfCode2022
{
  internal static class D19
  {
    enum Res { Ore, Clay, Obsidian, Geode }

    unsafe struct Blueprint
    {
      public Blueprint(int c0, int c1, int c2, int c3, int c4, int c5)
      { 
        this[Res.Ore, Res.Ore] = c0; 
        this[Res.Clay, Res.Ore] = c1; 
        this[Res.Obsidian, Res.Ore] = c2; 
        this[Res.Obsidian, Res.Clay] = c3; 
        this[Res.Geode, Res.Ore] = c4; 
        this[Res.Geode, Res.Obsidian] = c5; 
      }

      fixed int costs[16];
      public int this[Res robot, Res resource]
      { 
        get { return costs[(int)robot * 4 + (int)resource]; } 
        private set { costs[(int)robot * 4 + (int)resource] = value; } 
      }

      public Counts RobotCost(Res robot)
        => new Counts(this[robot, Res.Ore], this[robot, Res.Clay], this[robot, Res.Obsidian], this[robot, Res.Geode]);
    }

    unsafe struct Counts
    {
      public Counts(int ore, int clay, int obsidian, int geode)
        { c[0] = ore; c[1] = clay; c[2] = obsidian; c[3] = geode; }

      fixed int c[4];
      public int this[int r] { get => c[r]; set => c[r] = value; }
      public int this[Res r] { get => c[(int)r]; set => c[(int)r] = value; }

      public static Counts operator + (Counts a, Res res)
        { a[res]++; return a; }

      public static Counts operator + (Counts a, Counts b) => new Counts(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
      public static Counts operator - (Counts a, Counts b) => new Counts(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);

      public static bool operator >= (Counts a, Counts b) => a[0] >= b[0] && a[1] >= b[1] && a[2] >= b[2] && a[3] >= b[3];
      public static bool operator <= (Counts a, Counts b) => throw new NotImplementedException(); // Don't need this one but C# insists
    }

    // Simulate a blueprint for a given number of rounds (Starting with a single ore robot)
    static int Simulate(Blueprint bp, int rounds) 
      => Simulate(bp, new Counts(1, 0, 0, 0), new Counts(), 0, rounds);

    static int Simulate(Blueprint bp, Counts robots, Counts resources, int previouslyTriedBotFlags,  int timeRemaining)
    {
      // If we only have one last turn, don't bother building anything just add our geode values together and we're done.
      if (timeRemaining == 1)
        { return resources[Res.Geode] + robots[Res.Geode]; }

      // Calculate how many resources of each type we need from this point until the end to build one of the max cost (per resource)
      //  on every turn until we run out of turns.
      Counts mostRequired = new Counts(
        Math.Max(Math.Max(bp[Res.Ore, Res.Ore], bp[Res.Clay, Res.Ore]), Math.Max(bp[Res.Obsidian, Res.Ore], bp[Res.Geode, Res.Ore])) 
          * (timeRemaining - 1),
        bp[Res.Obsidian, Res.Clay] * (timeRemaining - 1),
        bp[Res.Geode, Res.Obsidian] * (timeRemaining - 1),
        int.MaxValue);

      int bestResult = 0;

      // For each robot type
      for (int i = 0; i < 4; i++)
      {
        var robot = (Res)i;
        var cost = bp.RobotCost(robot);

        // If we can't afford the robot, don't simulate building it (written as ! >= so I didn't have to implement a < operator)
        if (!(resources >= cost))
          { continue; }

        // If we opted not to build this robot in a past iteration we're not allowed to try again until we build something else first
        if ((previouslyTriedBotFlags & (1 << (int)robot)) != 0)
          { continue; }
                
        // If we have enough resources and generating power that we'll never run out while spending our max every turn, we don't need any more
        if (resources[robot] + robots[robot] * (timeRemaining - 1) >= mostRequired[robot])
          { continue; }
              
        // Make a note for the "build nothing" simulation we'll trigger later that it's no longer allowed to consider this robot
        //  type until we build something else first.
        previouslyTriedBotFlags |= (1 << (int)robot);

        // Simulate! (Note that we're passing a "previously tried bot flag" of 0 here, meaning that all bots are once again fair game
        bestResult = Math.Max(bestResult, Simulate(bp,  robots + robot, resources + robots - cost, 0, timeRemaining - 1));
      }

      // Finally we need to simulate the "do nothing" option: but we'll pass our previously-tried bot flags in so that any bots
      //  that were affordable this round are not considered until we build something else
      return Math.Max(bestResult, Simulate(bp, robots, resources + robots, previouslyTriedBotFlags, timeRemaining - 1));
    }

    public static void Run(string input)
    {
      List<Blueprint> blueprints = new List<Blueprint>();

      var numbers = input.Split().Where(x => int.TryParse(x, out var _)).Select(x => int.Parse(x)).ToList();
      for (int i = 0; i < numbers.Count; i += 6)
        { blueprints.Add(new Blueprint(numbers[i + 0], numbers[i + 1], numbers[i + 2], numbers[i + 3], numbers[i + 4], numbers[i + 5])); }

      int totalP1Score = blueprints.Select((b, i) => (b, i+1)).Aggregate(0, (s, v) => s + v.Item2 * Simulate(v.b, 24));
      Console.WriteLine($"[P1] Score: {totalP1Score}");

      blueprints = blueprints.Where((x, i) => i < 3).ToList();
      int totalP2Score = blueprints.Where((x, i) => i < 3).Aggregate(1, (p, b) => p * Simulate(b, 32));
      Console.WriteLine($"[P2] Score: {totalP2Score}");
    }
  }
}
