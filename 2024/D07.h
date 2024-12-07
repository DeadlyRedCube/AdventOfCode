#pragma once


namespace D07
{
  enum class Op
  {
    Add,
    Mul,
    Comb,
  };


  bool Recurse(Op op, Op opMax, const std::vector<s64> &v, u32 begin, u32 end, s64 prevV, s64 testV)
  {
    // Apply the op from our previous value and the current one.
    s64 result;
    if (op == Op::Add)
      { result = prevV + v[begin]; }
    else if (op == Op::Mul)
      { result = prevV * v[begin]; }
    else
    {
      // Scoot the previous value over as many digits as our new number has before adding them together.
      result = prevV;
      for (auto d = v[begin]; d > 0; d /= 10)
        { result *= 10; }
      result += v[begin];
    }

    if (begin + 1 == end)
    {
      // Last one, check if it's good.
      return result == testV;
    }

    for (Op oNext = Op::Add; oNext <= opMax; oNext = Op(u32(oNext) + 1))
    {
      auto succeeded = Recurse(oNext, opMax, v, begin + 1, end, result, testV);
      if (succeeded)
        { return succeeded; }
    }

    return false;
  };


  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    std::vector<s64> operands;
    for (auto line : ReadFileLines(path))
    {
      auto toks = Split(line, " :", KeepEmpty::No);

      char *end;
      auto answer = std::strtoll(toks[0].c_str(), &end, 10);
      operands.clear();

      for (const auto &t : toks | std::views::drop(1))
        { operands.push_back(std::strtoll(t.c_str(), &end, 10)); }

      bool succeeded = false;
      for (Op op = Op::Add; op <= Op::Mul; op = Op(u32(op) + 1))
      {
        if (Recurse(op, Op::Mul, operands, 1U, u32(operands.size()), operands[0], answer))
        {
          succeeded = true;
          break;
        }
      }

      if (succeeded)
      {
        p1 += answer;
        p2 += answer;
        continue;
      }

      for (Op op = Op::Add; op <= Op::Comb; op = Op(u32(op) + 1))
      {
        if (Recurse(op, Op::Comb, operands, 1U, u32(operands.size()), operands[0], answer))
        {
          succeeded = true;
          break;
        }
      }

      if (succeeded)
        { p2 += answer; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}