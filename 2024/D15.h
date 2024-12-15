#pragma once


namespace D15
{
  using Vec = Vec2S32;

  void Run(const char *path)
  {
    s32 p1 = 0;
    s32 p2 = 0;

    auto file = ReadEntireFile(path);
    // ReplaceAll(&file, "\r\n", "\n"); This isn't necessary since apparently I managed to get the line endings right.

    // Find the first blank line's index, that's where the grid stops
    auto idx = size_t(uintptr_t(strstr(file.c_str(), "\n\n")) - uintptr_t(file.c_str()));

    // Now convert the top part of the string into a 2D array.
    auto gridLines = Split(file.substr(size_t(0), size_t(idx)), "\r\n", KeepEmpty::No);
    auto gridP1 = MakeCharArray2D(gridLines);

    // The bottom half gets turned into a series of vector directions (the movements)
    auto moves = file.substr(size_t(idx + 1))
      | std::views::filter([](char c) { return c != '\n'; })
      | std::views::transform(
        [](char c)
        {
          switch (c)
          {
          case '^': return Vec{ 0, -1 };
          case 'v': return Vec{ 0, 1 };
          case '<': return Vec{ -1, 0};
          case '>': return Vec{ 1, 0};
          default: return Vec{};
          }
        })
      | std::ranges::to<std::vector>();

    // Find the robotPos
    Vec initialRobotPos;
    for (auto c : gridP1.IterCoords<s32>())
    {
      if (gridP1[c] == '@')
      {
        initialRobotPos = c;
        gridP1[c] = '.';
        break;
      }
    }

    auto robotPos = initialRobotPos;

    // Now we also want to set up the part 2 double-wide grid (before we run P1 - ask me how I know)
    constexpr Vec DoubleX { 2, 1 };
    constexpr Vec AddX { 1, 0 };
    constexpr Vec AddY { 0, 1 };
    auto gridP2 = Array2D<char> { gridP1.Width() * 2, gridP1.Height() };
    for (auto c : gridP1.IterCoords<s32>())
    {
      char c1 = gridP1[c];
      char c2 = c1;
      if (c1 == 'O')
      {
        c1 = '[';
        c2 = ']';
      }

      gridP2[c * DoubleX] = c1;
      gridP2[c * DoubleX + AddX] = c2;
    }

    // Now run part 1, pushing boxes around.
    for (auto dir : moves)
    {
      auto n = robotPos + dir;
      if (gridP1[n] == '#')
        { continue; } // If there's a wall we can't do anything.

      if (gridP1[n] != 'O')
      {
        // It's an open space, so move freely
        robotPos = n;
        continue;
      }

      // We found a box so scan until we find the end of the row of boxes
      auto b = n + dir;
      while (gridP1[b] == 'O')
        { b += dir; }

      // If it was a wall we can't push, do nothing.
      if (gridP1[b] == '#')
        { continue; }

      // Otherwise, we pushed the column one which is equivalent to adding a box in the empty spot we found and
      //  removing one from where we are about to move.
      gridP1[b] = 'O';
      gridP1[n] = '.';
      robotPos = n;
    }

    // Sum up the GPS coords.
    for (auto c : gridP1.IterCoords<s32>())
    {
      if (gridP1[c] == 'O')
        { p1 += 100*c.y + c.x; }
    }


    // Part 2 is trickier because we can push multiple boxes in expanding out.
    std::vector<Vec> pushBoxes;
    std::deque<Vec> boxQueue;
    robotPos = initialRobotPos * DoubleX;
    for (auto dir : moves)
    {
      auto n = robotPos + dir;
      if (gridP2[n] == '#')
        { continue; } // like in P1 if we try to move into a wall, just don't.

      if (gridP2[n] == '.')
      {
        // Also like P2 we can happily move into empty space.
        robotPos = n;
        continue;
      }

      if (dir.x != 0)
      {
        // Pushing left and right is very similar to the P1 solution, except we can't shortcut the moving of the box
        //  chars because they alternate now. (Probably could have done something slightly smarter but this was good
        //  enough)
        auto b = n + dir;
        // Scan until we run out of boxes in a row
        while (gridP2[b] == '[' || gridP2[b] == ']')
          { b += dir; }

        // If it was a wall we can't push, do nothing.
        if (gridP2[b] == '#')
          { continue; }

        // Now slide all of the characters into the destination positions.
        for (auto pos = b; pos != n; pos -= dir)
          { gridP2[pos] = gridP2[pos - dir]; }

        // Clear the grid space that the first block was pushed out of.
        gridP2[n] = '.';
        robotPos = n;
        continue;
      }

      // Now pushing up/down is more complicated - we're going to use a list of blocks being pushed and a queue of
      //  boxes to test where they're going. The nice thing about using a queue is that as we commit boxes into the
      //  list, they'll be in bottom to top order which means we can iterate backwards through the list and safely move
      //  the boxes (assuming we could move at all)
      pushBoxes.clear();
      boxQueue.clear();

      // The box we're pushing is one to the left of our position if we're pushing the right side of it, otherwise it
      //  is directly above us.
      boxQueue.push_back(gridP2[n] == ']' ? (n - AddX) : n);

      bool blocked = false;
      while (!boxQueue.empty())
      {
        auto box = boxQueue.front();
        boxQueue.pop_front();

        auto pushT = box + dir;
        if (gridP2[pushT] == '#' || gridP2[pushT + AddX] == '#')
        {
          // If any block we consider pushes into a wall we can't move at all and are done.
          blocked = true;
          break;
        }

        // This box could move, so commit it.
        pushBoxes.push_back(box);

        if (gridP2[pushT] == '.' && gridP2[pushT + AddX] == '.')
          { continue; } // Not only could it move, but it's not pushing anything. Yay!

        if (gridP2[pushT] == '[')
        {
          // The char above the left side is a box left side so we're pushing only one box.
          boxQueue.push_back(pushT);
        }
        else
        {
          // We're pushing one or two boxes, figure out which boxes we're pushing and add them to the queue.
          if (gridP2[pushT] == ']')
            { boxQueue.push_back(pushT - AddX); }
          if (gridP2[pushT + AddX] == '[')
            { boxQueue.push_back(pushT + AddX); }
        }
      }

      if (blocked)
        { continue; } // We can't move at all so go on to the next move.

      // As stated above, iterate through the boxes in reverse and move their chars up (and clear where they are coming
      //  from)
      for (auto box : pushBoxes | std::views::reverse)
      {
        auto pushT = box + dir;
        gridP2[pushT] = '[';
        gridP2[pushT + AddX] = ']';
        gridP2[box] = '.';
        gridP2[box + AddX] = '.';
      }

      // We moved!
      robotPos = n;
    }

    // Same calculation as for p1, except now for 2.
    for (auto c : gridP2.IterCoords<s32>())
    {
      if (gridP2[c] == '[')
        { p2 += 100*c.y + c.x; }
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}