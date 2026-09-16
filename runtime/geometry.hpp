#pragma once
// Semantic port of EC_Struct.IntersectRects. Preserve the original write order
// and leave the destination unchanged when the rectangles do not overlap.
#include <algorithm>
namespace pas {
template <class Rect>
bool intersect_rects(Rect &out, const Rect &first, const Rect &second) {
  if (second.Left >= first.Right || second.Right <= first.Left ||
      second.Top >= first.Bottom || second.Bottom <= first.Top)
    return false;
  out.Left = std::max(second.Left, first.Left);
  out.Right = std::min(second.Right, first.Right);
  out.Top = std::max(second.Top, first.Top);
  out.Bottom = std::min(second.Bottom, first.Bottom);
  return true;
}
} // namespace pas
