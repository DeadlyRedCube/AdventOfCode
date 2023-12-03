#pragma once


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

