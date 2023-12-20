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
        break;

      case Type::FlipFlop:
        if (!pulse)
        {
          state = !state;

          for (auto module : outputNames)
            { pulseQueue.push({ .source = name, .destination = module, .val = state }); }
        }
        break;

      case Type::Conjunction:
        inputs[inputName] = pulse;
        {
          state = false;
          for (auto &&[inName, val] : inputs)
          {
            if (!val)
            {
              state = true;
              break;
            }
          }

          for (auto module : outputNames)
            { pulseQueue.push({ .source = name, .destination = module, .val = state }); }
        }
        break;

      case Type::Broadcast:
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

    ASSERT(modules["rx"].inputs.size() == 1);

    auto rxSrcName = modules["rx"].inputs.begin()->first;

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

        if (pulse.destination == rxSrcName && pulse.val)
        {
          if (!loopLengths.contains(pulse.source))
          {
            if (loopPrevs.contains(pulse.source))
            {
              loopLengths[pulse.source] = i - loopPrevs[pulse.source];
              loopLengthArray.Append(loopLengths[pulse.source]);
              ASSERT(loopLengths[pulse.source] == loopPrevs[pulse.source]);

              done = true;
              for (auto &&[rxSrcInputName, v] : modules[rxSrcName].inputs)
              {
                if (!loopLengths.contains(rxSrcInputName))
                {
                  done = false;
                  break;
                }
              }

              if (done)
                { break; }
            }
            else
              { loopPrevs[pulse.source] = i; }
          }
        }

        modules[pulse.destination].SendPulse(pulseQueue, pulse.source, pulse.val);
      }

      if (done)
        { break; }
    }

    PrintFmt("Part 2: {}\n", LeastCommonMultiple(loopLengthArray));
  }
}