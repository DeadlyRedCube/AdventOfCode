module;

#include <cassert>

export module Util:Math.Vec;

import :Types;
import :Math.MathHelper;

export template <numeric T>
class Vec2
{
public:
  constexpr Vec2() noexcept = default;
  constexpr Vec2(T xv, T yv)
    : x(xv)
    , y(yv)
    { }

  explicit constexpr Vec2(T v)
    : x(v)
    , y(v)
    { }

  constexpr T LengthSqr() const
    { return x * x + y * y; }

  constexpr T Length() const requires (std::floating_point<T>)
    { return std::sqrt(LengthSqr()); }

  constexpr T MinComponent() const
    { return std::min(x, y); }

  constexpr T MaxComponent() const
    { return std::max(x, y); }

  constexpr T &operator[](ssz i)
  {
    assert(InRangeArray(i, 2));
    return (&x)[i];
  }

  constexpr T operator[](ssz i) const
  {
    assert(InRangeArray(i, 2));
    return (&x)[i];
  }

  constexpr bool operator==(const Vec2 &) const = default;
  constexpr auto operator<=>(const Vec2 &) const = default;

  constexpr Vec2 operator-() const requires (std::signed_integral<T> || std::floating_point<T>)
    { return { -x, -y }; }

  constexpr Vec2 operator+(Vec2 other) const
    { return { x + other.x, y + other.y }; }

  constexpr Vec2 operator-(Vec2 other) const
    { return { x - other.x, y - other.y }; }

  constexpr Vec2 operator*(T v) const
    { return { x * v, y * v }; }

  constexpr Vec2 operator*(Vec2 v) const
    { return { x * v.x, y * v.y }; }

  constexpr Vec2 operator/(T v) const
    { return { x / v, y / v }; }

  constexpr Vec2 operator/(Vec2 v) const
    { return { x / v.x, y / v.y }; }

  constexpr Vec2 &operator+=(Vec2 v)
    { *this = *this + v; return *this; }

  constexpr Vec2 &operator-=(Vec2 v)
    { *this = *this - v; return *this; }

  constexpr Vec2 &operator*=(T v)
    { *this = *this * v; return *this; }

  constexpr Vec2 &operator*=(Vec2 v)
    { *this = *this * v; return *this; }

  constexpr Vec2 &operator/=(T v)
    { *this = *this / v; return *this; }

  constexpr Vec2 &operator/=(Vec2 v)
    { *this = *this / v; return *this; }

  [[nodiscard]] constexpr Vec2 RotateRight() const
    { return { -y, x }; }

  [[nodiscard]] constexpr Vec2 RotateLeft() const
    { return { y, -x }; }

  [[nodiscard]] constexpr Vec2 Rotate180() const
    { return { -x, -y }; }

  T x = T(0);
  T y = T(0);

  static consteval Vec2 XAxis() { return { 1, 0 }; }
  static consteval Vec2 YAxis() { return { 0, 1 }; }
};



export template <numeric T>
class Vec3
{
public:
  constexpr Vec3() noexcept = default;
  constexpr Vec3(T xv, T yv, T zv)
    : x(xv)
    , y(yv)
    , z(zv)
    { }

  explicit constexpr Vec3(T v)
    : x(v)
    , y(v)
    , z(v)
    { }

  T LengthSqr() const
    { return x * x + y * y + z * z; }

  T Length() const requires (std::floating_point<T>)
    { return std::sqrt(LengthSqr()); }

  T MinComponent() const
    { return std::min({x, y, z}); }

  T MaxComponent() const
    { return std::max({x, y, z}); }

  T &operator[](ssz i)
  {
    assert(InRangeArray(i, 3));
    return (&x)[i];
  }

  T operator[](ssz i) const
  {
    assert(InRangeArray(i, 3));
    return (&x)[i];
  }

  constexpr bool operator==(const Vec3 &) const = default;
  constexpr auto operator<=>(const Vec3 &) const = default;

  Vec3 operator-() const requires (std::signed_integral<T> || std::floating_point<T>)
    { return { -x, -y, -z }; }

  Vec3 operator+(Vec3 other) const
    { return { x + other.x, y + other.y, z + other.z }; }

  Vec3 operator-(Vec3 other) const
    { return { x - other.x, y - other.y, z - other.z }; }

  Vec3 operator*(T v) const
    { return { x * v, y * v, z * v }; }

