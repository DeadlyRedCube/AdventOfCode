namespace D16
{
  void Run(const char *path)
  {
    auto grid = ReadFileAsCharArray(path);

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


    struct Beam
    {
      Vec2S32 coord = { 0, 0 };
      Vec2S32 dir = { 1, 0 };
    };

    UnboundedArray<Beam> beams;

    beams.Append({ .coord = { 0, 0 }, .dir = { 1, 0 } });

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

        if ((energized.Idx(beam->coord.x, beam->coord.y) & d) != Dir::None)
          { break; }

        energized.Idx(beam->coord.x, beam->coord.y) |= d;
        switch (grid.Idx(beam->coord.x, beam->coord.y))
        {
        case '.':
          break;

        case '/':
          beam->dir = { -beam->dir.y, -beam->dir.x };
          break;

        case '\\':
          beam->dir = { beam->dir.y, beam->dir.x };
          break;

        case '|':
          if (beam->dir.x == 0)
            { break; }

          beam->dir = { 0, 1 };
          {
            auto newBeam = Beam{ .coord = {beam->coord + Vec2S32{0, -1}}, .dir = { 0, -1 } };
            if (newBeam.coord.x >= 0 && newBeam.coord.x < grid.Width() && newBeam.coord.y >= 0 && newBeam.coord.y < grid.Height())
              { beams.Append(newBeam); }
          }
          beam = &beams[i];
          break;

        case '-':
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

        if (beam->coord.x < 0 || beam->coord.x >= grid.Width() || beam->coord.y < 0 || beam->coord.y >= grid.Height())
          { break; }
      }
    }

    s32 count = 0;
    for (s32 y = 0; y < energized.Height(); y++)
    {
      for (s32 x = 0; x < energized.Width(); x++)
      {
        if (energized.Idx(x, y) != Dir::None)
          { count++; }
        //PrintFmt("{}", (energized.Idx(x, y) != Dir::None) ? '#' : '.');
      }
      //PrintFmt("\n");
    }

    PrintFmt("{}\n", count);
  }
}