export module Util:Types;

import std;

// Aliases of basic types
export using s8 = std::int8_t;
export using u8 = std::uint8_t;

export using s16 = std::int16_t;
export using u16 = std::uint16_t;

export using s32 = std::int32_t;
export using u32 = std::uint32_t;

export using s64 = std::int64_t;
export using u64 = std::uint64_t;

export using f32 = float;
export using f64 = double;

export using ssz = std::make_signed_t<size_t>;
export using usz = size_t;

export template <typename T, typename FuncSig>
inline constexpr bool k_isCallableAs = false;

export template <typename T, typename RetVal, typename ... FuncArgs>
  requires requires (T func, FuncArgs...args) { { std::forward<T>(func)(std::forward<FuncArgs>(args)...) } -> std::same_as<RetVal>; }
inline constexpr bool k_isCallableAs<T, RetVal(FuncArgs...)> = true;

export template <typename T, typename FuncSig>
concept callable_as = k_isCallableAs<T, FuncSig>;

export template <typename T> concept numeric = (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool>;


export template <typename T> concept enum_class = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;
export template <typename T> concept flag_enum_class =
  enum_class<T>
  && std::is_same_v<decltype(T::None), T>
  && std::is_same_v<decltype(T::All), T>;

export template <flag_enum_class T>
constexpr T operator|(T a, T b)
  { return T(std::underlying_type_t<T>(a) | std::underlying_type_t<T>(b)); }

export template <flag_enum_class T>
constexpr T &operator|=(T &a, T b)
{
  a = a | b;
  return a;
}


export template <flag_enum_class T>
constexpr T operator&(T a, T b)
  { return T(std::underlying_type_t<T>(a) & std::underlying_type_t<T>(b)); }

export template <flag_enum_class T>
constexpr T &operator&=(T &a, T b)
{
  a = a & b;
  return a;
}


export template <flag_enum_class T>
constexpr T operator^(T a, T b)
  { return T(std::underlying_type_t<T>(a) ^ std::underlying_type_t<T>(b)); }

export template <flag_enum_class T>
constexpr T &operator^=(T &a, T b)
{
  a = a ^ b;
  return a;
}


export template <flag_enum_class T>
constexpr T operator~(T v)
  { return T(~std::underlying_type_t<T>(v)); }

export template <flag_enum_class T>
constexpr bool operator!(T v)
  { return std::underlying_type_t<T>(v) == 0; }
