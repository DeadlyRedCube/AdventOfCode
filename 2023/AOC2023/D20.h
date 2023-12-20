#pragma once

namespace D20
{
  enum Type
  {
    Untyped,
    FlipFlop,
    Conjunction,
    Broadcast,
  };

  struct Pulse
  {
    std::string source;
    std::string destination;
    bool val;
  };

  struct Module
  {
    std::string name;
    Type type = Type::Untyped;

    bool state = false;

    std::map<std::string, bool> inputs;

    UnboundedArray<std::string> outputNames;

    void Reset()
    {
      state = false;
      for (auto &&[n, v] : inputs)
        { v = false; }
    }

    void SendPulse(std::queue<Pulse> &pulseQueue, std::string inputName, bool pulse)
    {
      switch (type)
      {
      case Type::Untyped:
        // There are pulses that don't do anything, for funsies.
        break;

      case Type::FlipFlop:
        if (!pulse) // FlipFlop only does anything on a low pulse
        {
          // ... and what it does is flip its state and push that state out to every output.
          state = !state;
          for (auto module : outputNames)
            { pulseQueue.push({ .source = name, .destination = module, .val = state }); }
        }
        break;

      case Type::Conjunction:
        // Update the corresponding input's value
        inputs[inputName] = pulse;
        {
          // Conjunction has a low state if all of the inputs are high (otherwise it has a high state)
          state = false;
          for (auto &&[inName, val] : inputs)
          {
            if (!val)
            {
              state = true;
              break;
            }
          }

          // Send the corresponding pulse along (no matter what the pulse value is)
          for (auto module : outputNames)
            { pulseQueue.push({ .source = name, .destination = module, .val = state }); }
        }
        break;

      case Type::Broadcast:
        // Broadcast just zoops the pulse on through directly.
        for (auto module : outputNames)
          { pulseQueue.push({ .source = name, .destination = module, .val = pulse }); }
        break;
      }
    }
  };


  void Run(const char *path)
  {
    std::map<std::string, Module> modules;
    for (auto line : ReadFileLines(path))
    {
      auto splits = Split(line, " ,->", KeepEmpty::No);

      auto t = Type::Broadcast;
      if (splits[0].starts_with('&'))
      {
        splits[0] = splits[0].substr(1);
        t = Type::Conjunction;
      }
      else if (splits[0].starts_with('%'))
      {
        splits[0] = splits[0].substr(1);
        t = Type::FlipFlop;
      }

      Module &m = modules[splits[0]];
      m.type = t;
      m.name = splits[0];
      for (auto o : splits | std::views::drop(1))
        { m.outputNames.Append(o); }
    }

    for (auto &&[name, mod] : modules)
    {
      for (auto oName : mod.outputNames)
      {
        auto &oMod = modules[oName];
        oMod.inputs[mod.name] = false;
      }
    }

    std::queue<Pulse> pulseQueue;
    s64 high = 0;
    s64 low = 0;
    for (s32 i = 0; i < 1000; i++)
    {
      pulseQueue.push({ "button", "broadcaster", false });

      while (!pulseQueue.empty())
      {
        auto pulse = pulseQueue.front();
        pulseQueue.pop();

        if (pulse.val)
          { high++; }
        else
          { low++; }

        modules[pulse.destination].SendPulse(pulseQueue, pulse.source, pulse.val);
      }
    }

    PrintFmt("Part 1: {}\n", high * low);

    // For part 2, rx needs to get a low pulse. I looked at the input and only one node is outputting to it (which is
    //  what I expected), and it's a conjunction node, so we're going to find cycles in the input high pulses (to
    //  figure out where they all line up for rx)
    ASSERT(modules["rx"].inputs.size() == 1);
    auto rxSrcName = modules["rx"].inputs.begin()->first;
    auto &rcSrcMod = modules[rxSrcName];

    // Reset the modules back to factory settings.
    for (auto &mod : modules)
      { mod.second.Reset(); }

    std::map<std::string, s64> loopPrevs;
    std::map<std::string, s64> loopLengths;
    UnboundedArray<s64> loopLengthArray;
    for (s64 i = 1;; i++)
    {
      pulseQueue.push({ "button", "broadcaster", false });

      bool done = false;
      while (!pulseQueue.empty())
      {
        auto pulse = pulseQueue.front();
        pulseQueue.pop();
        modules[pulse.destination].SendPulse(pulseQueue, pulse.source, pulse.val);

        if (pulse.destination == rxSrcName && pulse.val)
        {
          if (!loopLengths.contains(pulse.source))
          {
            // We don't have the loop length for this source yet - see if we can calculate it.
            if (loopPrevs.contains(pulse.source))
            {
              // The length of the loop is how long since the last time this input went high.
              loopLengths[pulse.source] = i - loopPrevs[pulse.source];
              loopLengthArray.Append(loopLengths[pulse.source]);

              // Wasn't sure if this was going to be the case (my intuition says it has to be the case but I couldn't
              //  prove it), but it looks like the loops are perfect (which means, yay, LCM again)
              ASSERT(loopLengths[pulse.source] == loopPrevs[pulse.source]);

              if (loopLengthArray.Count() == ssz(rcSrcMod.inputs.size()))
              {
                done = true;
                break;
              }
            }
            else
              { loopPrevs[pulse.source] = i; } // Hadn't seen a high signal from this one yet, so now we have!
          }
        }
      }

      if (done)
        { break; }
    }

    PrintFmt("Part 2: {}\n", LeastCommonMultiple(loopLengthArray));
  }
}