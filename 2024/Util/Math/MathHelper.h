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


template <std::integral T>
T GreatestCommonDemoninator(T a, T b)
{
  for (;;)
  {
    if (a == 0)
      { return b; }

    b %= a;
    if (b == 0)
      { return a; }

    a %= b;
  }
}

template <std::integral T>
T LeastCommonMultiple(T a, T b)
{
  auto gcd = GreatestCommonDemoninator(a, b);
  return gcd ? (a / gcd * b) : 0;
}


template <std::integral T>
T LeastCommonMultiple(ArrayView<T> list)
{
  T mul = 1;
  for (auto v : list)
    { mul = LeastCommonMultiple(mul, v); }

  return mul;
}


template <std::integral T>
u32 CountSetBits(T v)
  { return s32(std::popcount(std::make_unsigned_t<T>(v))); }


template <numeric T>
T Square(T v)
  { return v * v; }

// Wrapper around std::ranges::fold_left because it's easier to type and I cannot for the life of me remember the name
//  fold_left
template <std::ranges::viewable_range R> requires numeric<std::ranges::range_value_t<R>>
auto Sum(const R &r)
  -> std::ranges::range_value_t<R>
{
  using N = std::ranges::range_value_t<R>;
  return std::ranges::fold_left(r, N(0), std::plus<N>{});
}


// Wrap an integral value to be in [0, max)
template <std::integral I>
I WrapIndex(I v, I max)
{
  if constexpr (std::is_unsigned_v<I>)
    { return v % max; } // Unsigned values are easy to wrap! A single modulo and we're done.
  else
  {
    using U = std::make_unsigned_t<I>;

    // The original logic was the following:
    //   return ((v % max) + max) % max;
    // However that uses two modulos, which are known to be *check's notes* not fast. There's another way to get
    //  this result, though, and that's:
    //   return (v < 0) ? (max - 1 + ((v + 1) % max)) : (v % max);
    // That is, if it's positive (or zero), do a straight modulo, otherwise add one, do a modulo, then add that value
    //  to max - 1 (I promise this logic works).
    // The below logic is the equivalente of the above, but without the conditional, and runs considerably faster
    //  than the two-modulo version.
    constexpr u32 Shift = sizeof(I) * CHAR_BIT - 1; // Number of bits in the type - 1 (so, 63 for an s64)
    return U(((max - 1) & (v >> Shift)) // == (max - 1) if index is negative, 0 if not
      + ((v + I(U(v) >> Shift)) % max)); // Add 1 to index if signed, 0 if not, then modulo
  }
}
