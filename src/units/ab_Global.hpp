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
    extern std::uint8_t ArcadeViewMode;

    extern SE_Process::TProcessSE* ArcadeSpaceProcess;

    extern std::int32_t ArcadeTickCount;

    extern std::int32_t ArcadeFrameCount;

    extern std::uint32_t ArcadeMapVersion;

    extern ab_Global::TSphericalBearingState SphereViewState;

    extern ab_Global::TMatrix4D SphereViewMatrix;

    extern ab_Global::TMatrix4D SpherePerspectiveMatrix;

    extern ab_Global::TMatrix4D SphereProjectionMatrix;

    extern Types::TPoint ArcadeMapViewPosition;

    extern Types::TPoint ArcadeMapCenter;

    extern Types::TRect ArcadeMapBounds;

    extern std::int32_t ArcadeGridMode;

    extern EC_Buf::TBufEC* ArcadeMapColorBuffer;

    extern std::uint8_t ArcadeAutopilotEnabled;

    extern std::uint8_t ArcadeEnemiesDefeated;

    extern std::int32_t ArcadeLastInputTick;

    extern double SphereRadius;

    extern double SphereCameraDistance;

    extern double SphereNearCameraOffset;

    extern double SphereFarCameraOffset;

    extern double SphereFieldOfView;

    extern double CameraFollowStep;

    extern double CameraLookAheadDistance;

    extern float PlayerDriftTurnStep;

    extern float PlayerInitialTurnSpeed;

    extern float PlayerFastTurnSpeed;

    extern float PlayerSlowTurnSpeed;

    extern float SphereLowSpeedDrag;

    extern float SphereHighSpeedDrag;

    extern std::int32_t ArcadeMapNodeRadius;

    extern std::int32_t ArcadeMapPanMargin;

    extern float ArcadePathStep;

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

    void VectorToSphericalAngles(EC_Struct::TVector3D Vector, double& LongitudeDegrees, double& PolarAngleDegrees);

    void AdvanceSphericalBearingState(double& LongitudeDegrees, double& PolarAngleDegrees, double& BearingDegrees, double SphereRadius, double ArcDistance);

    TSphericalBearingState AdvanceSphericalStateOnCurrentSphere(TSphericalBearingState Source, double ArcDistance);

    TSphericalBearingState AdvanceSphericalStateAlongBearing(TSphericalBearingState Source, double TravelBearingDegrees, double ArcDistance);

    TSphericalBearingState AdvanceSphericalStateAndTravelBearing(TSphericalBearingState Source, double& TravelBearingDegrees, double ArcDistance);

    void ComputeSphericalBearingAndDistance(double& BearingDeltaDegrees, pas::Var<double> Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double SourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius);

    void ComputeSphericalDistance(double& Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double UnusedSourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius);

    TSphericalBearingDistance GetSphericalBearingAndDistance(TSphericalBearingState Source, TSphericalBearingState Target);

    void UpdateSphereProjectionMetrics();

    std::uint8_t IsDepthBeforeSphereHorizon(double ProjectedDepth);

    EC_Struct::TVector3D NormalizeVector3D(const EC_Struct::TVector3D& Source);

    EC_Struct::TVector3D CrossProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B);

    double DotProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B);

    void ClearMatrix4D(TMatrix4D& Matrix);

    void SetIdentityMatrix4D(TMatrix4D& Matrix);

    TMatrix4D BuildZAxisRotationMatrix(double AngleRadians);

    TMatrix4D BuildPerspectiveProjectionMatrix(double NearPlane, double FarPlane, double FovRadians, double ProjectionScale);

    TMatrix4D BuildLookAtMatrix(const EC_Struct::TVector3D& CameraPos, const EC_Struct::TVector3D& TargetPos, const EC_Struct::TVector3D& UpVector);

    TMatrix4D InvertMatrix4D(const TMatrix4D& Matrix);

    TMatrix4D MultiplyMatrix4D(const TMatrix4D& Left, const TMatrix4D& Right);

    EC_Struct::TVector3D ProjectPointByMatrix(const TMatrix4D& Matrix, pas::ConstRef<EC_Struct::TVector3D> Source);

    std::uint8_t TryIntersectRayWithSphere(EC_Struct::TVector3D RayOrigin, EC_Struct::TVector3D RayPointOnRay, EC_Struct::TVector3D SphereCenter, double SphereRadius, EC_Struct::TVector3D& HitPoint);

} // namespace ab_Global
