using System.Diagnostics;

namespace AdventOfCode2022
{
  internal static class D07
  {
    struct File
    {
      public string Name;
      public int Size;
    }

    // Obviously this whole Dir thing is wildly inefficient but for purposes of a dumb programming test, whatever
    class Dir
    {
      public Dir? Parent = null;
      public string Name = string.Empty;
      
      public int FileSize
        { get => Files.Values.Aggregate(0, (s, v) => s + v.Size); }

      public int TotalSize
        { get => Subdirs.Values.Aggregate(FileSize, (s, v) => s + v.TotalSize); }

      public Dictionary<string, Dir> Subdirs = new Dictionary<string, Dir>();
      public Dictionary<string, File> Files = new Dictionary<string, File>();

      public IEnumerable<Dir> IterRecursive()
      {
        yield return this;
        foreach (var dir in Subdirs.Values)
        {
          foreach (var sub in dir.IterRecursive())
            { yield return sub; }
        }
      }
    }

    public static void Run(string input)
    {
      Dir root = new Dir();
      root.Parent = root;
      root.Name = "";

      Dir cur = root;
      foreach (var line in input.Split('\n', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        if (line.StartsWith('$'))
        {
          // this is a command

          // we can ignore ls commands, we only need to care about cd commands
          if (line.StartsWith("$ cd "))
          {
            var target = line.Substring("$ cd ".Length).Trim();
            cur = target switch
            {
              "/" => root,
              ".." => cur.Parent!,
              _ => cur.Subdirs[target]!, // We should have already listed the directories at this point so just yolo it
            };
          }
        }
        else if (line.StartsWith("dir"))
        {
          var name = line.Substring("dir".Length).Trim();
          Debug.Assert(!cur.Subdirs.ContainsKey(name));
          cur.Subdirs.Add(name, new Dir { Parent = cur, Name = name });
        }
        else
        {
          var split = line.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
          Debug.Assert (!cur.Files.ContainsKey(split[1]));
          cur.Files.Add(split[1], new File { Size = int.Parse(split[0]), Name = split[1] });
        }
      }

      // Now we have our directory structure time to evaluate it
      // We want the sum of the sizes of every directory that contains at most "sizeCap" bytes.
      const int sizeCap = 100_000;
      int undersizedNodeSum = root.IterRecursive()
        .Where(d => d.TotalSize <= sizeCap)
        .Aggregate(0, (s, v) => s + v.TotalSize);

      Console.WriteLine($"[P1] Sum of undersized node sizes: {undersizedNodeSum}");

      const int totalDiskSize = 70_000_000;
      const int requiredDiskSize = 30_000_000;
      int availableDiskSpace = totalDiskSize - root.TotalSize;
      int requiredDeletionAmount = requiredDiskSize - availableDiskSpace;

      // Find the smallest node at least as large as the required deletion amount
      var smallestOversizeNode = root.IterRecursive()
        .Where(d => d.TotalSize >= requiredDeletionAmount)
        .Aggregate(root, (result, cur) => (cur.TotalSize < result.TotalSize) ? cur : result);

      Console.WriteLine($"[P2] Size of dir to delete: {smallestOversizeNode.TotalSize}");
    }
  }
}
