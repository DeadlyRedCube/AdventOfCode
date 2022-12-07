using System;
using System.Collections.Generic;
using System.Formats.Tar;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D7
  {
    struct File
    {
      public string Name;
      public int Size;
    }

    class Dir
    {
      public Dir? Parent = null;
      public string Name = string.Empty;
      
      public int FileSize
      {
        get 
        {
          if (fileSize == 0)
          {
            foreach (var item in Files.Values) 
            {
              fileSize += item.Size;
            }
          }

          return fileSize;
        }
      }

      public int TotalSize
      {
        get
        {
          if (totalSize == 0)
          {
            totalSize = FileSize;

            foreach (var item in Subdirs.Values)
            {
              totalSize += item.TotalSize;
            }
          }

          return totalSize;
        }
      }


      int fileSize;
      int totalSize;
      public Dictionary<string, Dir> Subdirs = new Dictionary<string, Dir>();
      public Dictionary<string, File> Files = new Dictionary<string, File>();
    }


    static void FindUndersizeDirsNoExclusion(Dir dir, int sizeCap, List<Dir> outList)
    {
      if (dir.TotalSize <= sizeCap)
      {
        outList.Add(dir);
      }

      foreach (var sub in dir.Subdirs.Values)
      {
        FindUndersizeDirsNoExclusion(sub, sizeCap, outList);
      }
    }

    public static void Run(string input)
    {
      Dir root = new Dir();
      root.Parent = root;
      root.Name = "";

      Dir cur = root;
      foreach (var line in input.Split('\n', StringSplitOptions.TrimEntries))
      {
        if (line.Length == 0)
        {
          continue;
        }

        if (line.StartsWith('$'))
        {
          // this is a command

          // we can ignore ls commands, we only need to care about cd commands
          if (line.StartsWith("$ cd "))
          {
            var target = line.Substring("$ cd ".Length).Trim();

            if (target == "/")
            {
              cur = root;
            }
            else if (target == "..")
            {
              cur = cur.Parent!;
            }
            else
            {
              if (!cur.Subdirs.TryGetValue(target, out var targetDir))
              {
                targetDir = new Dir();
                targetDir.Parent = cur;
                targetDir.Name = target;
                cur.Subdirs.Add(target, targetDir);
              }

              cur = targetDir!;
            }
          }
        }
        else if (line.StartsWith("dir"))
        {
          var name = line.Substring("dir".Length).Trim();
          if (!cur.Subdirs.TryGetValue(name, out var newDir))
          {
            newDir = new Dir();
            newDir.Parent = cur;
            newDir.Name = name;
            cur.Subdirs.Add(name, newDir);
          }
        }
        else
        {
          var split = line.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
          if (!cur.Files.ContainsKey(split[1]))
          {
            var file = new File();
            file.Size = int.Parse(split[0]);
            file.Name = split[1];
            cur.Files.Add(file.Name, file);
          }
        }
      }

      // Now we have our directory structure time to evaluate it
      var undersizeNodes = new List<Dir>();
      const int sizeCap = 100_000;
      FindUndersizeDirsNoExclusion(root, sizeCap, undersizeNodes);

      int sum = undersizeNodes.Aggregate(0, (total, next) => total + next.TotalSize);
      Console.WriteLine($"Sum: {sum}");
    }
  }
}