  Vec3 operator*(Vec3 v) const
    { return { x * v.x, y * v.y, z * v.z }; }

  Vec3 operator/(T v) const
    { return { x / v, y / v, z / v }; }

  Vec3 operator/(Vec3 v) const
    { return { x / v.x, y / v.y, z / v.z }; }

  Vec3 &operator+=(Vec3 v)
    { *this = *this + v; return *this; }

  Vec3 &operator-=(Vec3 v)
    { *this = *this - v; return *this; }

  Vec3 &operator*=(T v)
    { *this = *this * v; return *this; }

  Vec3 &operator*=(Vec3 v)
    { *this = *this * v; return *this; }

  Vec3 &operator/=(T v)
    { *this = *this / v; return *this; }

  Vec3 &operator/=(Vec3 v)
    { *this = *this / v; return *this; }

  T x = T(0);
  T y = T(0);
  T z = T(0);
};


export template <numeric T>
class Vec4
{
public:
  constexpr Vec4() noexcept = default;
  constexpr Vec4(T xv, T yv, T zv, T wv)
    : x(xv)
    , y(yv)
    , z(zv)
    , w(wv)
    { }

  explicit constexpr Vec4(T v)
    : x(v)
    , y(v)
    , z(v)
    , w(v)
    { }

  T LengthSqr() const
    { return x * x + y * y + z * z + w * w; }

  T Length() const requires (std::floating_point<T>)
    { return std::sqrt(LengthSqr()); }

  T MinComponent() const
    { return std::min({x, y, z, w}); }

  T MaxComponent() const
    { return std::max({x, y, z, w}); }

  T &operator[](ssz i)
  {
    assert(InRangeArray(i, 4));
    return (&x)[i];
  }

  T operator[](ssz i) const
  {
    assert(InRangeArray(i, 4));
    return (&x)[i];
  }

  constexpr bool operator==(const Vec4 &) const = default;
  constexpr auto operator<=>(const Vec4 &) const = default;

  Vec4 operator-() const requires (std::signed_integral<T> || std::floating_point<T>)
    { return { -x, -y, -z, -w }; }

  Vec4 operator+(Vec4 other) const
    { return { x + other.x, y + other.y, z + other.z, w + other.w }; }

  Vec4 operator-(Vec4 other) const
    { return { x - other.x, y - other.y, z - other.z, w - other.w }; }

  Vec4 operator*(T v) const
    { return { x * v, y * v, z * v, w * v }; }

  Vec4 operator*(Vec4 v) const
    { return { x * v.x, y * v.y, z * v.z, w * v.z }; }

  Vec4 operator/(T v) const
    { return { x / v, y / v, z / v, w / v }; }

  Vec4 operator/(Vec4 v) const
    { return { x / v.x, y / v.y, z / v.z, w / v.w }; }

  Vec4 &operator+=(Vec4 v)
    { *this = *this + v; return *this; }

  Vec4 &operator-=(Vec4 v)
    { *this = *this - v; return *this; }

  Vec4 &operator*=(T v)
    { *this = *this * v; return *this; }

  Vec4 &operator*=(Vec4 v)
    { *this = *this * v; return *this; }

  Vec4 &operator/=(T v)
    { *this = *this / v; return *this; }

  Vec4 &operator/=(Vec4 v)
    { *this = *this / v; return *this; }

  T x = T(0);
  T y = T(0);
  T z = T(0);
  T w = T(0);

  static constexpr Vec4 Zero { 0, 0, 0, 0 };
  static constexpr Vec4 UnitX { 1, 0, 0, 0 };
  static constexpr Vec4 UnitY { 0, 1, 0, 0 };
  static constexpr Vec4 UnitZ { 0, 0, 1, 0 };
  static constexpr Vec4 UnitW { 0, 0, 0, 1 };
};


export template <std::floating_point T>
inline Vec2<T> Normalize(Vec2<T> v)
  { return v / v.Length(); }

export template <numeric T>
inline Vec2<T> Dot(Vec2<T> a, Vec2<T> b)
  { return a.x * b.x + a.y * b.y; }

export template <std::floating_point T>
inline Vec2<T> Lerp(ValueRange<Vec2<T>> r, T u)
  { return r.a * (T(1.0) - u) + r.b * u; }


export template <std::floating_point T>
inline Vec3<T> Normalize(Vec3<T> v)
  { return v / v.Length(); }

