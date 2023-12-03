using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D08
  {
    static IEnumerable<T> XSlice<T>(this T[,] grid, int x)
    {
      for (int y = 0; y < grid.GetLength(1); y++)
        { yield return grid[x, y]; }
    }

    static IEnumerable<T> YSlice<T>(this T[,] grid, int y)
    {
      for (int x = 0; x < grid.GetLength(0); x++)
        { yield return grid[x, y]; }
    }

    // Get a list of the indices of slice indices that are visible (taller than any preceding heights)
    static IEnumerable<int> VisibleIndicesCountAlongSlice(IEnumerable<int> slice)
    {
      int maxH = -1;
      int idx = 0;
      foreach (int h in slice)
      {
        if (h > maxH)
        {
          maxH = h;
          yield return idx;
        }

        idx++;
      }
    }

    // Calculate the view score along a single slice (stopping visibility testing at the given ref height)
    static int ViewScoreAlongSlice(IEnumerable<int> slice, int refHeight)
    {
      int count = 0;
      foreach (var h in slice)
      {
        count++;
        if (h >= refHeight)
          { break; }
      }

      return count;
    }

    // Return a list of all of the slices radiating out from the given xo, yo position
    public static IEnumerable<IEnumerable<int>> SlicesFromPosition(int[,] grid, int xo, int yo)
    {
      yield return grid.XSlice(xo).Where((v, y) => y < yo).Reverse();
      yield return grid.XSlice(xo).Where((v, y) => y > yo);
      yield return grid.YSlice(yo).Where((v, x) => x < xo).Reverse();
      yield return grid.YSlice(yo).Where((v, x) => x > xo);
    }

    // Calculate the view score for a given position in the grid
    public static int ViewScore(int[,] grid, int xo, int yo)
    {
      return SlicesFromPosition(grid, xo, yo)
        .Aggregate(1, (prod, slice) => prod * ViewScoreAlongSlice(slice, grid[xo, yo]));
    }

    public static void Run(string input)
    {
      int[,] grid;
      {
        // Parse our grid
        List<List<int>> gridParse = new List<List<int>>();
        foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
        {
          var l = new List<int>();
          gridParse.Add(l);
          foreach (char digit in line)
            { l.Add(int.Parse(digit.ToString())); }
        }

        // Turn it into a 2D array
        grid = new int[gridParse[0].Count, gridParse.Count];
        for (int i = 0; i < grid.GetLength(0); i++)
        {
          for (int j = 0; j < grid.GetLength(1); j++)
            { grid[i,j] = gridParse[j][i]; }
        }
      }

      // Make a grid of booleans where true values mean "this tree was visible from an edge"
      bool[,] vis = new bool[grid.GetLength(0), grid.GetLength(1)];

      // Calculate visibility looking from the left and right for every x coordinate
      for (int x = 0; x < grid.GetLength(0); x++)
      {
        foreach (var y in VisibleIndicesCountAlongSlice(grid.XSlice(x)))
          {  vis[x, y] = true; }

        foreach (var yr in VisibleIndicesCountAlongSlice(grid.XSlice(x).Reverse()))
          {  vis[x, grid.GetLength(1) - yr - 1] = true; }
      }

      // Do the same for y
      for (int y = 0; y < grid.GetLength(0); y++)
      {
        foreach (var x in VisibleIndicesCountAlongSlice(grid.YSlice(y)))
          {  vis[x, y] = true; }

        foreach (var xr in VisibleIndicesCountAlongSlice(grid.YSlice(y).Reverse()))
          {  vis[grid.GetLength(0) - xr - 1, y] = true; }
      }

      // Now count up how many trues (visible trees) there are in the vis grid
      int visCount = 0;
      for (int x = 0; x < grid.GetLength(0); x++)
        { visCount = vis.XSlice(x).Aggregate(visCount, (count, vis) => count + (vis ? 1 : 0)); }
      Console.WriteLine($"[P1] vis count: {visCount}");

      // Now test every grid position's View Score to find the best tree's score
      int maxScore = 0;
      for (int x = 0; x < grid.GetLength(0); x++)
      {
        for (int y = 0; y < grid.GetLength(1); y++)
          { maxScore = int.Max(maxScore, ViewScore(grid, x, y)); }
      }

      Console.WriteLine($"[P2] max score: {maxScore}");
    }
  }
}
