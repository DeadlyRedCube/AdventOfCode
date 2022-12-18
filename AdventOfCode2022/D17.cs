using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D17
  {
    static int MakeRock(byte a, byte b, byte c, byte d)
    {
      return ((int)a << 24) | ((int)b << 16) | ((int)c << 8) | ((int)d);
    }

    static readonly int[] RockShapes = 
    {
      MakeRock(
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011110),
      MakeRock(
        0b00000000,
        0b00001000,
        0b00011100,
        0b00001000),
      MakeRock(
        0b00000000,
        0b00000100,
        0b00000100,
        0b00011100),
      MakeRock(
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000),
      MakeRock(
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011000),
    };

    static byte RockExtent(int rock) 
      => (byte)(RockRow(rock, 0) | RockRow(rock, 1) | RockRow(rock, 2) | RockRow(rock, 3));

    static int ShiftRock(int rock, bool right)
    {
      int rockExtent = RockExtent(rock);
      if (right && (rockExtent & 1) == 0)
      {
        return rock >> 1;
      }
      else if (!right && (rockExtent & 0b01000000) == 0)
      {
        return rock << 1;
      }
      else
      {
        return rock;
      }
    }

    static byte RockRow(int rock, int row)
      => (byte)((rock >> ((3 - row) * 8)) & 0xFF);

    static void DrawChamber(IEnumerable<byte> chamber)
    {
      foreach(var b in chamber)
      {
        Console.Write("|");
        for (int i = 7; i >= 0; i--)
        {
          Console.Write((b & (1 << i)) != 0 ? "#" : ".");
        }

        Console.WriteLine("|");
      }

      Console.WriteLine("\n");
    }


    static int NormalizeChamberHeight(List<byte> chamber)
    {
      // All of our pieces are 4 units tall and we want 3 units of space between the bottom of
      //  a piece and the top of the current pile.
      while (chamber.Count < 3 + 4 || chamber[2 + 4] != 0)
        { chamber.Insert(0, 0b00000000); }

      return chamber.Count - 8;
    }

    public static void RunP1(string input)
      => Run(input, 2022);

    public static void RunP2(string input)
      => Run(input, 1_000_000_000_000);

    public static void Run(string input, long totalRockCount)
    {
      input = input.Trim();
      int jetIndex = 0;
      int rockShapeIndex = 0;

      // Create our chamber with a single entry (the floor, across all 7 tiles (bits))
      List<byte> chamber = new List<byte>(new byte[] { 0b01111111 });

      // When searching for our repeat counts, we use the set to look for known jet index values,
      //  and then set repeatJetIndex once we get a duplicate.
      int? repeatJetIndex = null;
      var jetIndexSet = new HashSet<int>();

      // Bottom/repeat/modulo are the three sections of our measurement - bottom is
      //  "how many rocks/how much height before our first measured repetition section"
      // repeat is "how many rocks/how much height in the repeated section"
      // modulo is "how much height after the repeated section
      int bottomHeight = 0;
      long bottomRockCount = 0;

      int repeatHeight = 0;
      long repeatRockCount = 0;

      int moduloHeight = 0;

      // This is our target - it defaults to our total but we'll calculate a much smaller version once
      //  we've found the repetition frequency of the system.
      long targetRockFallIndex = totalRockCount;

      for (long rockFallIndex = 0; rockFallIndex < targetRockFallIndex; rockFallIndex++)
      {
        NormalizeChamberHeight(chamber);

        int shape = RockShapes[rockShapeIndex];
        int y = 0;

        bool jetWrapped = false;
        while (true)
        {
          bool shiftRight = (input[jetIndex] == '>');
          jetIndex = (jetIndex + 1) % input.Length;
          if (jetIndex == 0)
          {
            jetWrapped = true;
          }

          // Shift side to side
          int shiftedShape = ShiftRock(shape, shiftRight);

          bool overlap = new int[]{0,1,2,3}
            .Aggregate(0, (overlapBits, row) => overlapBits | (RockRow(shiftedShape, row) & chamber[y + row])) 
            != 0;

          if (!overlap)
            { shape = shiftedShape; }

          // Now drop
          overlap = new int[]{0,1,2,3}
            .Aggregate(0, (overlapBits, row) => overlapBits | (RockRow(shape, row) & chamber[y + 1 + row])) 
            != 0;

          if (overlap)
          {
            // Write our rock into the array
            for (int r = 0; r < 4; r++)
            { 
              var row = RockRow(shape, r);
              chamber[y + r] |= row; 
            }

            break;
          }

          y++;
        }

        rockShapeIndex = (rockShapeIndex + 1) % RockShapes.Length;

        if (jetWrapped && rockShapeIndex == 0)
        {
          if (repeatJetIndex == null)
          {
            // We haven't found our repeat index yet so test to see if this is it or we need to keep searching
            if (jetIndexSet.Contains(jetIndex))
            {
              // Found it! We now know how tall the bottom section of our pattern is (i.e. the bit before
              //  the repeated pattern starts)
              bottomHeight = NormalizeChamberHeight(chamber);
              bottomRockCount = rockFallIndex;
              repeatJetIndex = jetIndex;
            }
            else
            {
              // We haven't seen this index yet so add it into the set.
              jetIndexSet.Add(jetIndex);
            }
          }
          else if(jetIndex == repeatJetIndex)
          {
            // We've now measured a full repeat of our jet indices, and we know how much height/rock count accumulates
            //  each time. Now we can use some modulo math to figure out how many rocks off from the repeat our 
            //  final rock count will be (so we can measure how much height is in the interrupted final section)
            repeatHeight = NormalizeChamberHeight(chamber) - bottomHeight;
            repeatRockCount = rockFallIndex - bottomRockCount;

            targetRockFallIndex = rockFallIndex + (totalRockCount - bottomRockCount) % repeatRockCount;
          }
        }
      }

      moduloHeight = NormalizeChamberHeight(chamber) - bottomHeight - repeatHeight;

      var repeatCount = (repeatRockCount > 0) ? ((totalRockCount - bottomRockCount) / repeatRockCount) : 0;

      long height = bottomHeight + repeatCount * repeatHeight + moduloHeight;
      Console.WriteLine($"Height for {totalRockCount}: {height}");
    }
  }
}
