#pragma once

#ifdef ASSERT
    #undef ASSERT
#endif
#ifdef VERIFY
    #undef VERIFY
#endif
#ifdef TRACE
    #undef TRACE
#endif

namespace NS_Debug
{
  enum class DialogResult
  {
    Ignore,
    IgnoreAlways,
    Break
  };

  DialogResult AssertTaskDialog(const wchar_t *title, const char *file, int line, bool allButtons, const char *message);
  void Trace(const char *message);
}

#ifdef _DEBUG
  #define ASSERT(condition) \
  do \
  { \
    auto ASSERT_res = condition; \
    if (!ASSERT_res) \
    { \
      if ([&]() \
      { \
        static bool ASSERT___ignoreAlways = false;  \
        if(!ASSERT___ignoreAlways && !ASSERT_res) \
        { \
          switch(::NS_Debug::AssertTaskDialog(L"Assertion Failed", __FILE__, __LINE__, true, #condition)) \
          { \
          case ::NS_Debug::DialogResult::Ignore: \
            break; \
          case ::NS_Debug::DialogResult::IgnoreAlways: \
            ASSERT___ignoreAlways = true; \
            break; \
          default: \
            return true; \
          } \
        } \
        return false; \
      }()) \
      { \
        __debugbreak(); \
      } \
    } \
  } \
  while (false)
#else
  #define ASSERT(condition)
#endif