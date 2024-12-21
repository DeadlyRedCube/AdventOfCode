#pragma once


namespace D21
{
  using Vec = Vec2<s32>;
  constexpr auto X = Vec::XAxis();
  constexpr auto Y = Vec::YAxis();


  std::string FindPath(Vec startPos, Array2D<char> &grid, std::string_view code)
  {
    std::string presses;

    std::map<char, Vec> pos;
    for (auto c : grid.IterCoords<s32>())
      { pos[grid[c]] = c; }

    auto cur = startPos;
    for (auto tc : code)
    {
      Vec t = pos[tc];

      while (true)
      {
        if (t == cur)
        {
          presses.push_back('A');
          break;
        }

        bool moveX;
        if (t.x == pos[0].x && cur.y == pos[0].y)
        {
          moveX = false;
        }
        else if (t.y == pos[0].y && cur.x == pos[0].x)
        {
          moveX = true;
        }
        else if (cur.x == t.x)
          { moveX = false; }
        else if (cur.y == t.y)
          { moveX = true; }
        else if (t.x > cur.x && t.y > cur.y)
          { moveX = false; }
        else
          { moveX = true; }

        if (moveX)
        {
          while (true)
          {
            if (t.x < cur.x)
            {
              presses.push_back('<');
              cur.x--;
            }
            else if(t.x > cur.x)
            {
              presses.push_back('>');
              cur.x++;
            }
            else
              { break; }
          }
        }
        else
        {
          while (true)
          {
            if (t.y < cur.y)
            {
              presses.push_back('^');
              cur.y--;
            }
            else if (t.y > cur.y)
            {
              presses.push_back('v');
              cur.y++;
            }
            else
              { break; }
          }
        }
      }
    }

    return presses;
  }



  enum class Pattern
  {
    LUA, // -> DLA | RUA | RA + 1
    ULA, // -> LA | DLA | RUA + 1
    LA,  // -> DLA | RUA + 2
    LDA, // -> DLA | RA | RUA + 1
    DLA, // -> LDA | LA | RUA + 1
    UA,  // -> LA | RA
    DA,  // ->  LDA | RUA
    RUA, // ->  DA | LUA | RA
    RA,  // -> DA | UA
    DRA, // -> LDA | RA | UA
  };


  void Run(const char *filePath)
  {
    s64 p1 = 0;
    s64 p2 = 0;

    auto numkeys = Array2D<char>{3, 4};
    numkeys[0, 0] = '7';
    numkeys[1, 0] = '8';
    numkeys[2, 0] = '9';
    numkeys[0, 1] = '4';
    numkeys[1, 1] = '5';
    numkeys[2, 1] = '6';
    numkeys[0, 2] = '1';
    numkeys[1, 2] = '2';
    numkeys[2, 2] = '3';
    numkeys[0, 3] = 0;
    numkeys[1, 3] = '0';
    numkeys[2, 3] = 'A';

    auto dirs = Array2D<char>{3, 2};
    dirs[0, 0] = 0;
    dirs[1, 0] = '^';
    dirs[2, 0] = 'A';
    dirs[0, 1] = '<';
    dirs[1, 1] = 'v';
    dirs[2, 1] = '>';

    auto lines = ReadFileLines(filePath);

    char shortest[10]
    {

    };



    for (auto &line : lines)
    {

      auto path = FindPath({2,3}, numkeys, line);

      for (s32 i = 0; i < 2; i++)
      {
        path = FindPath({2,0}, dirs, path);
      }

      char *end;
      p1 += path.size() * std::strtoll(line.c_str(), &end, 10);
    }



    PrintFmt("P1: {}\n", p1);
    PrintFmt("P2: {}\n", p2);
  }
}