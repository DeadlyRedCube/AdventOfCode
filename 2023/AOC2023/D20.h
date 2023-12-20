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
    s64 srcIndex;
    s64 dstIndex;
    bool val;
  };

  struct Module
  {
    std::string name;
    s64 index;
    Type type = Type::Untyped;

    bool state = false;

    struct Input
    {
      s64 modIndex;
      bool state;
    };

    UnboundedArray<Input> inputs;
    UnboundedArray<std::string> outputNames;
    UnboundedArray<s64> outputIndices;

    void Reset()
    {
      state = false;
      for (auto &input : inputs)
        { input.state = false; }
    }

    void SendPulse(std::queue<Pulse> &pulseQueue, s64 srcIndex, bool pulse)
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
          for (auto outInd : outputIndices)
            { pulseQueue.push({ .srcIndex = index, .dstIndex = outInd, .val = state }); }
        }
        break;

      case Type::Conjunction:
        // Update the corresponding input's value
        inputs[inputs.FindFirst(&Input::modIndex, srcIndex)].state = pulse;
        {
          // Conjunction has a low state if all of the inputs are high (otherwise it has a high state)
          state = !std::ranges::all_of(inputs, [](auto &&v) { return v.state; });

          // Send the corresponding pulse along (no matter what the pulse value is)
          for (auto outInd : outputIndices)
            { pulseQueue.push({ .srcIndex = index, .dstIndex = outInd, .val = state }); }
        }
        break;

      case Type::Broadcast:
        // Broadcast just zoops the pulse on through directly.
          for (auto outInd : outputIndices)
            { pulseQueue.push({ .srcIndex = index, .dstIndex = outInd, .val = pulse }); }
        break;
      }
    }
  };


  void Run(const char *path)
  {
    UnboundedArray<Module> modules;
    for (auto line : ReadFileLines(path))
    {
      auto splits = Split(line, " ,->", KeepEmpty::No);

      // Figure out the type based on the start of the first string (and then cull the type info from the string so
      //  we can use the name directly)
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

      modules.Append({ .name = splits[0], .index = modules.Count(), .type = t });
      for (auto o : splits | std::views::drop(1))
        { modules[FromEnd(-1)].outputNames.Append(o); }
    }

    for (auto &mod : modules)
    {
      for (auto oName : mod.outputNames)
      {
        ssz oInd = modules.FindFirst(&Module::name, oName);
        if (oInd < 0)
        {
          // We have an output that wasn't explicitly in our list, so just add a new untyped module.
          oInd = modules.Count();
          modules.Append({ .name = oName, .index = oInd, .type = Type::Untyped });
        }

        // Add the output's index into the module's index list, and add this node as an input to the output module
        //  (needed for conjunction nodes and useful for part 2)
        mod.outputIndices.Append(oInd);
        modules[oInd].inputs.Append({ .modIndex = mod.index, .state = false });
      }
    }

    std::queue<Pulse> pulseQueue;
    s64 highCount = 0;
    s64 lowCount = 0;

    // Cache the broadcaster's index so we don't have to look it up each time we push the button.
    s64 broadcasterIndex = modules.FindFirst(&Module::name, "broadcaster");
    for (s32 i = 0; i < 1000; i++)
    {
      pulseQueue.push({ .srcIndex = -1, .dstIndex = broadcasterIndex, .val = false });

      while (!pulseQueue.empty())
      {
        auto pulse = pulseQueue.front();
        pulseQueue.pop();

        // Record the pulse value for the part 1 answer
        if (pulse.val)
          { highCount++; }
        else
          { lowCount++; }

        modules[pulse.dstIndex].SendPulse(pulseQueue, pulse.srcIndex, pulse.val);
      }
    }

    PrintFmt("Part 1: {}\n", highCount * lowCount);

    // For part 2, rx needs to get a low pulse. I looked at the input and only one node is outputting to it (which is
    //  what I expected), and it's a conjunction node, so we're going to find cycles in the input high pulses (to
    //  figure out where they all line up for rx)
    const auto &rxMod = modules[modules.FindFirst(&Module::name, "rx")];
    ASSERT(rxMod.inputs.Count() == 1);
    ssz rxSrcIndex = rxMod.inputs[0].modIndex;
    auto &rxSrcMod = modules[rxSrcIndex];

    // Reset the modules back to factory settings.
    for (auto &mod : modules)
      { mod.Reset(); }

    UnboundedArray<s64> highModuleIndicesFound;
    UnboundedArray<s64> loopLengths;
    for (s64 i = 1;; i++)
    {
      pulseQueue.push({ -1, broadcasterIndex, false });

      bool done = false;
      while (!pulseQueue.empty())
      {
        auto pulse = pulseQueue.front();
        pulseQueue.pop();
        modules[pulse.dstIndex].SendPulse(pulseQueue, pulse.srcIndex, pulse.val);

        if (pulse.dstIndex == rxSrcIndex && pulse.val)
        {
          // Okay we got a high pulse to our output's source, record it (if we haven't already seen a high pulse from
          //  this node)
          if (highModuleIndicesFound.FindFirst(pulse.srcIndex) < 0)
          {
            highModuleIndicesFound.Append(pulse.srcIndex);
            loopLengths.Append(i);

            if (loopLengths.Count() == ssz(rxSrcMod.inputs.Count()))
            {
              // We have now seen a high pulse from every input.
              done = true;
              break;
            }
          }
        }
      }

      if (done)
        { break; }
    }

    PrintFmt("Part 2: {}\n", LeastCommonMultiple(loopLengths));
  }
}