export module D01;

import Util;

export namespace D01
{
  void Run(const char *path)
  {
    s32 curAngle = 50;
    s32 password1 = 0;
    s32 password2 = 0;
    for (auto line : ReadFileLines(path))
    {
      auto moveDelta = std::atoi(&line[1]) * ((line[0] == 'L') ? -1 : 1);

      // Was easier to reason about zero crossings if always counting in a positive direction. In the case of a
      //  negative angle, "flip" curAngle across the 100 mark so that it is the correct distance from 100 when adding
      //  the abs angle.
      auto flippedOrigin = (moveDelta < 0) ? (100 - curAngle) : curAngle;

      // The difference in hundreds digits between original (flipped) and adjusted is the number of 0 crossings.
      password2 += (flippedOrigin + std::abs(moveDelta)) / 100 - flippedOrigin / 100;

      // Add the angle to current and wrap it (properly handling large negative wraps)
      curAngle = ((curAngle + moveDelta) % 100 + 100) % 100;
      if (curAngle == 0)
        { password1++; }
    }

    PrintFmt("Part 1: {}\n", password1);
    PrintFmt("Part 2: {}\n", password2);
  }
}
