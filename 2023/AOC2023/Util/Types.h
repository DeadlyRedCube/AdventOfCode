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
