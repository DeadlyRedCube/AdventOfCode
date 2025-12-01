import Util;

import D01;
import D02;

int main()
{
  Timer t;
  std::puts("\n-- Day 01:");
  D01::Run("D01.txt");

  PrintFmt("Finished in {:.02f}ms", t.SecondsSinceLastCheckpoint() * 1000.0f);
  return 0;
}