export module D12;

import Util;

export namespace D12
{
  void Run(const char *path)
  {
    std::ignore = path;

    /*
    for (auto line : ReadFileLines(path))
    {
      auto vals = Split(line, " :", KeepEmpty::No);
    }
    */

    /*
    auto grid = ReadFileAsCharArray2D(path);
    for (s32 y = 0; y < grid.Height(); y++)
    {
      for (s32 x = 0; x < grid.Width(); x++)
      {
      }
    }
    */
  }
}