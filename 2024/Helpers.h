#pragma once

#include "Util/Math/Vec.h"

using ssize_t = std::make_signed_t<size_t>;

s64 StrToNum(const std::string &s)
  { return std::atoll(s.c_str()); }

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


std::string ReadEntireFile(const char *filename)
{
  std::ifstream infile { filename };
  ASSERT(!!infile);

  std::string outStr;
  while (!infile.eof())
  {
    if (!outStr.empty())
      { outStr += "\n"; }
    std::string s;
    std::getline(infile, s);
    outStr += s;
  }

  return outStr;
}


std::vector<std::string> ReadFileLines(const char *filename)
{
  std::ifstream infile { filename };
  ASSERT(!!infile);

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


  inline ssz WrapIndex(ssz i, ssz count)
  {
    // equivalent to: return (index < 0) ? (m_count - 1 + ((index + 1) % m_count)) : index % m_count,
    constexpr size_t k_shift = std::numeric_limits<size_t>::digits - 1;
    ssz index = ssz(i);
    return ((count - 1) & (index >> k_shift)) + ((index + ssz(usz(index) >> k_shift)) % count);
  }


template <typename T>
class Array2D
{
public:
  Array2D() = default;
  Array2D(ssize_t w, ssize_t h)
    : width(w)
    , height(h)
    , data(new T[w * h] {})
    { }

  Array2D(const Array2D &) = delete;
  auto operator=(const Array2D &) = delete;

  Array2D(Array2D &&other) noexcept
    : width(std::exchange(other.width, 0))
    , height(std::exchange(other.height, 0))
    , data(std::exchange(other.data, nullptr))
    { }


  Array2D &operator=(Array2D &&other)
  {
    delete[] data;

    width = std::exchange(other.width, 0);
    height = std::exchange(other.height, 0);
    data = std::exchange(other.data, nullptr);

    return *this;
  }

  ~Array2D()
    { delete[] data; }

  ssize_t Width() const
    { return width; }

  ssize_t Height() const
    { return height; }

  void Fill(const T &v)
  {
    for (ssize_t i = 0; i < width * height; i++)
      { data[i] = v; }
  }

  template <std::integral I>
  T &operator[](Vec2<I> v)
    { return Idx(v.x, v.y); }


  template <std::integral I>
  const T &operator[](Vec2<I> v) const
    { return Idx(v.x, v.y); }

  template <std::integral I>
  T &operator[](std::initializer_list<I> v)
    { ASSERT(v.size() == 2); return Idx(v.begin()[0], v.begin()[1]); }

  template <std::integral I>
  const T &operator[](std::initializer_list<I> v) const
    { ASSERT(v.size() == 2); return Idx(v.begin()[0], v.begin()[1]); }


  template <std::integral I>
  Vec2<I> Wrap(Vec2<I> v) const
  {
    return
    {
      WrapIndex(ssz(v.x), width),
      WrapIndex(ssz(v.y), height),
    };
  }

  bool PositionInRange(Vec2S32 v) const
    { return v.x >= 0 && v.x < width && v.y >= 0 && v.y < height; }

  bool PositionInRange(Vec2S64 v) const
    { return v.x >= 0 && v.x < width && v.y >= 0 && v.y < height; }

private:
  T &Idx(ssize_t x, ssize_t y)
  {
    ASSERT(x >= 0 && x < width && y >= 0 && y < height);
    return data[x + y * width];
  }

  const T &Idx(ssize_t x, ssize_t y) const
  {
    ASSERT(x < width && y < height);
    return data[x + y * width];
  }

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
      { ary[{x, y}] = v[y][x]; }
  }

  return ary;
}

Array2D<char> ReadFileAsCharArray(const char *path)
  { return MakeCharArray(ReadFileLines(path)); }


struct SSearchResult
{
  ssz index;
  std::string match;
};


// Calls a predicate on a substring for every character in the string, where the substring loses the first character
//  on each iteration
// i.e. if you Find on "abcd", it'll call the predicate on "abcd" then "bcd" then "cd" then "d"
// If the predicate returns a std::string, that's the "match" value, and we return that and the index it started on.
template <callable_as<std::optional<std::string>(const std::string_view &s)> PredicateType>
std::optional<SSearchResult> FindFirstMatch(const std::string_view &str, PredicateType pred)
{
  // This is a slow way to do it but whatever
  std::string_view v = str;

  for (ssz i = 0; !v.empty(); v = v.substr(1), i++)
  {
    if (auto res = pred(v); res.has_value())
      { return SSearchResult { i, *res }; }
  }

  return std::nullopt;
}

// Calls a predicate on a substring for every character in the string, where the substring loses the last character
//  on each iteration.
// i.e. if you Find on "abcd", it'll call the predicate on "abcd" then "abc" then "ab" then "a"
// If the predicate returns a std::string, that's the "match" value, and we return that and the index it ended on.
template <callable_as<std::optional<std::string>(const std::string_view &s)> PredicateType>
std::optional<SSearchResult> FindLastMatch(const std::string_view &str, PredicateType pred)
{
  // This is a slow way to do it but whatever
  std::string_view v = str;

  for (ssz i = str.length() - 1; !v.empty(); v = v.substr(0, v.length() - 1), i--)
  {
    if (auto res = pred(v); res.has_value())
      { return SSearchResult { i, *res }; }
  }

  return std::nullopt;
}


std::optional<std::smatch> FindFirstMatch(const std::regex &re, const std::string &str)
{
  std::smatch res;
  if (std::regex_search(str.cbegin(), str.cend(), res, re))
    { return res; }

  return std::nullopt;
}


