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


//#include "D01.h"
//#include "D02.h"
//#include "D03.h"
//#include "D04.h"
//#include "D05.h"
//#include "D06.h"
//#include "D07.h"
//#include "D08.h"
//#include "D09.h"
//#include "D10.h"
#include "D11.h"

int main()
{
  Timer t;

  //D01::Run("D01.txt");
  //D02::Run("D02.txt");
  //D03::Run("D03.txt");
  //D04::Run("D04.txt");
  //D05::Run("D05.txt");
  //D06::Run("D06.txt");
  //D07::Run("D07.txt");
  //D08::Run("D08.txt");
  //D09::Run("D09.txt");
  //D10::Run("D10.txt");
  D11::Run("D11.txt");

  auto time = t.SecondsSinceLastCheckpoint();
  PrintFmt("Finished in {:.02} seconds", time);
  return 0;
}
