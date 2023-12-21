#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <format>
#include <fstream>
#include <map>
#include <ranges>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

#include "Util/Core.h"
#include "Helpers.h"


#if 0
#include "D01.h"
#include "D02.h"
#include "D03.h"
#include "D04.h"
#include "D05.h"
#include "D06.h"
#include "D07.h"
#include "D08.h"
#include "D09.h"
#include "D10.h"
#include "D11.h"
#include "D12.h"
#include "D13.h"
#include "D14.h"
#include "D15.h"
#include "D16.h"
#include "D17.h"
#include "D18.h"
#include "D19.h"
#endif
#include "D20.h"

int main()
{
  Timer t;

  #if 0
  std::puts("\n-- Day 01:");
  D01::Run("D01.txt");
  std::puts("\n-- Day 02:");
  D02::Run("D02.txt");
  std::puts("\n-- Day 03:");
  D03::Run("D03.txt");
  std::puts("\n-- Day 04:");
  D04::Run("D04.txt");
  std::puts("\n-- Day 05:");
  D05::Run("D05.txt");
  std::puts("\n-- Day 06:");
  D06::Run("D06.txt");
  std::puts("\n-- Day 07:");
  D07::Run("D07.txt");
  std::puts("\n-- Day 08:");
  D08::Run("D08.txt");
  std::puts("\n-- Day 09:");
  D09::Run("D09.txt");
  std::puts("\n-- Day 10:");
  D10::Run("D10.txt");
  std::puts("\n-- Day 11:");
  D11::Run("D11.txt");
  std::puts("\n-- Day 12:");
  D12::Run("D12.txt");
  std::puts("\n-- Day 13:");
  D13::Run("D13.txt");
  std::puts("\n-- Day 14:");
  D14::Run("D14.txt");
  std::puts("\n-- Day 15:");
  D15::Run("D15.txt");
  std::puts("\n-- Day 16:");
  D16::Run("D16.txt");
  std::puts("\n-- Day 17:");
  D17::Run("D17.txt");
  std::puts("\n-- Day 18:");
  D18::Run("D18.txt");
  std::puts("\n-- Day 19:");
  D19::Run("D19.txt");
  #endif
  std::puts("\n-- Day 20:");
  D20::Run("D20.txt");

  auto time = t.SecondsSinceLastCheckpoint();
  PrintFmt("Finished in {:.02} seconds", time);
  return 0;
}
