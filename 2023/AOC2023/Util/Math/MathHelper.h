#pragma once

template <numeric T> constexpr T Zero = T(0);
template <numeric T> constexpr T One = T(1);
template <numeric T> constexpr T Two = T(2);
template <std::floating_point T> constexpr T Half = T(0.5f);

template <std::floating_point T> constexpr T E = T(2.71828182845904523536028747135266249);
template <std::floating_point T> constexpr T Log10E = T(0.4342944819032518276511289189166);
template <std::floating_point T> constexpr T Log2E = T(1.44269504088896340735992468100189);
template <std::floating_point T> constexpr T Pi = T(3.1415926535897932384626433832795);
template <std::floating_point T> constexpr T PiOver2 = T(1.5707963267948966192313216916398);
template <std::floating_point T> constexpr T PiOver4 = T(0.78539816339744830961566084581988);
template <std::floating_point T> constexpr T TwoPi = T(6.283185307179586476925286766559);
template <std::floating_point T> constexpr T Epsilon = T(1e-5);
template <> constexpr f64 Epsilon<f64> = 1e-9;


template <typename T>
struct ValueRange
{
  bool operator==(const ValueRange &) const = default;

  T a;
  T b;
};

template <std::floating_point T>
constexpr T Lerp(ValueRange<T> r, T u) noexcept
  { return r.a * (T(1.0) - u) + r.b * u; }

template <numeric T>
constexpr T Clamp(T v, ValueRange<T> r)
  { return std::clamp(v, r.a, r.b); }

template <std::floating_point T>
constexpr T RadFromDeg(T deg) noexcept
  { return deg * Pi<T> / T(180.0); }

template <std::floating_point T>
constexpr T DegFromRad(T rad) noexcept
  { return rad * T(180.0) / Pi<T>; }

// Wraps an angle to be within the range of -pi ... pi
template <std::floating_point T>
inline T WrapAngleRad(T angle) noexcept
{
  angle += Pi<T>;
  angle -= TwoPi<T> * std::floor(angle / TwoPi<T>);
  return angle - Pi<T>;
}

constexpr f32 operator "" _deg (long double deg) noexcept
  { return RadFromDeg(f32(deg)); }

constexpr f32 operator "" _deg (unsigned long long deg) noexcept
  { return RadFromDeg(f32(deg)); }

constexpr u32 CountSetBits(std::integral auto val) noexcept
  { return std::popcount(val); }