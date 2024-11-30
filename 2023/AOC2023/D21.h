#pragma once

namespace D21
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);
    Array2D<s64> distances = { grid.Width(), grid.Height() };
    distances.Fill(std::numeric_limits<s64>::max());

    Vec2S64 sPosition;
    bool foundS = false;
    for (ssz y = 0; y < grid.Height(); y++)
    {
      for (ssz x = 0; x < grid.Height(); x++)
      {
        if (grid[{x, y}] == 'S')
        {
          sPosition = {x, y};
          foundS = true;
          break;
        }
      }

      if (foundS)
        { break; }
    }

    s64 p1Count = 0;


    const s64 p2TargetStepCount = 26501365;

    // For part 2: some counts that I'll explain later
    s64 p2CenterOnParityCount = 0;
    s64 p2CenterOffParityCount = 0;
    s64 p2OtherDiamondBothParityCount = 0;
    {
      Vec2S64 dirs[] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

      struct Step
      {
        Vec2S64 pos;
        s64 step = 0;

        bool operator < (const Step &b) const
          { return step > b.step; }
      };


      std::priority_queue<Step> stepQueue;
      stepQueue.push({ .pos = sPosition, .step = 0 });

      while (!stepQueue.empty())
      {
        auto step = stepQueue.top();
        stepQueue.pop();

        if (distances[step.pos] != std::numeric_limits<s64>::max())
          { continue; }

        distances[step.pos] = step.step;

        if (step.step <= 64)
        {
          // Count up all the step lengtsh <= 64 for part 1
          if ((step.step & 1) == 0)
            { p1Count++; }
        }

        // Now tally up the part 2 counts (more on what's going on here after this loop)
        if (step.step <= grid.Width() / 2)
        {
          // For part 2, there's a diamond pattern, count everything inside the diamond (manhattan distance of
          //  width / 2) at each even/odd parity
          if ((step.step & 1) == (p2TargetStepCount & 1))
            { p2CenterOnParityCount++; }
          else
            { p2CenterOffParityCount++; }
        }
        else
        {
          // Outside of the diamond, we care about the corners, but because every even/odd parity corner piece comes in
          //  pairs, we'll just tally up literally all the walkable space in those outer portions.
          p2OtherDiamondBothParityCount++;
        }

        for (auto dir : dirs)
        {
          // Queue up any neighboring directions that are on-grid.
          if (grid.PositionInRange(step.pos + dir) && grid[step.pos + dir] != '#')
            { stepQueue.push({ step.pos + dir, step.step + 1 }); }
        }
      }

      PrintFmt("Part 1: {}\n", p1Count);
    }

    {
      // In the actual puzzle input (although not the example, blah), the input has diamond channels cut in it:
      //
      //  AAAA.E.BBBB
      //  AAA.EEE.BBB
      //  AA.EEEEE.BB
      //  A.EEEEEEE.B
      //  .EEEEEEEEE.
      //  C.EEEEEEE.D
      //  CC.EEEEE.DD
      //  CCC.EEE.DDD
      //  CCCC.E.DDDD
      //
      // We're going to count up how many reachable odd-step squares there are in the E segment
      //  (p2CenterOnParityCount), how many reachable even-step squares there are "p2CenterOffParityCount", and then
      //  how many of both odds and evens we can touch in the remaining regions. More on that later.

      ASSERT(grid.Width() == grid.Height());                        // Square!
      ASSERT(p2TargetStepCount % grid.Width() == grid.Width() / 2); // For the fast solution to work, this must be.

      // Every grid.Width steps we add an additional ring of the diamonds we've measured to the list.
      const s64 stepExtent = p2TargetStepCount / grid.Width();

      // Since the initial step size is half the grid width (65 in the actual input), we count up all of the odd
      //  reachable squares in the E region of the above diagram at the center.
      // If we add another grid width worth of step (65 + 131 = 196) we'd be looking at these regions
      //  (extra spaces added between grid squares to make them easier to make out, "-" means "not counted", and
      //  lowercase are off-parity (i.e. count the evens instead of the odds):
      // ----.-.---- ----.e.---- ----.-.----
      // ---.---.--- ---.eee.--- ---.---.---
      // --.-----.-- --.eeeee.-- --.-----.--
      // -.-------.- -.eeeeeee.- -.-------.-
      // .---------. .eeeeeeeee. .---------.
      // ----------- eeeeeeeeeee -----------
      // .---------. .eeeeeeeee. .---------.
      // -.-------.D c.eeeeeee.d C.-------.-
      // --.-----.DD cc.eeeee.dd CC.-----.--
      // ---.---.DDD ccc.eee.ddd CCC.---.---
      // ----.-.DDDD cccc.e.dddd CCCC.-.----

      // ----.e.bbbb AAAA.E.BBBB aaaa.e.----
      // ---.eee.bbb AAA.EEE.BBB aaa.eee.---
      // --.eeeee.bb AA.EEEEE.BB aa.eeeee.--
      // -.eeeeeee.b A.EEEEEEE.B a.eeeeeee.-
      // .eeeeeeeee. .EEEEEEEEE. .eeeeeeeee.
      // eeeeeeeeeee EEEEEEEEEEE eeeeeeeeeee
      // .eeeeeeeee. .EEEEEEEEE. .eeeeeeeee.
      // -.eeeeeee.d C.EEEEEEE.D c.eeeeeee.-
      // --.eeeee.dd CC.EEEEE.DD cc.eeeee.--
      // ---.eee.ddd CCC.EEE.DDD ccc.eee.---
      // ----.e.dddd CCCC.E.DDDD cccc.e.----

      // ----.-.BBBB aaaa.e.bbbb AAAA.-.----
      // ---.---.BBB aaa.eee.bbb AAA.---.---
      // --.-----.BB aa.eeeee.bb AA.-----.--
      // -.-------.B a.eeeeeee.b A.-------.-
      // .---------. .eeeeeeeee. .---------.
      // ----------- eeeeeeeeeee -----------
      // .---------. .eeeeeeeee. .---------.
      // -.-------.- -.eeeeeee.- -.-------.-
      // --.-----.-- --.eeeee.-- --.-----.--
      // ---.---.--- ---.eee.--- ---.---.---
      // ----.-.---- ----.e.---- ----.-.----

      // in that ring we end up adding:
      //  4x "e" regions (count the evens)
      //  2x "A", "a", "B", "b", "C", "c", "D", and "d" regions (equal parts of each, of both parities)
      //
      // If we add another ring (not going to draw that diagram), we end up adding:
      //  8x "E" regions (count the odds again)
      //  4x "A", "a", "B", "b", "C", "c", "D", and "d" regions (again equal parts of each, both parities)
      //
      // Next ring:
      // 12x "e" regions (back to the evens)
      //  6x "A", "a", "B", "b", "C", "c", "D", and "d" regions (same as before, equal of all)
      //
      // So every Nth multiple of gridWidth we add, we add N*4 "E" or "e" regions (alternating) and N*2 of the other
      //  regions (which is the same as saying we add N*2 of an "AbCd" diamond and N*2 of an "aBcD" diamond.
      //
      // The closed form of each of those (taking advantage of integer division for the center counts)

      // ECount (count of "E" regions): = 1 1 9 9 25 25 49 49 ...
      s64 ECount = Square(2*(stepExtent / 2) + 1);

      // eCount (count of "e" regions): = 0 4 4 16 16 36 36 64 64 ...
      s64 eCount = Square(2*((stepExtent + 1) / 2));

      // aAbBcCdDCount (count of all other regions) = 0 2 6 12 20 30 42 ...
      s64 aAbBcCdDCount = (ECount + eCount) / 2;

      // Summing allllll that up (multiplied by the values in each square) gives us:
      s64 p2Count = ECount * p2CenterOnParityCount
        + eCount * p2CenterOffParityCount
        + aAbBcCdDCount * p2OtherDiamondBothParityCount;
      PrintFmt("Part 2: {}\n", p2Count);
    }
  }
}