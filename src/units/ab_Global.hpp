#pragma once
#include "types/EC_Struct.hpp"
#include "types/ab_Global.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace SE_Process {
    struct TProcessSE;

} // namespace SE_Process

namespace Types {
    struct TPoint;

    struct TRect;

} // namespace Types

namespace ab_Global {
    // Shared camera/transition state; native TfAB accesses it through an external reference cell. Original unit unresolved.
    extern std::uint8_t ArcadeViewMode;

    // Shared scene process; native TfAB accesses it through external reference. Original unit unresolved.
    extern SE_Process::TProcessSE* ArcadeSpaceProcess;

    extern std::int32_t ArcadeTickCount;

    extern std::int32_t ArcadeFrameCount;

    // Second dword of the native abwm map header.
    extern std::uint32_t ArcadeMapVersion;

    // Shared view state; original data ownership unresolved.
    extern ab_Global::TSphericalBearingState SphereViewState;

    extern ab_Global::TMatrix4D SphereViewMatrix;

    extern ab_Global::TMatrix4D SpherePerspectiveMatrix;

    // Shared projection matrix via reference cell; original data ownership unresolved.
    extern ab_Global::TMatrix4D SphereProjectionMatrix;

    extern Types::TPoint ArcadeMapViewPosition;

    extern Types::TPoint ArcadeMapCenter;

    extern Types::TRect ArcadeMapBounds;

    extern std::int32_t ArcadeGridMode;

    // Loaded by the arcade map reader; stop lines and triangles borrow color pointers into Data.
    extern EC_Buf::TBufEC* ArcadeMapColorBuffer;

    extern std::uint8_t ArcadeAutopilotEnabled;

    extern std::uint8_t ArcadeEnemiesDefeated;

    extern std::int32_t ArcadeLastInputTick;

    extern double SphereRadius;

    extern double SphereCameraDistance;

    extern double SphereNearCameraOffset;

    extern double SphereFarCameraOffset;

    extern double SphereFieldOfView;

    // Maximum camera travel per tick; doubled for Keller dialogue framing.
    extern double CameraFollowStep;

    // Forward offset from the player when following.
    extern double CameraLookAheadDistance;

    extern float PlayerDriftTurnStep;

    // Set on the player ship during TfAB.OnOpen.
    extern float PlayerInitialTurnSpeed;

    extern float PlayerFastTurnSpeed;

    extern float PlayerSlowTurnSpeed;

    extern float SphereLowSpeedDrag;

    extern float SphereHighSpeedDrag;

    extern std::int32_t ArcadeMapNodeRadius;

    extern std::int32_t ArcadeMapPanMargin;

    // Heading increment, straight-path spacing and endpoint snap distance.
    extern float ArcadePathStep;

    // Initial spacing along turning arcs.
    extern float ArcadePathArcStep;

    extern float SphereProjectedRadius;

    extern float SphereNearHorizonDepth;

    extern float SphereHorizonDepth;

    extern float SphereFarHorizonDepth;

    extern float ShipFrontDepth;

    extern float ShipBackDepth;

    extern float ShipTailFrontDepth;

    extern float ShipTailBackDepth;

    extern float ItemFrontDepth;

    extern float HitFrontDepth;

    extern float HitBackDepth;

    extern float WorldImageFrontDepth;

    extern float WorldImageBackDepth;

    extern float ExplosionFrontDepth;

    extern float ExplosionBackDepth;

    // Six colors for each of six difficulty appearances.
    extern pas::Array<std::uint32_t, 0, 35> ArcadeMapPalette;

    extern pas::Array<std::int32_t, 0, 5> BonusRespawnSeconds;

    extern pas::Array<std::int32_t, 0, 7> BonusDurationSeconds;

    extern std::int32_t RegenerationHealthPerTick;

    extern float SpeedBonusScale;

    extern float SpeedPenaltyScale;

    extern float WeaponDamageBonusScale;

    extern float AmmoRechargeBonusScale;

    extern float ShieldDamageScale;

    extern float OtherInvisibleAlpha;

    extern float PlayerInvisibleAlpha;

    extern std::int32_t RevealAfterFiringMs;

    extern std::int32_t WeaponSwitchDelayMs;

