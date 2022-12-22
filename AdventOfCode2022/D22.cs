using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
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

    [DebuggerDisplay("{X}, {Y}")]
    struct Vec
    {
      public int X;
      public int Y;

      public Vec(int x, int y)
        { X = x; Y = y; }

      public static Vec operator+(Vec a, Vec b)
        => new Vec{X = a.X + b.X, Y = a.Y + b.Y};

      public static Vec operator-(Vec a, Vec b)
        => new Vec{X = a.X - b.X, Y = a.Y - b.Y};
    }


    [DebuggerDisplay("{X}, {Y}, {Z}")]
    struct Vec3
    {
      public int X;
      public int Y;
      public int Z;

      public Vec3(int x, int y, int z)
        { X = x; Y = y; Z = z; }

      public static Vec3 operator+(Vec3 a, Vec3 b)
        => new Vec3{X = a.X + b.X, Y = a.Y + b.Y, Z = a.Z + b.Z};

      public static Vec3 operator-(Vec3 a, Vec3 b)
        => new Vec3{X = a.X - b.X, Y = a.Y - b.Y, Z = a.Z - b.Z};

      public static Vec3 operator*(Vec3 a, int b)
        => new Vec3(a.X * b, a.Y * b, a.Z * b);

      public static Vec3 operator-(Vec3 a)
        => new Vec3(-a.X, -a.Y, -a.Z);

      public static Vec3 Cross(Vec3 a, Vec3 b)
        => new Vec3(a.Y*b.Z - a.Z*b.Y, -(a.X*b.Z - a.Z*b.X), a.X*b.Y - a.Y*b.X);

      public static int Dot(Vec3 a, Vec3 b)
        => a.X*b.X + a.Y*b.Y + a.Z*b.Z;

      public static Vec3 RotateRight(Vec3 facing, Vec3 axis)
        => Cross(facing, axis);

      public static Vec3 RotateLeft(Vec3 facing, Vec3 axis)
        => Cross(axis, facing);

      public int MinComponent { get { return Math.Min(X, Math.Min(Y, Z)); } }
      public int MaxComponent { get { return Math.Max(X, Math.Max(Y, Z)); } }
      public int ManhattanLength { get { return Math.Abs(X) + Math.Abs(Y) + Math.Abs(Z); } }

      public static bool operator==(Vec3 a, Vec3 b)
        => a.X == b.X && a.Y == b.Y && a.Z == b.Z;

      public static bool operator!=(Vec3 a, Vec3 b)
        => !(a == b);
    }

    class Face
    {
      public Vec mapOrigin;
      public Vec3 normal;
      public Vec3 uDir;
      public Vec3 vDir;
      public char[,] map;

      public Vec Project(Vec3 pos)
      {
        Vec p = new Vec(Vec3.Dot(pos, uDir), Vec3.Dot(pos, vDir));
        if (uDir.MinComponent < 0)
        {
          p.X = map.GetLength(0) - 1 + p.X;
        }

        if (vDir.MinComponent < 0)
        {
          p.Y = map.GetLength(1) - 1 + p.Y;
        }

        return p;
      }
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

      var directions = input.Split(new string[] { "\r\n", "\n"}, StringSplitOptions.TrimEntries).Last();
      var stepDirs = new Vec[] { new Vec(1, 0), new Vec(0, 1), new Vec(-1, 0), new Vec(0, -1) };

      // Part 1
      {
        Vec p = new Vec(0,0);
        Facing f = Facing.Right;

        while (map[p.X, 0] != '.')
          { p.X++; }

        char[,] stepped = new char[width, height];
        foreach (var t in Tokenize(directions))
        {
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
          }
          else
          {
            Debug.Assert(t == "L" || t == "R");
            f = (t == "L") ? SpinLeft(f) : SpinRight(f);
          }
        }

        Console.WriteLine($"[p1] Score: {(p.Y + 1) * 1000 + 4 * (p.X + 1) + (int)f}");
      }

      {
        // Part 2!

        // Figure out how to unwrap the cube
        // Find the minimum contiguous dimension 
        int cubeSize = int.MaxValue;
        for (int x = 0; x < width; x++)
        {
          int counter = 0;
          for (int y = 0; y < height; y++)
          {
            if (map[x, y] == ' ')
            {
              if (counter > 0)
              {
                cubeSize = Math.Min(cubeSize, counter);
              }
              counter = 0;
            }
            else
            {
              counter++;
            }
          }
        }

        for (int y = 0; y < height; y++)
        {
          int counter = 0;
          for (int x = 0; x < width; x++)
          {
            if (map[x, y] == ' ')
            {
              if (counter > 0)
              {
                cubeSize = Math.Min(cubeSize, counter);
              }
              counter = 0;
            }
            else
            {
              counter++;
            }
          }
        }

        Dictionary<Vec, Face> facemap = new Dictionary<Vec, Face>();
        for (int fy = 0; fy < height; fy += cubeSize)
        {
          for (int fx = 0; fx < width; fx += cubeSize)
          {
            if (map[fx, fy] == ' ')
            {
              continue;
            }

            var face = new Face();
            face.mapOrigin = new Vec(fx, fy);
            face.map = new char[cubeSize, cubeSize];
            // Oh shit a face here we go
            for (int x = 0; x < cubeSize; x++)
            {
              for (int y = 0; y < cubeSize; y++)
              {
                face.map[x,y] = map[x + fx, y + fy];
              }
            }

            facemap.Add(new Vec(fx, fy), face);
          }
        }

        // Now figure out how to connect them
        {
          Queue<Face> q = new Queue<Face>();
          var firstFace = facemap.First().Value;
          firstFace.uDir = new Vec3(1, 0, 0);
          firstFace.vDir = new Vec3(0, 1, 0);
          firstFace.normal = new Vec3(0, 0, -1);
          q.Enqueue(firstFace);
          while (q.Count > 0)
          {
            var face = q.Dequeue();

            if (facemap.TryGetValue(new Vec(face.mapOrigin.X + cubeSize, face.mapOrigin.Y), out var rightFace) 
              && rightFace.normal.ManhattanLength == 0)
            {
              // Face is off to the right
              rightFace.normal = face.uDir;
              rightFace.vDir = face.vDir;
              rightFace.uDir = -face.normal;
              q.Enqueue(rightFace);
            }

            if (facemap.TryGetValue(new Vec(face.mapOrigin.X - cubeSize, face.mapOrigin.Y), out var leftFace) 
              && leftFace.normal.ManhattanLength == 0)
            {
              // Face is off to the right
              leftFace.normal = -face.uDir;
              leftFace.vDir = face.vDir;
              leftFace.uDir = face.normal;
              q.Enqueue(leftFace);
            }

            if (facemap.TryGetValue(new Vec(face.mapOrigin.X, face.mapOrigin.Y + cubeSize), out var downFace) 
              && downFace.normal.ManhattanLength == 0)
            {
              // Face is off to the right
              downFace.normal = face.vDir;
              downFace.uDir = face.uDir;
              downFace.vDir = -face.normal;
              q.Enqueue(downFace);
            }

            if (facemap.TryGetValue(new Vec(face.mapOrigin.X, face.mapOrigin.Y - cubeSize), out var upFace) 
              && upFace.normal.ManhattanLength == 0)
            {
              // Face is off to the right
              upFace.normal = -face.vDir;
              upFace.uDir = face.uDir;
              upFace.vDir = face.normal;
              q.Enqueue(upFace);
            }
          }
        }

        Face curFace;
        Vec3 curPos;
        Vec3 facing;
        {
          Vec p = new Vec(0,0);
          while (map[p.X, 0] != '.')
            { p.X++; }

          var startCoord = new Vec(p.X / cubeSize * cubeSize, p.Y / cubeSize * cubeSize);
          curFace = facemap[startCoord];
          Debug.Assert(curFace != null);
          p -= startCoord;
          curPos = curFace.uDir * p.X + curFace.vDir * p.Y;
          facing = curFace.uDir;
        }

        // Great now we can finally start walking around this thing
        foreach (var t in Tokenize(directions))
        {
          char[,] stepped = new char[width, height];
          if (int.TryParse(t, out var stepCount))
          {
            for (int i = 0; i < stepCount; i++)
            {
              var oldPos = curPos;
              var oldFace = curFace;
              var oldFacing = facing;
              curPos += facing;
              
              var proj = curFace.Project(curPos);
              if (proj.X < 0)
              {
                // Went off the left
                curPos = oldPos;
                facing = -curFace.normal;
                curFace = facemap.Values.Where(f => f.normal == -curFace.uDir).First();
              }
              else if (proj.Y < 0)
              {
                // Went off the top
                curPos = oldPos;
                facing = -curFace.normal;
                curFace = facemap.Values.Where(f => f.normal == -curFace.vDir).First();
              }
              else if (proj.X >= cubeSize)
              {
                // Went off the right
                curPos = oldPos;
                facing = -curFace.normal;
                curFace = facemap.Values.Where(f => f.normal == curFace.uDir).First();
              }
              else if (proj.Y >= cubeSize)
              {
                // Off bottom
                curPos = oldPos;
                facing = -curFace.normal;
                curFace = facemap.Values.Where(f => f.normal == curFace.vDir).First();
              }
            
              proj = curFace.Project(curPos);
              if (curFace.map[proj.X, proj.Y] != '.')
              {
                Debug.Assert(curFace.map[proj.X, proj.Y] == '#');
                curPos = oldPos;
                facing = oldFacing;
                curFace = oldFace;
              }
              else
              {
                stepped[proj.X + curFace.mapOrigin.X, proj.Y + curFace.mapOrigin.Y] = '@';
              }
            }
          }
          else
          {
            Debug.Assert(t == "L" || t == "R");
            facing = (t == "L") ? Vec3.RotateLeft(facing, curFace.normal) : Vec3.RotateRight(facing, curFace.normal);
          }
        }

        {
          var proj = curFace.Project(curPos);
          var dir = curFace.Project(curPos + facing) - proj;
          proj += curFace.mapOrigin;
          int f =  stepDirs.ToList().IndexOf(dir);

          Console.WriteLine($"[p2] Score: {(proj.Y + 1) * 1000 + 4 * (proj.X + 1) + f}");
        }
      }
    }
  }
}
