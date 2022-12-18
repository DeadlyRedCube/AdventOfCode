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

    public static void Run(string input)
    {
      input = input.Trim();
      int jetIndex = 0;
      int rockShapeIndex = 0;

      List<byte> chamber = new List<byte>();
      chamber.Add(0b01111111);
      
      const int rockCount = 2022;
      for (int rockFallIndex = 0; rockFallIndex < rockCount; rockFallIndex++)
      {
        // Our rocks are all 4 units tall and we want there to be 3 spaces of
        //  space between bottom of rock start and where the drop starts
        while (chamber.Count < 3 + 4 || chamber[2 + 4] != 0)
        {
          chamber.Insert(0, 0b00000000);
        }

        int shape = RockShapes[rockShapeIndex];
        int y = 0;

        while (true)
        {
          bool shiftRight = (input[jetIndex] == '>');
          jetIndex = (jetIndex + 1) % input.Length;

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

            //DrawChamber(chamber);
            break;
          }

          y++;
        }

        rockShapeIndex = (rockShapeIndex + 1) % RockShapes.Length;
      }

      int height = chamber.Count - 1;
      for (int i = 0; chamber[i] == 0; i++)
      {
        height--;
      }

      Console.WriteLine($"Height: {height}");
    }
  }
}
