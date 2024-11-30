#pragma once

template <numeric T>
T AreaOf2DPolygon(ArrayView<Vec2<T>> poly)
{
  T doubleSignedArea = 0;
  for (ssz i = 0; i < poly.Count(); i++)
    { doubleSignedArea += poly[i].x * (poly[Wrapped(i + 1)].y - poly[Wrapped(i - 1)].y); }

  return std::abs(doubleSignedArea / 2);
}


// A "grid polygon" is my name for an integral-vertex polygon with only 90/180/270 degree angles.

// Get the length of the perimeter of the polygon (this also doubles as the number of points that are on the border
//  for Pick's-Theorem-related calculations)
// For polygon:{ (0, 0), (5, 0), (5, 2), (3, 2), (3, 4), (0, 4) }
//      XXXXXX
//      XooooX
//      XooXXX
//      XooX
//      XXXX
// It returns 18, the count of the "X"s in the diagram
template <std::integral T>
T GridPolygonPerimeter(ArrayView<Vec2<T>> poly)
{
  T length = 0;
  for (s32 i = 0; i < poly.Count(); i++)
  {
    auto p = poly[i];
    auto n = poly[Wrapped(i + 1)];
    ASSERT(p.x == n.x || p.y == n.y); // Must be the case to be a grid polygon
    length += std::abs(p.x - n.x + p.y - n.y);
  }

  return length;
}


// Calculate how many points are contained within the border (not counting points *on* the border) for instance:
// For polygon:{ (0, 0), (5, 0), (5, 2), (3, 2), (3, 4), (0, 4) }
//      XXXXXX
//      XooooX
//      XooXXX
//      XooX
//      XXXX
// This would return 8, the count the "o"s in the diagram.
template <std::integral T>
T GridPolygonContainedPointCount(ArrayView<Vec2<T>> poly)
{
  // Use Pick's Theorem to calculate how many points are contained
  return AreaOf2DPolygon(poly) - GridPolygonPerimeter(poly) / 2 + 1;
}


// Calculate how many points are inside of or on the border of the grid polygon.
// For polygon:{ (0, 0), (5, 0), (5, 2), (3, 2), (3, 4), (0, 4) }
//      XXXXXX
//      XooooX
//      XooXXX
//      XooX
//      XXXX
// It returns 26, the count of both "X"s and "o"s in the polygon.
template <std::integral T>
T GridPolygonAreaIncludingPerimeter(ArrayView<Vec2<T>> poly)
  { return AreaOf2DPolygon(poly) + GridPolygonPerimeter(poly) / 2 + 1; }