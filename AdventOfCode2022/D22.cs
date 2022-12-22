using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D22
  {
    enum Facing
    {
      Right,
      Down,
      Left,
      Up,
    }

    static Facing SpinLeft(Facing facing)
    {
      return facing switch
      {
        Facing.Right => Facing.Up,
        Facing.Down => Facing.Right,
        Facing.Left => Facing.Down,
        _ => Facing.Left,
      };
    }

    static Facing SpinRight(Facing facing)
    {
      return facing switch
      {
        Facing.Right => Facing.Down,
        Facing.Down => Facing.Left,
        Facing.Left => Facing.Up,
        _ => Facing.Right,
      };
    }

    static IEnumerable<string> Tokenize(string input)
    {
      int i = 0;
      StringBuilder b = new StringBuilder();
      while (i < input.Length)
      {
        b.Clear();
        while (i < input.Length && char.IsDigit(input[i]))
        {
          b.Append(input[i]);
          i++;
        }

        if (b.Length > 0)
        {
          yield return b.ToString();
        }

        while (i < input.Length && !char.IsDigit(input[i]))
        {
          yield return input[i].ToString();
          i++;
        }
      }
    }

    struct Vec
    {
      public int X;
      public int Y;

      public Vec(int x, int y)
        { X = x; Y = y; }

      public static Vec operator+(Vec a, Vec b)
        => new Vec{X = a.X + b.X, Y = a.Y + b.Y};
    }


    public static void Run(string input)
    {
      int width = 0;
      int height = 0;
      foreach (var line in input.Split(new string[] { "\r\n", "\n"}, StringSplitOptions.None))
      {
        if (line.Length == 0)
        {
          break;
        }

        width = Math.Max(line.Length, width);
        height++;
      }

      char[,] map = new char[width, height];
      {
        int y = 0;
        foreach (var line in input.Split(new string[] { "\r\n", "\n"}, StringSplitOptions.None))
        {
          if (line.Length == 0)
          {
            break;
          }

          for (int x = 0; x < line.Length; x++)
          {
            map[x,y] = line[x];
          }

          for (int x = line.Length; x < width; x++)
          {
            map[x,y] = ' ';
          }

          y++;
        }
      }

      Vec p = new Vec(0,0);
      Facing f = Facing.Right;

      while (map[p.X, 0] != '.')
        { p.X++; }

      var stepDirs = new Vec[] { new Vec(1, 0), new Vec(0, 1), new Vec(-1, 0), new Vec(0, -1) };
      var directions = input.Split(new string[] { "\r\n", "\n"}, StringSplitOptions.TrimEntries).Last();

      char[,] stepped = new char[width, height];
      foreach (var t in Tokenize(directions))
      {
        //Console.WriteLine($"{t}");
        if (int.TryParse(t, out var stepCount))
        {
          var d = stepDirs[(int)f];
          for (int i = 0; i < stepCount; i++)
          {
            var old = p;
            p += d;
            if (d.X > 0)
            {
              if (p.X >= width || map[p.X, p.Y] == ' ')
              {
                while (p.X > 0 && map[p.X - 1, p.Y] != ' ')
                {
                  p.X--; 
                }
              }
            }
            else if (d.X < 0)
            {
              if (p.X < 0 || map[p.X, p.Y] == ' ')
              {
                while (p.X < width - 1 && map[p.X + 1, p.Y] != ' ')
                {
                  p.X++; 
                }
              }
            }
            else if (d.Y > 0)
            {
              if (p.Y >= height || map[p.X, p.Y] == ' ')
              {
                while (p.Y > 0 && map[p.X, p.Y - 1] != ' ')
                {
                  p.Y--; 
                }
              }
            }
            else
            {
              if (p.Y < 0 || map[p.X, p.Y] == ' ')
              {
                while (p.Y < height - 1 && map[p.X, p.Y + 1] != ' ')
                {
                  p.Y++; 
                }
              }
            }

            if (map[p.X, p.Y] == '#')
            {
              p = old;
            }

            stepped[p.X, p.Y] = '@';
          }

          //Console.WriteLine($"  New X: {p.X}, Y: {p.Y}");
        }
        else
        {
          Debug.Assert(t == "L" || t == "R");
          f = (t == "L") ? SpinLeft(f) : SpinRight(f);
          //Console.WriteLine($"New Facing: {f}");
        }

        #if false
        for (int y = 0; y < height; y++)
        {
          for (int x = 0; x < width; x++)
          {
            if (p.X == x && p.Y == y)
            {
              Console.Write('@');
            }
            else if (stepped[x,y] != 0)
            {
              Debug.Assert(map[x,y] == '.');
              Console.Write(map[x,y] == '.' ? '+' : 'X');
            }
            else
            {
              Console.Write(map[x,y]);
            }
          }
          Console.WriteLine();
        }

        Console.WriteLine($"X: {p.X}, Y: {p.Y}, facing: {f}, move: {t}\n\n");
        #endif
      }

      Console.WriteLine($"Score: {(p.Y + 1) * 1000 + 4 * (p.X + 1) + (int)f}");
    }
  }
}
