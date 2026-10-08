#pragma once
#include "units/ab_Global.hpp"

namespace ab_Global {
    inline TSphericalBearingState MakeSphericalBearingState(double LongitudeDegrees, double PolarAngleDegrees,
        double BearingDegrees) {
        TSphericalBearingState Result{};
        Result.LongitudeDegrees = LongitudeDegrees;
        Result.PolarAngleDegrees = PolarAngleDegrees;
        Result.BearingDegrees = BearingDegrees;
        return Result;
    }

    // Compares against the horizon depth set by UpdateSphereProjectionMetrics.
    inline std::uint8_t IsDepthBeforeSphereHorizon(double ProjectedDepth) {
        return ProjectedDepth < SphereHorizonDepth;
    }

} // namespace ab_Global
