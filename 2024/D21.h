#pragma once


namespace D21
{
  using Vec = Vec2<s32>;

  // There are 12 different orders in which to move around the arrow keypad (ignoring double presses of buttons),
  //  one each for the 4 cardinal directions and then two orderings to move diagonally per diagonal direction.
  // Basically each of these patterns is one or two directions, then the "A" button (the A button always resets the
  //  keypad position).
  enum class ButtonPattern
  {
    // Going left and up (in either order)
    LUA, // <^A
    ULA, // ^<A

    // Left only
    LA,  // <A

    // Left and down (either order)
    LDA, // <vA
    DLA, // v<A

    // Up only
    UA,  // ^A

    // Down only
    DA,  // vA

    // Up and right (either order)
    URA, // ^>A
    RUA, // >^A

    // Right only
    RA,  // >A

    // Down and right (Either order)
    DRA, // v>A
    RDA, // >vA
  };


  // Memoize based on the pattern and the remaining depth
  Array2D<s64> expansionMap {12, 26}; // 12 patterns x 25 max depth (so 26 to store depth == 25).


  // Count the buttons that the given button pattern turns into (recursively). Any given movement from A and back
  //  expands into a very specific pattern of optimal movements from A and back.
  // Basically, prefer moving left before moving vertically, and prefer moving vertically before moving right (and
  //  never, ever, turn more than once per move. Going "<v<A" would be a mistake as double presses are cheap all the
  //  way up the stack).
  //  If you're moving to or from the left (<) key, though, you can't move left or up first, respectively, so instead
  //  it turns into a down->left->A or a right->up->A.
  s64 RecurseButtonPresses(ButtonPattern pat, u32 depth)
  {
    using enum ButtonPattern;

    if (depth == 0)
    {
      // Once we bottom out then it's just the number of button presses in the pattern itself.
      switch (pat)
      {
      case LUA: return 3;
      case ULA: return 3;
      case LA:  return 2;
      case LDA: return 3;
      case DLA: return 3;
      case UA:  return 2;
      case DA:  return 2;
      case URA: return 3;
      case RUA: return 3;
      case RA:  return 2;
      case DRA: return 3;
      case RDA: return 3;
      }

      ASSERT(false);
      return 0;
    }

    // Look up the memoized version, if we have one return it immediately.
    auto &v = expansionMap[u32(pat), depth];
    if (v != 0)
      { return v; }

    // I got lazy
    auto Recurse = [&](ButtonPattern pat) { return RecurseButtonPresses(pat, depth - 1); };

    switch (pat)
    {
    case LUA: // becomes v<<A >^A >A
      v = Recurse(DLA) + Recurse(RUA) + Recurse(RA) + 1; // +1 is for the repeated '<'
      break;
    case ULA: // becomes <A v<A >>^A
      v = Recurse(LA) + Recurse(DLA) + Recurse(RUA) + 1; // +1 for repeated '>'
      break;
    case LA:  // becomes v<<A >>^A
      v = Recurse(DLA) + Recurse(RUA) + 2; // + 1 for the repeated '<' then another + 1 for the extra '>'
      break;
    case LDA: // becomes v<<A >A ^>A
      v = Recurse(DLA) + Recurse(RA) + Recurse(URA) + 1; // + 1 for the extra '<'
      break;
    case DLA: // becomes <vA <A >>^A
      v = Recurse(LDA) + Recurse(LA) + Recurse(RUA) + 1; // + 1 for the extra '>'
      break;
    case UA:  // becomes <A >A
      v = Recurse(LA) + Recurse(RA);
      break;
    case DA:  // becomes <vA ^>A
      v = Recurse(LDA) + Recurse(URA);
      break;
    case URA: // becomes <A v>A ^A
      v = Recurse(LA) + Recurse(DRA) + Recurse(UA);
      break;
    case RUA: // becomes vA <^A >A
      v = Recurse(DA) + Recurse(LUA) + Recurse(RA);
      break;
    case RA:  // becomes vA ^A
      v = Recurse(DA) + Recurse(UA);
      break;
    case DRA: // becomes <vA >A ^A
      v = Recurse(LDA) + Recurse(RA) + Recurse(UA);
      break;
    case RDA:  // becomes vA <A ^>A
      v = Recurse(DA) + Recurse(LA) + Recurse(URA);
      break;
    }

    return v;
  }


  s64 TypeCode(std::string_view code, u32 depth)
  {
    // Start on the A key always
    Vec pos = {2,3};

    s64 count = 0;
    for (auto c : code)
    {
      // Figure out the target position
      Vec target;
      if (c == 'A')
        { target = {2,3}; }
      else if (c == '0')
        { target = {1,3}; }
      else
      {
        // The 1-9 buttons are in a 3x3 grid so a divide and modulo will get the position.
        s32 v = c - '1';
        target.x = v % 3;
        target.y = 2 - v / 3;
      }

      if (target == pos) // This doesn't happen in the real data but it's easy to handle, so whatever.
        { count++; continue; } // An extra press of A is an extra press of A the wholllleeeee way down.


      // As mentioned above, the rules for choosing the direction to move first (of the up-to-two directions that will
      //  be keyed in) is as follows:
      //  - Only turn once per move from key to key
      //  - Prefer moving left vs. vertically, and prefer moving vertically vs. right
      //  - Unless doing so would put you through the empty space (lower left corner of the numeric pad), then you
      //    need to start in the other direction.
      using enum ButtonPattern;
      ButtonPattern pat;
      if (target.x == pos.x)
      {
        // We're horizontally in line with the target so we just move up or down
        pat = (target.y > pos.y) ? DA : UA;
      }
      else if (target.y == pos.y)
      {
        // Vertically in line so move left or right
        pat = (target.x > pos.x) ? RA : LA;
      }
      else if (target.x > pos.x)
      {
        // Moving to the right, so we prefer moving vertically first.
        if (target.y > pos.y)
        {
          // Can't start vertically if it would move us into {3, 0}.
          pat = (target.y == 3 && pos.x == 0) ? RDA :DRA;
        }
        else
        {
          // On the numeric keypad (unlike the arrow pad) we can always go up first, as preferred.
          pat = URA;
        }
      }
      else
      {
        // We're moving to the left, so prefer left vs. vertical.
        if (target.y > pos.y)
        {
          pat = LDA;
        }
        else
        {
          // If we would move into {3, 0} going horizontally, start by going up first instead.
          pat = (target.x == 0 && pos.y == 3) ? ULA : LUA;
        }
      }

      // For every move we take along a direction past the first one, it's a repeated button press (Which just
      //  becomes a single extra 'A' press at the outermost level, so we can just add one per)
      // For instance, moving from 'A' to '2' is a ULA pattern, one move up, one move left, one press of A, so that's
      //  3 keys (counted by RecurseButtonPresses). However, from 'A' to '8' is *three* moves up and one move left, so
      //  5 keys. It's still considered a ULA pattern (Which is 3 keys), so we'd add 2 extra presses (of 'A') for this
      //  movement.
      auto extra = std::max(0, std::abs(target.y - pos.y) - 1) + std::max(0, std::abs(target.x - pos.x) - 1);

      ASSERT(extra >= 0);
      count += RecurseButtonPresses(pat, depth) + extra;

      // Start tracking from the new position for the next button press.
      pos = target;
    }

    return count;
  }


  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    for (auto &line : ReadFileLines(filePath))
    {
      auto multiplier = AsS32(line);
      p1 += multiplier * TypeCode(line, 2);
      p2 += multiplier * TypeCode(line, 25);
    }

    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}