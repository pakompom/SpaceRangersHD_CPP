#include "types/Types.hpp"
#include "units/TypesImports.hpp"

namespace TypesImports {
    std::uint8_t PtInRect(const Types::TRect& Rect, const Types::TPoint& P) {
        return P.X >= Rect.Left && P.X < Rect.Right && P.Y >= Rect.Top && P.Y < Rect.Bottom;
    }

} // namespace TypesImports
