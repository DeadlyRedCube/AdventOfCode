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
      public string name = "";
      public string op1 = "";
      public string op2 = "";
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
      var equationsByOperand = new Dictionary<string, List<Op>>();

      void TestEqs(string newKnown)
      {
        if (equationsByOperand.ContainsKey(newKnown))
        {
          for (int i = 0; i < equationsByOperand[newKnown].Count;)
          {
            var op = equationsByOperand[newKnown][i];

            if (knownValues.ContainsKey(op.op1) && knownValues.ContainsKey(op.op2))
            {
              knownValues.Add(op.name, RunOp(op.op, knownValues[op.op1], knownValues[op.op2]));
              equationsByOperand[op.op1] = equationsByOperand[op.op1].Where(o => o != op).ToList();
              equationsByOperand[op.op2] = equationsByOperand[op.op2].Where(o => o != op).ToList();
              TestEqs(op.name);
            }
            else
            {
              i++;
            }
          }
        }
      };

      // We're going to store the value for "humn" here instead of adding it into known values
      long humnValue = 0;

      foreach (var line in input.Split("\n", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries))
      {
        var split 
          = line.Split(new char[] {':', ' '}, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

        if (split.Length == 2)
        {
          if (split[0] == "humn")
          {
            // Store this for now instead of throwing it into known values (so it doesn't collapse before we do part 2)
            humnValue = long.Parse(split[1]);
          }
          else
          {
            knownValues.Add(split[0], long.Parse(split[1]));
            TestEqs(split[0]);
          }
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
              if (!equationsByOperand.ContainsKey(operand))
              {
                equationsByOperand.Add(operand, new List<Op>());
              }

              equationsByOperand[operand].Add(op);
            }
          }
        }
      }

      // Take all of our equations and throw them into a list by their output name
      var equationsByName = new Dictionary<string, Op>();
      foreach (var l in equationsByOperand.Values)
      {
        foreach (var op in l)
        {
          if (!equationsByName.ContainsKey(op.name))
          {
            equationsByName.Add(op.name, op);
          }
        }
      }

      // NOTE that for this part, every step should have one known and one unknown value (thankfully)

      // Get the root and figure out which value we're tracing up from
      var rootOp = equationsByName["root"];
      Debug.Assert(knownValues.ContainsKey(rootOp.op1) || knownValues.ContainsKey(rootOp.op2));
      long p2Value = knownValues.ContainsKey(rootOp.op1) ? knownValues[rootOp.op1] : knownValues[rootOp.op2];
      var curOp = knownValues.ContainsKey(rootOp.op1) ? equationsByName[rootOp.op2] : equationsByName[rootOp.op1];

      while (true)
      {
        Debug.Assert(knownValues.ContainsKey(curOp.op1) || knownValues.ContainsKey(curOp.op2));
       
        // Compute the inverse of our operation given known/unknown value
        p2Value = (knownValues.ContainsKey(curOp.op1), curOp.op) switch
        {
          (true, '+') => p2Value - knownValues[curOp.op1],
          (false, '+') => p2Value - knownValues[curOp.op2],
          (true, '-') => -(p2Value - knownValues[curOp.op1]),
          (false, '-') => p2Value + knownValues[curOp.op2],
          (true, '*') => p2Value / knownValues[curOp.op1],
          (false, '*') => p2Value / knownValues[curOp.op2],
          (true, '/') => knownValues[curOp.op1] / p2Value,
          (false, '/') => p2Value * knownValues[curOp.op2],
          _ => throw new InvalidOperationException(),
        };

        // Figure out what the next equation in the chain is - if we've reached the "humn" entry we're done.
        string nextKey = knownValues.ContainsKey(curOp.op1) ? curOp.op2 : curOp.op1;
        if (nextKey == "humn")
          { break; }
        
        curOp = equationsByName[nextKey];
      }

      // Now that we've done the P2 work we can collapse the P1 answer
      knownValues.Add("humn", humnValue);
      TestEqs("humn");

      Console.WriteLine($"[P1] root: {knownValues["root"]}");
      Console.WriteLine($"[P2] humn: {p2Value}");
    }
  }
}
