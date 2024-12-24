#pragma once


namespace D24
{
  void Run(const char *filePath)
  {
    s64 p1 = 0;

    auto lines = ReadFileLines(filePath);

    std::map<std::string, bool> values;

    enum class Op
    {
      And,
      Or,
      Xor,
    };

    using enum Op;

    struct Instr
    {
      std::string a;
      std::string b;
      Op op;
      std::string dst;

      auto operator<=>(const Instr &) const = default;
    };

    s64 bitCount = 0;
    std::vector<Instr> instructions;
    {
      s32 i = 0;
      for (; i < lines.size(); i++)
      {
        if (lines[i].empty())
          { break; }

        auto toks = Split(lines[i], " :", KeepEmpty::No);

        values[toks[0]] = bool(AsS32(toks[1]));
      }


      for (++i;i < lines.size(); i++)
      {
        auto toks = Split(lines[i], " ->", KeepEmpty::No);

        // Count up how many 'x' values there are, which tells us the ultimate bit count.
        if (toks[0][0] == 'x')
          { bitCount++; }

        instructions.push_back(
          {
            .a = toks[0],
            .b = toks[2],
            .op = (toks[1] == "AND")
              ? And
              : (toks[1] == "OR") ? Or : Xor,
            .dst = toks[3],
          });
      }
    }

    // Make a copy since P1 is destructive
    auto instructionsP2 = instructions;

    while (!instructions.empty())
    {
      // This is a janky way to do this but it's gonna work. Iterate through the instructions until the moment we find
      //  the first instruction we have the values for.
      for (size_t i = 0; i < instructions.size(); i++)
      {
        auto &inst = instructions[i];

        // If neither operand is in the values list then
        auto foundA = values.find(inst.a);
        if (foundA == values.end())
          { continue; }
        auto foundB = values.find(inst.b);
        if (foundB == values.end())
          { continue; }

        bool a = foundA->second;
        bool b = foundB->second;

        switch (inst.op)
        {
          case And: values[inst.dst] = a && b; break;
          case Or:  values[inst.dst] = a || b; break;
          case Xor: values[inst.dst] = a ^ b;  break;
        }

        instructions.erase(instructions.begin() + i);
      }
    }

    // Now build our binary value by finding any 'z' values, converting its number (everything after the z) into an
    //  integer, then shifting the bit's (0 or 1) value by that much.
    for (auto [k, v] : values)
    {
      if (!k.starts_with('z'))
        { continue; }
      p1 |= s64(v) << AsS32(std::string_view{k}.substr(1).data());
    }

    PrintFmt("P1: {}\n", p1);


    // Part 2:

    // For this to be a proper adder, here's what this needs to look like:
    //  x00 ^ y00 -> z00         // x00 ^ y00 outputs the first bit
    //  x00 & y00 -> CARRY[00]   // x00 & y00 is the first carry bit

    // Then, for the next bits:
    //  xn ^ yn -> XYADD[n]
    //  xn & yn -> XYCARRY[n]
    // XYADD[n] ^ CARRY[n - 1] -> Zn
    // XYADD[n] & CARRY[n - 1] -> ANDS[$n] (the dollar sign indicates it's a temp value, not numbered)
    // XYCARRY[n] | ANDS[$n] -> CARRY[n]  (CARRY[44] is just z45)

    // We're going to split the instructions up into categories to make this easier.
    std::vector<Instr> xyAdds;      // XYADD[n] above
    std::vector<Instr> xyCarries;   // XYCARRY[n] above
    std::vector<Instr> zOuts;       // These should be Zn outputs
    std::vector<Instr> ands;        // ANDS[$n] above
    std::vector<Instr> carries;     // CARRY[n] above

    for (auto &inst : instructionsP2)
    {
      // Ignore the instructions including x and y for now.
      if (inst.a[0] == 'x' || inst.a[0] == 'y' || inst.b[0] == 'x' || inst.b[0] == 'y')
      {
        switch (inst.op)
        {
          case Xor: xyAdds.push_back(inst); break;
          case And: xyCarries.push_back(inst); break;
        }
        continue;
      }

      switch (inst.op)
      {
        case And: ands.push_back(inst); break;
        case Or: carries.push_back(inst); break;
        case Xor: zOuts.push_back(inst); break;
      }
    }


    // Doing the obvious checks first, in the hopes that it's sufficient.

    std::vector<std::string> wrongs;
    for (auto &xy : xyAdds)
    {
      if (xy.a == "x00" || xy.a == "y00") // x00 ^ y00 is a special case, ignore it
        { continue; }

      // If an XYAdd desn't show up in the xor ops (or if it's a 'z' register), it's wrong
      if (xy.dst[0] == 'z'
        || !std::ranges::any_of(zOuts, [&](const auto &c) { return c.a == xy.dst || c.b == xy.dst; }))
        { wrongs.push_back(xy.dst); }
    }

    for (auto &xy : xyCarries)
    {
      if (xy.a == "x00" || xy.a == "y00") // x00 & y00 is, again, a special case, ignore it.
        { continue; }

      // if an XYCarry doesn't show up in the carries (or if it's a 'z' register), it's wrong
      if (xy.dst[0] == 'z'
        || !std::ranges::any_of(carries, [&](const auto &c) { return c.a == xy.dst || c.b == xy.dst; }))
        { wrongs.push_back(xy.dst); }
    }

    // if an zOut output isn't a 'z' register, it's wrong
    for (auto &v : zOuts)
    {
      if (v.dst[0] != 'z')
        { wrongs.push_back(v.dst); }
    }

    // if a carry output *is* a z register (excepting the final z, because that's the last carry bit), it's wrong
    auto lastZ = std::format("z{}", bitCount + 1);
    for (auto &v : carries)
    {
      if (v.dst[0] == 'z' && v.dst != lastZ)
        { wrongs.push_back(v.dst); }
    }

    // If an "and" output is a z register it's wrong.
    for (auto &v : ands)
    {
      if (v.dst[0] == 'z')
        { wrongs.push_back(v.dst); }
    }

    // Yay it *was* sufficient! This is not a general-purpose solution but whatever it's day 24

    std::ranges::sort(wrongs);
    auto p2 = wrongs | std::views::join_with(',') | std::ranges::to<std::string>();
    PrintFmt("P2: {}\n", p2);
  }
}