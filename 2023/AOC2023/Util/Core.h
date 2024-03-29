#pragma once

#include <algorithm>
#include <atomic>
#include <bit>
#include <cctype>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>



#pragma warning (push)
#pragma warning (disable: 4623 4626 4625 5026 5027)
#include <mutex>
#pragma warning (pop)

#include <type_traits>
#include <utility>

constexpr bool InRangeInclusive(auto v, auto minV, auto maxV)
  { return v >= minV && v <= maxV; }

constexpr bool InRangeArray(auto v, auto count)
  { return v >= 0 && v < count; }

constexpr bool IsPowerOfTwo(std::integral auto v)
  { return (v & (v - 1)) == 0; }

#include "Util/Types.h"
#include "Util/Debug.h"

#include "Util/Containers/ArrayView.h"
#include "Util/Containers/ResizeableArray.h"
#include "Util/Containers/BoundedArray.h"
#include "Util/Containers/UnboundedArray.h"
#include "Util/Containers/FixedArray.h"

#include "Util/Math/MathHelper.h"
#include "Util/Math/Vec.h"
#include "Util/Math/Mat.h"
#include "Util/Timer.h"
#include "Util/Math/Interval.h"
#include "Util/Math/Polygons.h"
