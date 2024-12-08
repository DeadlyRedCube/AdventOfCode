#pragma once


namespace D07
{
  // Subtract r from l if it's greater (otherwise return nullopt, signaling "this operation failed")
  std::optional<s64> UnAdd(s64 l, s64 r)
    { return (l > r) ? std::optional{l - r} : std::nullopt; }

  // Divide r from l assuming l is a multiple of r (if it's not a multiple, return nullopt)
  std::optional<s64> UnMul(s64 l, s64 r)
    { return ((l % r) == 0) ? std::optional{l / r} : std::nullopt; }

  // unconcatenate the digits of R from L (and return nullopt if it couldn't be done)
  std::optional<s64> UnConcat(s64 l, s64 r)
  {
    // Calculate pow(10, Base10DigitCount(r))
    s64 tens = 1;
    for (auto d = r; d > 0; d /= 10)
      { tens *= 10; }

    // the modulo of l and the tens value should be the number on the right (think if you concatenated 123 with 45, the
    //  "tens" value would be 100 and 12345 % 100 == 45), if so, do the divide by tens to "remove" the right value's
    //  digits (int multiplication does a floor so this works great), otherwise we can't and return nullopt.
    return (l % tens == r) ? std::optional{l / tens} : std::nullopt;
  }

  using UnFunc = std::optional<s64>(*)(s64, s64);
  template <UnFunc ...funcs>
  bool UnRecurse(const std::vector<s64> &v, s64 result, u32 begin)
  {
    // If we're at the end test to see if our result matches the first item in the list, which means if we'd done the
    //  operations from left to right they would have matched.
    if (begin == 1)
      { return result == v[1]; }

    return (... || // Use a fold expression here to run the following lambda for every element of the funcs pack
      [&]
      {
        // Calculate the result of undoing the operation at this point. If it has no value we're done (op couldn't be
        //  undone in a valid way), otherwise recurse into this function using the previous value in the list.
        auto opt = funcs(result, v[begin]);
        return opt.has_value() ? UnRecurse<funcs...>(v, *opt, begin - 1) : false;
      }());
  }


  void Run(const char *path)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    for (auto line : ReadFileLines(path))
    {
      auto vals = Split(line, " :", KeepEmpty::No) | std::views::transform(AsS64) | std::ranges::to<std::vector>();

      // If the P1 test works (using only add and mul) then p2 is also valid and we don't have to do the heavier test.
      if (UnRecurse<UnAdd, UnMul>(vals, vals[0], u32(vals.size() - 1)))
      {
        p1 += vals[0];
        p2 += vals[0];
        continue;
      }

      // Try again with all of the ops enabled for P2
      if (UnRecurse<UnConcat, UnAdd, UnMul>(vals, vals[0], u32(vals.size() - 1)))
        { p2 += vals[0]; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}