using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdventOfCode2022
{
  internal static class D21
  {
    [DebuggerDisplay("{name} = {op1} {op} {op2}")]
    class Op
    {
      public string name;
      public string op1;
      public string op2;
      public char op;
    }

    public static long RunOp(char op, long v1, long v2)
    {
      return op switch
      {
        '+' => v1 + v2,
        '-' => v1 - v2,
        '*' => v1 * v2,
        '/' => v1 / v2,
        _ => throw new NotSupportedException()
      };
    }

    public static void Run(string input)
    {
      var knownValues = new Dictionary<string, long>();
      var equations = new Dictionary<string, List<Op>>();

      void TestEqs(string newKnown)
      {
        if (equations.ContainsKey(newKnown))
        {
          for (int i = 0; i < equations[newKnown].Count;)
          {
            var op = equations[newKnown][i];

            if (knownValues.ContainsKey(op.op1) && knownValues.ContainsKey(op.op2))
            {
              knownValues.Add(op.name, RunOp(op.op, knownValues[op.op1], knownValues[op.op2]));
              equations[op.op1] = equations[op.op1].Where(o => o != op).ToList();
              equations[op.op2] = equations[op.op2].Where(o => o != op).ToList();
              //if (op.name == "root")
              {
                Console.WriteLine($"{op.name}: {knownValues[op.name]}");
              }
              TestEqs(op.name);
            }
            else
            {
              i++;
            }
          }
        }
      };

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var split 
          = line.Split(new char[] {':', ' '}, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

        if (split.Length == 2)
        {
          knownValues.Add(split[0], long.Parse(split[1]));
          TestEqs(split[0]);
        }
        else
        {
          Debug.Assert(split.Length == 4);
          var op = new Op();
          op.name = split[0];
          op.op1 = split[1];
          op.op2 = split[3];
          op.op = split[2][0];
          Debug.Assert(split[2].Length == 1);
          if (knownValues.ContainsKey(op.op1) && knownValues.ContainsKey(op.op2))
          {
            knownValues.Add(split[0], RunOp(op.op, knownValues[op.op1], knownValues[op.op2]));
            TestEqs(split[0]);
          }
          else
          {
            foreach (var operand in new string[] { op.op1, op.op2 })
            {
              if (!equations.ContainsKey(operand))
              {
                equations.Add(operand, new List<Op>());
              }

              equations[operand].Add(op);
            }
          }
        }
      }
    }
  }
}
