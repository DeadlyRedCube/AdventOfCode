using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D8
  {
    public static int VisibilityFrom(int[,] grid, int xo, int yo)
    {
      int count = 0;

      int product = 1;
      for (int x = xo - 1; x >= 0; x--)
      {
        count++;
        if (grid[x,yo] >= grid[xo,yo])
        {
          break;
        }
      }

      product *= count;
      count = 0;
      for (int x = xo + 1; x < grid.GetLength(0); x++)
      {
        count++;
        if (grid[x,yo] >= grid[xo,yo])
        {
          break;
        }
      }

      product *= count;
      count = 0;
      for (int y = yo - 1; y >= 0; y--)
      {
        count++;
        if (grid[xo,y] >= grid[xo,yo])
        {
          break;
        }
      }

      product *= count;
      count = 0;
      for (int y = yo + 1; y < grid.GetLength(1); y++)
      {
        count++;
        if (grid[xo,y] >= grid[xo,yo])
        {
          break;
        }
      }

      product *= count;
      return product;
    }

    public static void Run(string input)
    {
      List<List<int>> gridParse = new List<List<int>>();
      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var l = new List<int>();
        gridParse.Add(l);
        foreach (char digit in line)
        {
          l.Add(int.Parse(digit.ToString()));
        }
      }

      int[,] grid = new int[gridParse[0].Count, gridParse.Count];

      // these are all false
      bool[,]vis = new bool[grid.GetLength(0), grid.GetLength(1)];
      for (int i = 0; i < grid.GetLength(0); i++)
      {
        for (int j = 0; j < grid.GetLength(1); j++)
        {
          grid[i,j] = gridParse[j][i];
        }
      }

      for (int i = 0; i < grid.GetLength(0); i++)
      {
        int maxFromLowSide = grid[i, 0];
        int maxFromHighSide = grid[i, grid.GetLength(1) - 1];
        vis[i, 0] = true;
        vis[i, grid.GetLength(1) - 1] = true;
        for (int j = 1; j < grid.GetLength(1); j++)
        {
          int j2 = grid.GetLength(1) - j - 1;
          if (grid[i,j] > maxFromLowSide)
          {
            maxFromLowSide = grid[i, j];
            vis[i,j] = true;
          }

          if (grid[i,j2] > maxFromHighSide)
          {
            maxFromHighSide = grid[i, j2];
            vis[i,j2] = true;
          }
        }
      }

      for (int j = 0; j < grid.GetLength(0); j++)
      {
        int maxFromLowSide = grid[0, j];
        int maxFromHighSide = grid[grid.GetLength(0) - 1, j];
        vis[0, j] = true;
        vis[grid.GetLength(0) - 1, j] = true;
        for (int i = 1; i < grid.GetLength(0); i++)
        {
          int i2 = grid.GetLength(0) - i - 1;
          if (grid[i,j] > maxFromLowSide)
          {
            maxFromLowSide = grid[i, j];
            vis[i,j] = true;
          }

          if (grid[i2,j] > maxFromHighSide)
          {
            maxFromHighSide = grid[i2, j];
            vis[i2,j] = true;
          }
        }
      }

      int visCount = 0;
      for (int i = 0; i < grid.GetLength(0); i++)
      {
        for (int j = 0; j < grid.GetLength(1); j++)
        {
          visCount += vis[i,j] ? 1 : 0;
        }
      }    
      
      Console.WriteLine($"vis count: {visCount}");

      int maxScore = 0;
      for (int i = 0; i < grid.GetLength(0); i++)
      {
        for (int j = 0; j < grid.GetLength(1); j++)
        {
          maxScore = int.Max(maxScore, VisibilityFrom(grid, i, j));
        }
      }    

      Console.WriteLine($"max score: {maxScore}");

    }
  }
}