export template <numeric T>
inline T Dot(Vec3<T> a, Vec3<T> b)
  { return a.x * b.x + a.y * b.y + a.z * b.z; }

export template <std::floating_point T>
inline Vec3<T> Lerp(ValueRange<Vec3<T>> r, T u)
  { return r.a * (T(1.0) - u) + r.b * u; }

export template <std::floating_point T>
inline Vec4<T> Normalize(Vec4<T> v)
  { return v / v.Length(); }

export template <numeric T>
inline Vec4<T> Dot(Vec4<T> a, Vec4<T> b)
  { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

export template <std::floating_point T>
inline Vec4<T> Lerp(ValueRange<Vec4<T>> r, T u)
  { return r.a * (T(1.0) - u) + r.b * u; }


using Vec2F32 = Vec2<f32>;
using Vec3F32 = Vec3<f32>;
using Vec4F32 = Vec4<f32>;

using Vec2F64 = Vec2<f64>;
using Vec3F64 = Vec3<f64>;
using Vec4F64 = Vec4<f64>;

using Vec2S32 = Vec2<s32>;
using Vec3S32 = Vec3<s32>;
using Vec4S32 = Vec4<s32>;

using Vec2S64 = Vec2<s64>;
using Vec3S64 = Vec3<s64>;
using Vec4S64 = Vec4<s64>;


export template <numeric T, ssz N>
class VecN
{
public:
  constexpr VecN() noexcept = default;

  constexpr VecN(const std::initializer_list<T> &l)
  {
    assert(l.size() == usz(N));
    const T *inT = l.begin();

    for (ssz i = 0; i < N; i++)
      { data[i] = inT[i]; }
  }

  T data[N];

  T LengthSqr() const
  {
    T sum = 0;
    for (ssz i = 0; i < N; i++)
      { sum += data[i] * data[i]; }
    return sum;
  }

  T Length() const requires (std::floating_point<T>)
    { return std::sqrt(LengthSqr()); }

  T MinComponent() const
    { return std::ranges::min_element(data); }

  T MaxComponent() const
    { return std::ranges::max_element(data); }

  T &operator[](ssz i)
  {
    assert(InRangeArray(i, N));
    return data[i];
  }

  T operator[](ssz i) const
  {
    assert(InRangeArray(i, N));
    return data[i];
  }

  constexpr bool operator==(const VecN &) const = default;
  constexpr auto operator<=>(const VecN &) const = default;

  VecN operator-() const requires (std::signed_integral<T> || std::floating_point<T>)
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = -data[i]; }
    return o;
  }

  VecN operator+(VecN other) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] + other.data[i]; }
    return o;
  }

  VecN operator-(VecN other) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] - other.data[i]; }
    return o;
  }

  VecN operator*(T v) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] * v; }
    return o;
  }

  VecN operator*(VecN v) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] + v.data[i]; }
    return o;
  }

  VecN operator/(T v) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] / v; }
    return o;
  }

  VecN operator/(VecN v) const
  {
    VecN o;
    for (ssz i = 0; i < N; i++)
      { o[i] = data[i] / v.data[i]; }
    return o;
  }

  VecN &operator+=(VecN v)
    { *this = *this + v; return *this; }

  VecN &operator-=(VecN v)
    { *this = *this - v; return *this; }

  VecN &operator*=(T v)
    { *this = *this * v; return *this; }

  VecN &operator*=(VecN v)
    { *this = *this * v; return *this; }

  VecN &operator/=(T v)
    { *this = *this / v; return *this; }

  VecN &operator/=(VecN v)
    { *this = *this / v; return *this; }
};

export template <numeric T, ssz N>
class Vec : public VecN<T, N>
  { using VecN<T, N>::VecN; };

export template <numeric T>
class Vec<T, 4> : public Vec4<T>
  { using Vec4<T>::Vec4; };

export template <numeric T>
class Vec<T, 3> : public Vec3<T>
  { using Vec3<T>::Vec3; };

export template <numeric T>
class Vec<T, 2> : public Vec2<T>
  { using Vec2<T>::Vec2; };

extern "C++"
{
  export template<numeric T>
  struct std::hash<Vec2<T>>
  {
    std::size_t operator()(const Vec2<T> &v) const noexcept
    {
      return std::hash<T>{}(v.x) ^ std::hash<T>{}(v.y);
    }
  };
}