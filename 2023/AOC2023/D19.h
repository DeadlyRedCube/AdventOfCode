#pragma once

namespace D19
{
  enum class Op
  {
    Gt,
    Lt,
    Else,
  };

  struct FlowTest
  {
    std::string cat;
    Op op;
    s64 val;
    std::string result;
  };

  struct Flow
  {
    std::string name;
    UnboundedArray<FlowTest> tests;
  };

  struct Part
  {
    s64 x;
    s64 m;
    s64 a;
    s64 s;

    s64 Val(const std::string_view &st) const
    {
      if (st == "x") { return x; }
      else if (st == "m") { return m; }
      else if (st == "a") { return a; }
      else { ASSERT(st == "s"); return s; }
    }
  };

  void Run(const char *path)
  {
    std::map<std::string, Flow> flows;
    UnboundedArray<Part> parts;
    for (auto line : ReadFileLines(path))
    {
      if (line.empty())
        { continue; }

      if (!line.starts_with('{'))
      {
        // Parse a workflow
        Flow flow;
        auto splits = Split(line, "{,}", KeepEmpty::No);
        flow.name = splits[0];
        for (auto &t : splits | std::views::drop(1))
        {
          auto &test = flow.tests.AppendNew();

          if (t.contains(':'))
          {
            // This is a < or > comparison
            auto opTarg = Split(t, ":", KeepEmpty::No);
            test.result = opTarg[1];
            test.op = opTarg[0].contains('<') ? Op::Lt : Op::Gt;
            auto operands = Split(opTarg[0], "<>", KeepEmpty::No);
            test.cat = operands[0];
            test.val = StrToNum(operands[1]);
          }
          else
          {
            // No ':' means this is an "else" statement
            test.result = t;
            test.op = Op::Else;
          }
        }

        flows[flow.name] = std::move(flow);
      }
      else
      {
        // Part!
        auto splits = Split(line, "{=,}", KeepEmpty::No);
        parts.Append(
          {
            .x = StrToNum(splits[1]),
            .m = StrToNum(splits[3]),
            .a = StrToNum(splits[5]),
            .s = StrToNum(splits[7]),
          });
      }
    }

    {
      s64 sum = 0;
      for (auto &part : parts)
      {
        std::string workflow = "in";

        while (true)
        {
          auto &flow = flows[workflow];

          for (auto &test : flow.tests)
          {
            // Find the target of this test (if we match any)
            std::string target;
            switch (test.op)
            {
            case Op::Gt:
              if (part.Val(test.cat) > test.val)
                { target = test.result; }
              break;
            case Op::Lt:
              if (part.Val(test.cat) < test.val)
                { target = test.result; }
              break;
            case Op::Else:
              target = test.result;
              break;
            }

            if (!target.empty())
            {
              // We matched this test, so we're going to go to whatever its target is.
              workflow = target;
              break;
            }
          }

          if (workflow == "A")
          {
            // This was accepted!
            sum += part.x + part.a + part.m + part.s;
            break;
          }
          else if (workflow == "R")
            { break; } // Rejected, move on to the next part.
        }
      }

      PrintFmt("Part 1: {}\n", sum);
    }

    // Part 2!
    {
      using Interval = ::Interval<s64>;

      struct PartRange // A range of part numbers
      {
        Interval x = Interval::FromFirstAndLast(1, 4000);
        Interval m = Interval::FromFirstAndLast(1, 4000);
        Interval s = Interval::FromFirstAndLast(1, 4000);
        Interval a = Interval::FromFirstAndLast(1, 4000);

        Interval &Get(const std::string_view &st)
        {
          if (st == "x") { return x; }
          else if (st == "m") { return m; }
          else if (st == "a") { return a; }
          else { ASSERT(st == "s"); return s; }
        }

        s64 Count() const
          { return x.Length() * m.Length() * a.Length() * s.Length(); }

        bool IsEmpty() const
          { return x.Length() == 0 || m.Length() == 0 || s.Length() == 0 || a.Length() == 0; }

        bool operator==(const PartRange &) const = default;
        auto operator<=>(const PartRange &) const = default;
      };


      struct S
      {
        PartRange range;
        std::string t;
      };

      s64 count = 0;
      UnboundedArray<S> stack;
      stack.Append({ {}, "in" });
      while (!stack.IsEmpty())
      {
        auto [cur, workflow] = stack[FromEnd(-1)];
        stack.RemoveAt(FromEnd(-1));
        auto &flow = flows[workflow];

        // We're going to filter our current range through the tests (adding any that are split off into the queue)
        //  until we get a result.
        for (auto &test : flow.tests)
        {
          switch (test.op)
          {
          case Op::Gt:
            if (cur.Get(test.cat).Last() > test.val)
            {
              // Make a sub-range with all the parts of the current range where the category is > the value
              auto sub = cur;
              sub.Get(test.cat) = Interval::FromFirstAndLast(test.val + 1, cur.Get(test.cat).Last());

              // See if it was accepted, rejected, or needs to be fed to a new result
              if (test.result == "A")
                { count += sub.Count(); }
              else if (test.result != "R")
                { stack.Append({sub, test.result}); }

              // Clip our current range to whatever remains.
              cur.Get(test.cat) = Interval::FromFirstAndLast(cur.Get(test.cat).Start(), test.val);
            }
            break;
          case Op::Lt:
            if (cur.Get(test.cat).First() < test.val)
            {
              // Same as above, but a subrange where the current range < value
              auto sub = cur;
              sub.Get(test.cat) = Interval::FromStartAndEnd(cur.Get(test.cat).Start(), test.val);
              if (test.result == "A")
                { count += sub.Count(); }
              else if (test.result != "R")
                { stack.Append({sub, test.result}); }

              // Clip to the remaining area
              cur.Get(test.cat) = Interval::FromFirstAndLast(test.val, cur.Get(test.cat).Last());
            }
            break;
          case Op::Else:
            // The whole range is going somewhere - if it's accepted, add its count, otherwise put it back in the stack
            //  if not rejected.
            if (test.result == "A")
              { count += cur.Count(); }
            else if (test.result != "R")
              { stack.Append({cur, test.result}); }
            break;
          }
        }
      }

      PrintFmt("Part 2: {}\n", count);
    }
  }
}
