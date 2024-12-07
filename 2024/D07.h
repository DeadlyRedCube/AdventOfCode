#pragma once


namespace D07
{
  // Functions for the ops to pass to the recurse function as template parameters.
  s64 Add(s64 l, s64 r)
    { return l + r; }

  s64 Mul(s64 l, s64 r)
    { return l * r; }

  s64 Concat(s64 l, s64 r)
  {
    // Multiply l by 10 for every base-10 digit that r contains before summing them (effectively concatenating the digits)
    for (auto d = r; d > 0; d /= 10)
      { l *= 10; }
    return l + r;
  }

  using Func = s64(*)(s64, s64);
  template <Func ...funcs>
  bool Recurse(const std::vector<s64> &v, u32 begin, s64 result)
  {
    // If we're at the end test to see if our previous result matches the test value.
    if (begin == v.size())
      { return result == v[0]; }

    // Run the recurse function for every func template parameter as an || operator which will nicely early-out if
    //  an earlier one succeeds.
    return ((Recurse<funcs...>(v, begin + 1, funcs(result, v[begin]))) || ...);
  }


  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    for (auto line : ReadFileLines(path))
    {
      auto vals = Split(line, " :", KeepEmpty::No) | std::views::transform(AsS64) | std::ranges::to<std::vector>();
      if (Recurse<Add, Mul>(vals, 2, vals[1]))
      {
        // If it's good for part 1 it's good for both parts
        p1 += vals[0];
        p2 += vals[0];
        continue;
      }

      // Try again with all of the ops enabled for P2 only. I thought it might run faster to put Concat first but
      //  instead it's slightly slower (I think because it's the more complex op and frontloading it does more work)
      if (Recurse<Add, Mul, Concat>(vals, 2, vals[1]))
        { p2 += vals[0]; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}