#pragma once


namespace D17
{
  using Vec = Vec2S64;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();


  enum class ComboOpcode
  {
    Lit0,
    Lit1,
    Lit2,
    Lit3,
    Reg0,
    Reg1,
    Reg2,
    Unused,
  };

  enum class Instruction
  {
    Adv, // (A >> operand) -> A
    Bxl, // (opcode ^ B) -> B
    Bst, // (opcode ^ 7) -> B
    Jnz, // if (A != 0) { instructionPointer = operand; }
    Bxc, // (B ^ C) -> B (still read operand
    Out, // (operand & 7) -> display (do not add separators)
    Bdv, // (A >> operand) -> B
    Cdv, // (A >> operand) -> C
  };


  void RunProgram(const std::vector<s32> &program, std::array<s64, 3> initialRegisters, std::vector<u8> &output)
  {
    std::array<s64, 3> registers = initialRegisters;
    s32 instPtr = 0;

    while (instPtr < s32(program.size()))
    {
      auto inst = Instruction(program[instPtr++]);

      // Get the operation as both a literal and combo opcode (the instruction will use whichever)
      s32 literalOp = program[instPtr++];
      s64 comboOp = 0;
      switch (ComboOpcode(literalOp))
      {
        case ComboOpcode::Lit0:
        case ComboOpcode::Lit1:
        case ComboOpcode::Lit2:
        case ComboOpcode::Lit3:
          comboOp = literalOp;
          break;
        case ComboOpcode::Reg0:
        case ComboOpcode::Reg1:
        case ComboOpcode::Reg2:
          comboOp = registers[literalOp - 4];
          break;
      }

      switch (inst)
      {
        case Instruction::Adv: registers[0] >>= comboOp; break;
        case Instruction::Bxl: registers[1] ^= literalOp; break;
        case Instruction::Bst: registers[1] = (comboOp & 7); break;
        case Instruction::Jnz:
          if (registers[0] != 0)
            { instPtr = s32(literalOp); }
          break;
        case Instruction::Bxc: registers[1] ^= registers[2]; break;
        case Instruction::Out: output.push_back(u8(comboOp & 7)); break;
        case Instruction::Bdv: registers[1] = (registers[0] >> comboOp); break;
        case Instruction::Cdv: registers[2] = (registers[0] >> comboOp); break;
      }
    }
  }


  // For part 2: Turns out the only instruction that can write to A is the shift instruction so count up the total
  //  shifts of A
  u32 AShift(const std::vector<s32> &program)
  {
    u32 shift = 0;

    for (u32 instPtr = 0; instPtr < program.size();)
    {
      auto inst = Instruction(program[instPtr++]);
      u32 literalOp = program[instPtr++];

      switch (inst)
      {
        case Instruction::Adv:
          ASSERT(literalOp <= 3); // This had better not be a combo op
          shift += literalOp;
          break;
      }
    }

    return shift;
  }


  void Run(const char *filePath)
  {
    std::array<s64, 3> registers;
    std::vector<s32> program;
    auto lines = ReadFileLines(filePath);
    for (s32 i = 0; i < 3; i++)
    {
      registers[i] = AsS64(Split(lines[i], " :,", KeepEmpty::No).back());
    }

    program = Split(lines[4], " :,", KeepEmpty::No)
      | std::views::drop(1)
      | std::views::transform(AsS32)
      | std::ranges::to<std::vector>();


    // For part 1 we just need to run the program and output its output, comma-separated.
    {
      std::vector<u8> output;
      RunProgram(program, registers, output);
      PrintFmt(
        "P1: {}\n",
        output
          | std::views::transform([](u8 v) { return std::format("{}", v); })
          | std::views::join_with(',')
          | std::ranges::to<std::string>());
    }


    // Part 2 is not general-purpose for any input count, it requires:
    // - We don't shift A (writing to A) a variable amount (i.e. not using a register), nor a zero amount.
    // - We don't shift A after the jump instruction (could probably handle this, but I don't)
    // - The initial values of B and C don't matter (they can be anything to start and get the same result)
    // - Maybe some other stuff?


    const s32 aShift = AShift(program);

    // Reserve memory so we aren't allocating every time we run the program.
    std::vector<u8> output;
    output.reserve(program.size() * 2);

    // Use a stack (starting with a default state of "We are after the last output and need to backsolve)
    std::vector<s64> stateStack;
    stateStack.emplace_back();
    while (!stateStack.empty())
    {
      auto A = stateStack.back();
      stateStack.pop_back();

      // Shift up by whatever our calculated shift amount per loop is.
      A <<= aShift;

      // If A's shift is < 3 then we only have as many digits as the shift frees up. But if it's 3 or more we have the
      //  full 3-bit space to play with.
      s32 maxV = std::min(7, (1 << aShift) - 1);

      // Now try every possible output digit to see which one, when we run it through the whole process, we get the
      //  last "st.outputCount" digits of the program. Iterate backwards through this because we're using a stack
      //  and we want to go depth-first on the lowest values first (Since that's what the answer asked for)
      for (s32 v = maxV; v >= 0; v--)
      {
        output.clear();
        auto newA = A | v;
        RunProgram(program, {newA, 0, 0}, output);

        // If our output doesn't match the last N elements of the program then this doesn't match (obviously)
        if (!std::ranges::equal(output, program | std::views::drop(program.size() - output.size())))
          { continue; }

        if (output.size() == program.size())
        {
          // Due to our stack and reverse iteration of v, once we find any answer it's the guaranteed smallest answer.
          PrintFmt("P2: {}\n", newA, output);
          stateStack.clear();
          break;
        }

        // We got a match so keep going from here.
        stateStack.push_back(newA);
      }
    }
  }
}