    extern float ArcadeHighDangerThreshold;

    extern float ManualCargoPickupDistance;

    extern float CargoPickupDistance;

    TSphericalBearingState MakeSphericalBearingState(double LongitudeDegrees, double PolarAngleDegrees, double BearingDegrees);

    EC_Struct::TVector3D SphericalToVector3D(double LongitudeRadians, double PolarAngleRadians, double Radius);

    // Requires a nonzero vector.
    void VectorToSphericalAngles(EC_Struct::TVector3D Vector, double& LongitudeDegrees, double& PolarAngleDegrees);

    // Negative distance moves backward. Longitude and bearing pass through Single precision when wrapped.
    void AdvanceSphericalBearingState(pas::Var<double> LongitudeDegrees, pas::Var<double> PolarAngleDegrees, pas::Var<double> BearingDegrees, double SphereRadius, double ArcDistance);

    TSphericalBearingState AdvanceSphericalStateOnCurrentSphere(TSphericalBearingState Source, double ArcDistance);

    // Uses the current sphere radius; preserves the body's bearing relative to travel.
    TSphericalBearingState AdvanceSphericalStateAlongBearing(TSphericalBearingState Source, double TravelBearingDegrees, double ArcDistance);

    // Uses the current sphere radius; updates travel bearing and preserves the body's bearing relative to it.
    TSphericalBearingState AdvanceSphericalStateAndTravelBearing(TSphericalBearingState Source, double& TravelBearingDegrees, double ArcDistance);

    // Bearing is relative to SourceBearingDegrees; coincident points return zero bearing delta and distance.
    void ComputeSphericalBearingAndDistance(pas::Var<double> BearingDeltaDegrees, pas::Var<double> Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double SourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius);

    void ComputeSphericalDistance(double& Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double UnusedSourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius);

    // Uses the current sphere radius; ignores Target.BearingDegrees.
    TSphericalBearingDistance GetSphericalBearingAndDistance(TSphericalBearingState Source, TSphericalBearingState Target);

    // Uses the shared sphere radius, camera distance, field of view and projection scale.
    void UpdateSphereProjectionMetrics();

    // Compares against the horizon depth set by UpdateSphereProjectionMetrics.
    std::uint8_t IsDepthBeforeSphereHorizon(double ProjectedDepth);

    // Requires a nonzero vector.
    EC_Struct::TVector3D NormalizeVector3D(const EC_Struct::TVector3D& Source);

    EC_Struct::TVector3D CrossProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B);

    double DotProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B);

    void ClearMatrix4D(TMatrix4D& Matrix);

    void SetIdentityMatrix4D(TMatrix4D& Matrix);

    // Rotates clockwise in the XY plane for positive angles.
    TMatrix4D BuildZAxisRotationMatrix(double AngleRadians);

    // Uses the same scale for X and Y; projects NearPlane to depth 0 and FarPlane to depth 1.
    TMatrix4D BuildPerspectiveProjectionMatrix(double NearPlane, double FarPlane, double FovRadians, double ProjectionScale);

    // CameraPos must differ from TargetPos; UpVector must not be parallel to the viewing direction.
    TMatrix4D BuildLookAtMatrix(const EC_Struct::TVector3D& CameraPos, const EC_Struct::TVector3D& TargetPos, const EC_Struct::TVector3D& UpVector);

    // Does not report singularity; zero pivots are replaced by 1e-20.
    TMatrix4D InvertMatrix4D(const TMatrix4D& Matrix);

    TMatrix4D MultiplyMatrix4D(const TMatrix4D& Left, const TMatrix4D& Right);

    // Includes perspective division; homogeneous W must be nonzero.
    EC_Struct::TVector3D ProjectPointByMatrix(const TMatrix4D& Matrix, pas::ConstRef<EC_Struct::TVector3D> Source);

    // Ray points must differ. Rejects tangency. May write HitPoint on false; true requires forward distance greater than 0.001.
    std::uint8_t TryIntersectRayWithSphere(EC_Struct::TVector3D RayOrigin, EC_Struct::TVector3D RayPointOnRay, EC_Struct::TVector3D SphereCenter, double SphereRadius, EC_Struct::TVector3D& HitPoint);

} // namespace ab_Global
