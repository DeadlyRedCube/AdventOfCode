#pragma once


using ssize_t = std::make_signed_t<size_t>;

std::vector<std::string> split(const std::string& s, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

void ReplaceFirst(
    std::string *s,
    std::string const &toReplace,
    std::string const &replaceWith)
{
  std::size_t pos = s->find(toReplace);
  if (pos == std::string::npos) return;
  s->replace(pos, toReplace.length(), replaceWith);
}


void ReplaceAll(
  std::string *s,
  std::string const& toReplace,
  std::string const& replaceWith)
{
  std::string buf;
  std::size_t pos = 0;
  std::size_t prevPos;

  // Reserves rough estimate of final size of string.
  buf.reserve(s->size());

  while (true)
  {
    prevPos = pos;
    pos = s->find(toReplace, pos);
    if (pos == std::string::npos)
      { break; }

    buf.append(*s, prevPos, pos - prevPos);
    buf += replaceWith;
    pos += toReplace.size();
  }

  buf.append(*s, prevPos, s->size() - prevPos);
  std::swap(*s, buf);
}

std::vector<std::string> ReadFileLines(const char *filename)
{
  std::ifstream infile { filename };
  assert(infile);

  std::vector<std::string> vec;
  while (!infile.eof())
  {
    std::string s;
    std::getline(infile, s);
    vec.push_back(s);
  }

  if (vec[vec.size() - 1].length() == 0)
    { vec.pop_back(); }

  return vec;
}


enum class KeepEmpty
{
  No,
  Yes
};

std::vector<std::string> Split(const std::string &s, const std::string &delims, KeepEmpty ke)
{
  std::string buf;
  std::vector<std::string> v;
  for (auto c : s)
  {
    if (delims.find(c) != std::string::npos)
    {
      if (buf.size() > 0 || ke == KeepEmpty::Yes)
      {
        v.push_back(std::move(buf));
        buf = {};
      }
    }
    else
    {
      buf += c;
    }
  }

  if (buf.size() > 0 || ke == KeepEmpty::Yes)
    { v.push_back(std::move(buf)); }

  return v;
}


template <typename... t_args>
void PrintFmt(std::format_string<t_args...> fmt, t_args&&...args)
{
  printf(std::format(fmt, std::forward<t_args>(args)...).c_str());
}



template <typename T>
class Array2D
{
public:
  Array2D() = default;
  Array2D(ssize_t w, ssize_t h)
    : width(w)
    , height(h)
    , data(new T[w * h])
    { }

  ~Array2D()
    { delete[] data; }

  ssize_t Width() const
    { return width; }

  ssize_t Height() const
    { return height; }

  T &Idx(ssize_t x, ssize_t y)
  {
    assert(x < width && y < height);
    return data[x + y * width];
  }

  const T &Idx(ssize_t x, ssize_t y) const
  {
    assert(x < width && y < height);
    return data[x + y * width];
  }

  void Fill(const T &v)
  {
    for (ssize_t i = 0; i < width* height; i++)
      { data[i] = v; }
  }


private:
  ssize_t width = 0;
  ssize_t height = 0;
  T *data = nullptr;
};


Array2D<char> MakeCharArray(const std::vector<std::string> &v)
{
  Array2D<char> ary { ssize_t(v[0].length()), ssize_t(v.size()) };
  for (ssize_t y = 0; y < ary.Height(); y++)
  {
    for (ssize_t x = 0; x < ary.Width(); x++)
    {
      ary.Idx(x, y) = v[y][x];
    }
  }

  return ary;
}