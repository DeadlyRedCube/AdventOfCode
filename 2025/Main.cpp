import Util;

import D01;
import D02;
import D03;
import D04;
import D05;
import D06;
import D07;
import D08;
import D09;
import D10;
import D11;
import D12;

int main()
{
  Timer t;
  //std::puts("\n-- Day 01:"); D01::Run("D01.txt");
  //std::puts("\n-- Day 02:"); D02::Run("D02.txt");
  //std::puts("\n-- Day 03:"); D03::Run("D03.txt");
  //std::puts("\n-- Day 04:"); D04::Run("D04.txt");
  //std::puts("\n-- Day 05:"); D05::Run("D05.txt");
  //std::puts("\n-- Day 06:"); D06::Run("D06.txt");
  //std::puts("\n-- Day 07:"); D07::Run("D07.txt");
  //std::puts("\n-- Day 08:"); D08::Run("D08.txt");
  std::puts("\n-- Day 09:"); D09::Run("D09.txt");
  //std::puts("\n-- Day 10:"); D10::Run("D10.txt");
  //std::puts("\n-- Day 11:"); D11::Run("D11.txt");
  //std::puts("\n-- Day 12:"); D12::Run("D12.txt");

  PrintFmt("Finished in {:.02f}ms", t.SecondsSinceLastCheckpoint() * 1000.0f);
  return 0;
}