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
#include "D06.h"

int main()
{
  Timer t;

  //D01::Run("D01.txt");
  //D02::Run("D02.txt");
  //D03::Run("D03.txt");
  //D04::Run("D04.txt");
  //D05::Run("D05.txt");
  D06::Run("D06.txt");

  auto time = t.SecondsSinceLastCheckpoint();
  PrintFmt("Finished in {:.02} seconds", time);
  return 0;
}
