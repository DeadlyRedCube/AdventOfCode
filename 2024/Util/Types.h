#pragma once

// Aliases of basic types
using s8 = std::int8_t;
using u8 = std::uint8_t;

using s16 = std::int16_t;
using u16 = std::uint16_t;

using s32 = std::int32_t;
using u32 = std::uint32_t;

using s64 = std::int64_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

using ssz = std::make_signed_t<size_t>;
using usz = size_t;

template <typename T, typename FuncSig>
inline constexpr bool k_isCallableAs = false;

template <typename T, typename RetVal, typename ... FuncArgs>
  requires requires (T func, FuncArgs...args) { { std::forward<T>(func)(std::forward<FuncArgs>(args)...) } -> std::same_as<RetVal>; }
inline constexpr bool k_isCallableAs<T, RetVal(FuncArgs...)> = true;

template <typename T, typename FuncSig>
concept callable_as = k_isCallableAs<T, FuncSig>;

template <typename T> concept numeric = (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool>;


template <typename T> concept enum_class = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;
template <typename T> concept flag_enum_class =
  enum_class<T>
  && std::is_same_v<decltype(T::None), T>
  && std::is_same_v<decltype(T::All), T>;

template <flag_enum_class T>
inline constexpr T operator|(T a, T b)
  { return T(std::underlying_type_t<T>(a) | std::underlying_type_t<T>(b)); }

template <flag_enum_class T>
inline constexpr T &operator|=(T &a, T b)
{
  a = a | b;
  return a;
}


template <flag_enum_class T>
inline constexpr T operator&(T a, T b)
  { return T(std::underlying_type_t<T>(a) & std::underlying_type_t<T>(b)); }

template <flag_enum_class T>
inline constexpr T &operator&=(T &a, T b)
{
  a = a & b;
  return a;
}


template <flag_enum_class T>
inline constexpr T operator^(T a, T b)
  { return T(std::underlying_type_t<T>(a) ^ std::underlying_type_t<T>(b)); }

template <flag_enum_class T>
inline constexpr T &operator^=(T &a, T b)
{
  a = a ^ b;
  return a;
}


template <flag_enum_class T>
inline constexpr T operator~(T v)
  { return T(~std::underlying_type_t<T>(v)); }

template <flag_enum_class T>
inline constexpr bool operator!(T v)
  { return std::underlying_type_t<T>(v) == 0; }