std::optional<std::smatch> FindLastMatch(const std::regex &re, const std::string &str)
{
  auto iterStart = str.cbegin();
  auto iterEnd = str.cend();

  std::smatch lastMatch;
  // Start with the first match, and if we didn't find one there's no match at all.
  if (!std::regex_search(iterStart, iterEnd, lastMatch, re))
    { return std::nullopt; }

  // Start our search one character after the start of our first match.
  iterStart = lastMatch.prefix().second + 1;

  while (true)
  {
    // Start at the midpoint between just past our last match and the "known no matches after here" point.
    auto midpoint = iterStart + (iterEnd - iterStart) / 2;
    std::smatch match;
    if (std::regex_search(midpoint, str.cend(), match, re))
    {
      // Found a match after the midpoint, update our start point to be one past the start of it.
      lastMatch = std::move(match);
      iterStart = lastMatch.prefix().second + 1;
    }
    else
    {
      // No matches after this point, so we can narrow our allowed starting search space
      iterEnd = midpoint;
    }

    if (iterEnd <= iterStart)
      { return lastMatch; }
  }
}


std::vector<std::smatch> FindAllMatches(const std::regex &re, const std::string &str)
{
  std::smatch res;

  std::vector<std::smatch> matches;
  for (std::string::const_iterator searchStart( str.cbegin() ); std::regex_search( searchStart, str.cend(), res, re ); )
  {
    matches.push_back(res);
    searchStart = res.suffix().first;
  }
  return matches;
}

std::vector<std::smatch> FindAllMatchesOverlapping(const std::regex &re, const std::string &str)
{
  std::smatch res;

  std::vector<std::smatch> matches;
  for (std::string::const_iterator searchStart( str.cbegin() ) ; std::regex_search( searchStart, str.cend(), res, re );)
  {
    matches.push_back(res);
    searchStart = res.prefix().second + 1; // skip the first character in the match and start matching again from there.
  }
  return matches;
}


template <typename InT, typename AccT>
AccT AccumulateRange(InT &&range, AccT &&initial)
  { return std::accumulate(range.begin(), range.end(), initial); }


template <typename InT, typename AccT, typename PredicateType>
AccT AccumulateRange(InT &&range, AccT &&initial, PredicateType &&pred)
  { return std::accumulate(range.begin(), range.end(), initial, pred); }


std::string Join(std::initializer_list<std::string> strings, const char *joiner)
  { return AccumulateRange(strings, std::string{}, [&joiner](auto &&a, auto &&b) { return a.empty() ? b : a + joiner + b; }); }



template <typename T>
class Array3D
{
public:
  Array3D() = default;
  Array3D(ssz xDim, ssz yDim, ssz zDim)
    : dims { xDim, yDim, zDim }
    , data(new T[xDim * yDim * zDim] {})
    { }

  Array3D(const Array3D &) = delete;
  auto operator=(const Array3D &) = delete;

  Array3D(Array3D &&other) noexcept
    : data(std::exchange(other.data, nullptr))
  {
    for (auto i = 0; i < 3; i++)
      { dims[i] = std::exchange(other.dims[i], 0); }
  }


  Array3D &operator=(Array3D &&other)
  {
    delete[] data;

    for (auto i = 0; i < 3; i++)
      { dims[i] = std::exchange(other.dims[i], 0); }
    data = std::exchange(other.data, nullptr);

    return *this;
  }

  ~Array3D()
    { delete[] data; }

  ssize_t XDim() const
    { return dims[0]; }

  ssize_t YDim() const
    { return dims[1]; }

  ssize_t ZDim() const
    { return dims[2]; }

  void Fill(const T &v)
  {
    for (ssize_t i = 0; i < dims[0] * dims[1] * dims[2]; i++)
      { data[i] = v; }
  }

  template <std::integral I>
  T &operator[](Vec3<I> v)
    { return Idx(v.x, v.y, v.z); }


  template <std::integral I>
  const T &operator[](Vec3<I> v) const
    { return Idx(v.x, v.y, v.z); }

  T &operator[](ssz x, ssz y, ssz z)
    { return Idx(x, y, z); }

  const T &operator[](ssz x, ssz y, ssz z) const
    { return Idx(x, y, z); }

  template <std::integral I>
  Vec3<I> Wrap(Vec3<I> v) const
  {
    return
    {
      WrapIndex(ssz(v.x), dims[0]),
      WrapIndex(ssz(v.y), dims[1]),
      WrapIndex(ssz(v.z), dims[2]),
    };
  }

  bool PositionInRange(Vec3S32 v) const
    { return v.x >= 0 && v.x < dims[0] && v.y >= 0 && v.y < dims[1] && v.z >= 0 && v.z < dims[2]; }

  bool PositionInRange(Vec3S64 v) const
    { return v.x >= 0 && v.x < dims[0] && v.y >= 0 && v.y < dims[1] && v.z >= 0 && v.z < dims[2]; }

private:
  T &Idx(ssz x, ssz y, ssz z)
  {
    ASSERT(x >= 0 && x < dims[0] && y >= 0 && y < dims[1] && z >= 0 && z < dims[2]);
    return data[x + y * dims[0] + z * dims[0] * dims[1]];
  }

  const T &Idx(ssz x, ssz y, ssz z) const
  {
    ASSERT(x >= 0 && x < dims[0] && y >= 0 && y < dims[1] && z >= 0 && z < dims[2]);
    return data[x + y * dims[0] + z * dims[0] * dims[1]];
  }

  ssz dims[3];
  T *data = nullptr;
};


