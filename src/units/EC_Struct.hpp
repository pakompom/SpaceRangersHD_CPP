#pragma once
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"

namespace EC_Struct {
    extern pas::Object* StartupCleanupObject;

    TVector3D MakeVector3D(double X, double Y, double Z);

    TPointF MakePointF(float X, float Y);

    Types::TPoint TruncatePointF(TPointF Point);

    Types::TPoint RoundPointF(TPointF Point);

    TPointF PointToPointF(Types::TPoint Point);

    Types::TPoint HalfPoint(Types::TPoint Point);

    Types::TPoint AddPoints(Types::TPoint Left, Types::TPoint Right);

    Types::TPoint SubtractPoints(Types::TPoint Left, Types::TPoint Right);

    TPointF HalfPointF(TPointF Point);

    TPointF AddPointsF(TPointF Left, TPointF Right);

    TPointF SubtractPointsF(TPointF Left, TPointF Right);

    inline std::uint8_t IntersectRects(Types::TRect& Intersection, const Types::TRect& First, const Types::TRect& Second);

    void TObjectEx_Create(TObjectEx* Self);

    void TObjectEx_Destroy(TObjectEx* Self);

} // namespace EC_Struct

#include "inline/EC_Struct.hpp"
