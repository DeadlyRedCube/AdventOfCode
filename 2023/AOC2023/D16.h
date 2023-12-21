namespace D16
{
  struct Beam
  {
    Vec2S32 coord = { 0, 0 };
    Vec2S32 dir = { 1, 0 };
  };


  s32 CalculateEnergization(const Array2D<char> &grid, Beam start)
  {
    enum class Dir
    {
      N = 0x01,
      S = 0x02,
      E = 0x04,
      W = 0x08,

      None = 0,
      All = 0x0F,
    };

    auto energized = Array2D<Dir> { grid.Width(), grid.Height() };
    energized.Fill(Dir::None);


    UnboundedArray<Beam> beams;

    beams.Append(start);

    for (ssz i = 0; i < beams.Count(); i++)
    {
      for (;;)
      {
        auto *beam = &beams[i];

        Dir d = Dir::None;
        if (beam->dir.x < 0)
          { d = Dir::W; }
        if (beam->dir.x > 0)
          { d = Dir::E; }
        if (beam->dir.y < 0)
          { d = Dir::N; }
        if (beam->dir.y > 0)
          { d = Dir::S; }

        // If we already were energized in this direction we already know the rest
        if ((energized[beam->coord] & d) != Dir::None)
          { break; }

        // Add energy from this direction to this square
        energized[beam->coord] |= d;

        switch (grid[beam->coord])
        {
        case '.':
          break;

        case '/': // Reflect 90
          beam->dir = { -beam->dir.y, -beam->dir.x };
          break;

        case '\\': // Reflect 90, the other way
          beam->dir = { beam->dir.y, beam->dir.x };
          break;

        case '|': // Split if horizontal
          if (beam->dir.x == 0)
            { break; }

          beam->dir = { 0, 1 };
          {
            auto newBeam = Beam{ .coord = {beam->coord + Vec2S32{0, -1}}, .dir = { 0, -1 } };
            if (newBeam.coord.x >= 0 && newBeam.coord.x < grid.Width() && newBeam.coord.y >= 0 && newBeam.coord.y < grid.Height())
              { beams.Append(newBeam); }
          }

          // adding to the array may have caused it to reallocate so re-grab our pointer to our current beam.
          beam = &beams[i];
          break;

        case '-': // Split if vertical
          if (beam->dir.y == 0)
            { break; }

          beam->dir = { 1, 0 };
          {
            auto newBeam = Beam{ .coord = {beam->coord + Vec2S32{-1, 0}}, .dir = { -1, 0 } };
            if (newBeam.coord.x >= 0 && newBeam.coord.x < grid.Width() && newBeam.coord.y >= 0 && newBeam.coord.y < grid.Height())
              { beams.Append(newBeam); }
          }

          beam = &beams[i];
          break;

        default:
          ASSERT(false);
        }

        beam->coord += beam->dir;

        // If we beamed off the edge we're done.
        if (beam->coord.x < 0 || beam->coord.x >= grid.Width() || beam->coord.y < 0 || beam->coord.y >= grid.Height())
          { break; }
      }
    }

    s32 count = 0;
    for (s32 y = 0; y < energized.Height(); y++)
    {
      for (s32 x = 0; x < energized.Width(); x++)
      {
        if (energized[{x, y}] != Dir::None)
          { count++; }
      }
    }

    return count;
  }

  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

    // Part 1
    PrintFmt("Part 1: {}\n", CalculateEnergization(grid, { .coord = { 0, 0 }, .dir = { 1, 0 } }));

    // Part 2: calculate the max from every side
    s32 maxCount = 0;
    for (s32 y = 0; y < grid.Height(); y++)
    {
      maxCount = std::max(CalculateEnergization(grid, { .coord = { 0, y }, .dir = { 1, 0 } }), maxCount);
      maxCount = std::max(CalculateEnergization(grid, { .coord = { s32(grid.Width()) - 1, y }, .dir = { -1, 0 } }), maxCount);
    }

    for (s32 x = 0; x < grid.Width(); x++)
    {
      maxCount = std::max(CalculateEnergization(grid, { .coord = { x, 0 }, .dir = { 0, 1 } }), maxCount);
      maxCount = std::max(CalculateEnergization(grid, { .coord = { x, s32(grid.Height()) - 1 }, .dir = { 0, -1 } }), maxCount);
    }

    PrintFmt("Part 2: {}\n", maxCount);
  }
}