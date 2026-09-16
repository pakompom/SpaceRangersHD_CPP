#pragma once
#include "types/ab_Global.hpp"
#include "types/ab_StopLine.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace ab_StopLine {
    extern std::uint32_t StopPointHeap;

    extern ab_StopLine::PabStopPoint FirstStopPoint;

    extern ab_StopLine::PabStopPoint LastStopPoint;

    extern ab_StopLine::PabStopPoint SelectedStopPoint;

    extern std::uint32_t StopLineHeap;

    extern ab_StopLine::PabStopLine FirstStopLine;

    extern ab_StopLine::PabStopLine LastStopLine;

    extern ab_StopLine::PabStopLine SelectedStopLine;

    extern ab_StopLine::PabStopLine FirstCollisionLine;

    extern pas::DynArray<ab_StopLine::PabStopPoint> StopPointIndex;

    void ab_StopPoint_Clear();

    // Allocates and links a node owned by the world list.
    PabStopPoint ab_StopPoint_Add();

    void ab_StopPoint_Delete(PabStopPoint Point);

    // Nil updates every point.
    void ab_StopPoint_UpdatePosition(PabStopPoint Point);

    void ab_StopPoint_ClearImages();

    void ab_StopPoint_ClearSegments(PabStopPoint Point);

    void ab_StopPoint_BuildIndex();

    void ab_StopPoint_ClearIndex();

    std::int32_t ab_StopPoint_Count();

    void ab_StopLine_Clear();

    // Allocates and links a node owned by the world list.
    PabStopLine ab_StopLine_Add();

    void ab_StopLine_Delete(PabStopLine Line);

    void ab_StopLine_AddLatitude(double PolarAngle, double Step);

    void ab_StopLine_UpdateWorldLines();

    void ab_StopLine_UpdateColors();

    void ab_StopLine_ClearSegments(PabStopLine Line);

    // Empty in this native version.
    void ab_StopLine_PrepareCollision(PabStopLine Line);

    void ab_StopLine_BuildCollisionList();

    std::uint8_t ab_StopLine_ReflectMovement(ab_Global::TSphericalBearingState Source, ab_Global::TSphericalBearingState Target, double& HeadingDelta, double& Speed, double& UnusedResult);

    void ab_StopLine_GetDistances(ab_Global::TSphericalBearingState Source, double& ForwardDistance, double& BackwardDistance);

    std::uint8_t ab_StopLine_IsBlocked(double SourceLongitude, double SourcePolarAngle, double TargetLongitude, double TargetPolarAngle);

    void ab_StopLine_Load(EC_Buf::TBufEC* Buffer);

} // namespace ab_StopLine
