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

    s64 Val(const std::string_view &st)
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
        // First section
        Flow flow;
        auto splits = Split(line, "{,}", KeepEmpty::No);
        flow.name = splits[0];
        for (auto &t : splits | std::views::drop(1))
        {
          auto &test = flow.tests.AppendNew();

          if (t.contains(':'))
          {
            auto opTarg = Split(t, ":", KeepEmpty::No);
            test.result = opTarg[1];
            test.op = opTarg[0].contains('<') ? Op::Lt : Op::Gt;
            auto operands = Split(opTarg[0], "<>", KeepEmpty::No);
            test.cat = operands[0];
            test.val = StrToNum(operands[1]);
          }
          else
          {
            test.result = t;
            test.op = Op::Else;
          }
        }

        flows[flow.name] = std::move(flow);
      }
      else
      {
        // Second section
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


    s64 sum = 0;
    for (auto &part : parts)
    {
      std::string workflow = "in";

      while (true)
      {
        auto &flow = flows[workflow];

        for (auto &test : flow.tests)
        {
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
            workflow = target;
            break;
          }
        }

        if (workflow == "A")
        {
          sum += part.x + part.a + part.m + part.s;
          break;
        }
        else if (workflow == "R")
          { break; }
      }
    }

    PrintFmt("Part 1: {}\n", sum);
  }
}
