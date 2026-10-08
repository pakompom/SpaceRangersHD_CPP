#include "layout/ab_Global.hpp"
#include "types/EC_Buf.hpp"
#include "types/SE_Process.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"

// Native math contribution:.
// Native PACKAGEINFO visits ab_Global from the arcade geometry family.
// Shared globals retain direct accesses.
namespace ab_Global {
    // Shared camera/transition state; native TfAB accesses it through an external reference cell. Original unit unresolved.
    std::uint8_t ArcadeViewMode = 0;

    // Shared scene process; native TfAB accesses it through external reference. Original unit unresolved.
    SE_Process::TProcessSE* ArcadeSpaceProcess = nullptr;

    std::int32_t ArcadeTickCount{};

    std::int32_t ArcadeFrameCount{};

    // Second dword of the native abwm map header.
    std::uint32_t ArcadeMapVersion{};

    // Shared view state; original data ownership unresolved.
    ab_Global::TSphericalBearingState SphereViewState{};

    ab_Global::TMatrix4D SphereViewMatrix{};

    ab_Global::TMatrix4D SpherePerspectiveMatrix{};

    // Shared projection matrix via reference cell; original data ownership unresolved.
    ab_Global::TMatrix4D SphereProjectionMatrix{};

    Types::TPoint ArcadeMapViewPosition{};

    Types::TPoint ArcadeMapCenter{};

    Types::TRect ArcadeMapBounds{};

    std::int32_t ArcadeGridMode{};

    // Loaded by the arcade map reader; stop lines and triangles borrow color pointers into Data.
    EC_Buf::TBufEC* ArcadeMapColorBuffer{};

    std::uint8_t ArcadeAutopilotEnabled{};

    std::uint8_t ArcadeEnemiesDefeated{};

    std::int32_t ArcadeLastInputTick{};

    double SphereRadius = 1.0E+3;

    double SphereCameraDistance = 2.3E+3;

    double SphereNearCameraOffset = 1.3E+3;

    double SphereFarCameraOffset = 2.0E+4;

    double SphereFieldOfView = 88.0;

    // Maximum camera travel per tick; doubled for Keller dialogue framing.
    double CameraFollowStep = 14.0;

    // Forward offset from the player when following.
    double CameraLookAheadDistance = 0.0;

    float PlayerDriftTurnStep = 1.8f;

    // Set on the player ship during TfAB.OnOpen.
    float PlayerInitialTurnSpeed = 3.3f;

    float PlayerFastTurnSpeed = 2.5f;

    float PlayerSlowTurnSpeed = 3.8f;

    float SphereLowSpeedDrag = 0.007f;

    float SphereHighSpeedDrag = 0.18f;

    std::int32_t ArcadeMapNodeRadius = 60;

    std::int32_t ArcadeMapPanMargin = 200;

    // Heading increment, straight-path spacing and endpoint snap distance.
    float ArcadePathStep = 4.0f;

    // Initial spacing along turning arcs.
    float ArcadePathArcStep = 4.0f;

    float SphereProjectedRadius = 1.0f;

    float SphereNearHorizonDepth = -1.0f;

    float SphereHorizonDepth = 0.0f;

    float SphereFarHorizonDepth = 1.0f;

    float ShipFrontDepth = 2.0E+1f;

    float ShipBackDepth = 3.0E+1f;

    float ShipTailFrontDepth = 21.0f;

    float ShipTailBackDepth = 31.0f;

    float ItemFrontDepth = 21.0f;

    float HitFrontDepth = 19.0f;

    float HitBackDepth = 29.0f;

    float WorldImageFrontDepth = 22.0f;

    float WorldImageBackDepth = 28.0f;

    float ExplosionFrontDepth = 18.0f;

    float ExplosionBackDepth = 28.0f;

    // Six colors for each of six difficulty appearances.
    pas::Array<std::uint32_t, 0, 35> ArcadeMapPalette = pas::Array<std::uint32_t, 0, 35>{{0xff28ac00u, 0x8028ac00u,
        0xc028ac00u, 0x0028ac00u, 0xc028ac00u, 0xc0ffffffu, 0xff003cffu, 0x80003cffu, 0xc0003cffu, 0x00003cffu,
        0xc0003cffu, 0xc0ffffffu, 0xffffff00u, 0x80ffff00u, 0xc0ffff00u, 0x00ffff00u, 0xc0ffff00u, 0xc0ffffffu,
        0xffffa636u, 0x80ffa636u, 0xc0ffa636u, 0x00ffa636u, 0xc0ffa636u, 0xc0ffffffu, 0xffc80000u, 0x80c80000u,
        0xc0c80000u, 0x00c80000u, 0xc0c80000u, 0xc0ffffffu, 0xffa6002bu, 0x80a6002bu, 0xc0a6002bu, 0x00a6002bu,
        0xc0a6002bu, 0xc0ffffffu}};

    pas::Array<std::int32_t, 0, 5> BonusRespawnSeconds = pas::Array<std::int32_t, 0, 5>{{40, 60, 80, 100, 100, 150}};

    pas::Array<std::int32_t, 0, 7> BonusDurationSeconds = pas::Array<std::int32_t, 0, 7>{{12, 50, 40, 50, 40, 50, 60,
        60}};

    std::int32_t RegenerationHealthPerTick = 1;

    float SpeedBonusScale = 1.2f;

    float SpeedPenaltyScale = 0.7f;

    float WeaponDamageBonusScale = 1.5f;

    float AmmoRechargeBonusScale = 2.0f;

    float ShieldDamageScale = 0.4f;

    float OtherInvisibleAlpha = 0.2f;

    float PlayerInvisibleAlpha = 0.5f;

    std::int32_t RevealAfterFiringMs = 5500;

    std::int32_t WeaponSwitchDelayMs = 1200;

    float ArcadeHighDangerThreshold = 7.0E+1f;

    float ManualCargoPickupDistance = 2.0E+2f;

    float CargoPickupDistance = 8.0E+1f;

    EC_Struct::TVector3D SphericalToVector3D(double LongitudeRadians, double PolarAngleRadians, double Radius) {
        EC_Struct::TVector3D Result{};
        EC_Struct::TVector3D V{};
        V.X = static_cast<float>(System::Sin(PolarAngleRadians) * Radius);
        V.Y = static_cast<float>(-System::Cos(PolarAngleRadians) * Radius);
        V.Z = 0.0;
        Result.X = static_cast<float>(System::Sin(LongitudeRadians) * V.X);
        Result.Y = V.Y;
        Result.Z = static_cast<float>(-System::Cos(LongitudeRadians) * V.X);
        return Result;
    }

    // Requires a nonzero vector.
    void VectorToSphericalAngles(EC_Struct::TVector3D Vector, double& LongitudeDegrees, double& PolarAngleDegrees) {
        double Radius = System::Sqrt(static_cast<float>(static_cast<float>(pas::sqr(static_cast<double>(Vector.X)) + pas::sqr(static_cast<double>(Vector.Y))) +
            pas::sqr(static_cast<double>(Vector.Z))));
        PolarAngleDegrees = aMyFunction::RadiansToHeadingDegrees(MathImports::ArcCos(static_cast<float>(static_cast<double>(-Vector.Y) /
                Radius)));
        LongitudeDegrees = aMyFunction::RadiansToHeadingDegrees(static_cast<float>(SystemImports::Pi - Math::ArcTan2(Vector.X, Vector.Z)));
    }

    // Negative distance moves backward. Longitude and bearing pass through Single precision when wrapped.
    void AdvanceSphericalBearingState(pas::Var<double> LongitudeDegrees, pas::Var<double> PolarAngleDegrees,
        pas::Var<double> BearingDegrees, double SphereRadius, double ArcDistance) {
        double InvSin{};
        std::uint8_t Reverse{};
        if (ArcDistance < 0.0) {
            Reverse = true;
            ArcDistance = -ArcDistance;
            pas::store_unaligned<double>(BearingDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(static_cast<float>(pas::load_unaligned<double>(BearingDegrees.address) + 1.8E+2))));
        } else {
            Reverse = false;
        }
        double OldBearing = aMyFunction::HeadingDegreesToRadians(pas::load_unaligned<double>(BearingDegrees.address));
        double ArcAngle = static_cast<float>(static_cast<float>(static_cast<float>(ArcDistance / (static_cast<float>(pas::constant(2.0 * SystemImports::Pi) * SphereRadius))) * SystemImports::Pi) *
            2.0);
        double OldPolar = aMyFunction::HeadingDegreesToRadians(pas::load_unaligned<double>(PolarAngleDegrees.address));
        double NewPolar = MathImports::ArcCos(static_cast<float>(static_cast<float>(System::Cos(OldPolar) * System::Cos(ArcAngle)) + static_cast<float>(static_cast<float>(System::Sin(OldPolar) *
                System::Sin(ArcAngle)) * System::Cos(OldBearing))));
        if (NewPolar < 1.0E-5) {
            InvSin = 99999999.0;
        } else {
            InvSin = static_cast<float>(1.0 / System::Sin(NewPolar));
        }
        double Value = static_cast<float>((static_cast<float>(static_cast<float>(System::Sin(OldPolar) * System::Cos(ArcAngle)) - static_cast<float>(static_cast<float>(System::Cos(OldPolar) * System::Sin(ArcAngle)) *
                System::Cos(OldBearing)))) * InvSin);
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double LongitudeDelta = MathImports::ArcCos(Value);
        Value = static_cast<float>((static_cast<float>(static_cast<float>(System::Sin(ArcAngle) * System::Cos(OldPolar)) - static_cast<float>(static_cast<float>(System::Cos(ArcAngle) * System::Sin(OldPolar)) *
                    System::Cos(OldBearing)))) * InvSin);
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double NewBearing = MathImports::ArcCos(Value);
        if (pas::load_unaligned<double>(BearingDegrees.address) > 1.8E+2) {
            pas::store_unaligned<double>(BearingDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(aMyFunction::RadiansToHeadingDegrees(static_cast<float>(SystemImports::Pi +
                        NewBearing)))));
            pas::store_unaligned<double>(LongitudeDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(
                        static_cast<float>(static_cast<double>(pas::load_unaligned<double>(LongitudeDegrees.address)) -
                        aMyFunction::RadiansToHeadingDegrees(LongitudeDelta)))));
        } else {
            pas::store_unaligned<double>(BearingDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(aMyFunction::RadiansToHeadingDegrees(static_cast<float>(SystemImports::Pi -
                        NewBearing)))));
            pas::store_unaligned<double>(LongitudeDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(
                        static_cast<float>(static_cast<double>(pas::load_unaligned<double>(LongitudeDegrees.address)) +
                        aMyFunction::RadiansToHeadingDegrees(LongitudeDelta)))));
        }
        pas::store_unaligned<double>(PolarAngleDegrees.address, aMyFunction::RadiansToHeadingDegrees(NewPolar));
        if (Reverse) {
            pas::store_unaligned<double>(BearingDegrees.address,
                static_cast<double>(
                    aMyFunction::WrapHeadingDegrees(static_cast<float>(pas::load_unaligned<double>(BearingDegrees.address) + 1.8E+2))));
        }
    }

    TSphericalBearingState AdvanceSphericalStateOnCurrentSphere(TSphericalBearingState Source, double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&Result.LongitudeDegrees),
            pas::Var<double>(&Result.PolarAngleDegrees), pas::Var<double>(&Result.BearingDegrees), SphereRadius,
            ArcDistance);
        return Result;
    }

    // Uses the current sphere radius; preserves the body's bearing relative to travel.
    TSphericalBearingState AdvanceSphericalStateAlongBearing(TSphericalBearingState Source, double TravelBearingDegrees,
        double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        double RelativeBearing = aMyFunction::HeadingDifferenceDegrees(TravelBearingDegrees, Result.BearingDegrees);
        ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&Result.LongitudeDegrees),
            pas::Var<double>(&Result.PolarAngleDegrees), pas::Var<double>(&TravelBearingDegrees), SphereRadius,
            ArcDistance);
        Result.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearingDegrees) +
                RelativeBearing));
        return Result;
    }

    // Uses the current sphere radius; updates travel bearing and preserves the body's bearing relative to it.
    TSphericalBearingState AdvanceSphericalStateAndTravelBearing(TSphericalBearingState Source,
        double& TravelBearingDegrees, double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        double RelativeBearing = aMyFunction::HeadingDifferenceDegrees(TravelBearingDegrees, Result.BearingDegrees);
        ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&Result.LongitudeDegrees),
            pas::Var<double>(&Result.PolarAngleDegrees), pas::Var<double>(&TravelBearingDegrees), SphereRadius,
            ArcDistance);
        Result.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TravelBearingDegrees) +
                RelativeBearing));
        return Result;
    }

    // Bearing is relative to SourceBearingDegrees; coincident points return zero bearing delta and distance.
    void ComputeSphericalBearingAndDistance(pas::Var<double> BearingDeltaDegrees, pas::Var<double> Distance,
        double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double SourceBearingDegrees,
        double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius) {
        double TargetPolar = aMyFunction::HeadingDegreesToRadians(TargetPolarAngleDegrees);
        double SourcePolar = aMyFunction::HeadingDegreesToRadians(SourcePolarAngleDegrees);
        double LongitudeDelta = aMyFunction::HeadingDegreesToRadians(
            aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TargetLongitudeDegrees) - SourceLongitudeDegrees)));
        double Value = static_cast<float>(static_cast<float>(System::Cos(TargetPolar) * System::Cos(SourcePolar)) + static_cast<float>(static_cast<float>(System::Sin(TargetPolar) *
                System::Sin(SourcePolar)) * System::Cos(LongitudeDelta)));
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double ArcAngle = MathImports::ArcCos(Value);
        pas::store_unaligned<double>(Distance.address,
            static_cast<double>(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(ArcAngle / pas::constant(2.0 * SystemImports::Pi)) * 2.0) * SystemImports::Pi) *
                SphereRadius)));
        if (ArcAngle == 0.0) {
            pas::store_unaligned<double>(BearingDeltaDegrees.address, 0.0);
            return;
        }
        Value = static_cast<float>((static_cast<float>(static_cast<float>(System::Sin(SourcePolar) * System::Cos(TargetPolar)) - static_cast<float>(static_cast<float>(System::Cos(SourcePolar) *
                    System::Sin(TargetPolar)) * System::Cos(LongitudeDelta)))) / System::Sin(ArcAngle));
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double Bearing = MathImports::ArcCos(Value);
        if (aMyFunction::HeadingDifferenceDegrees(SourceLongitudeDegrees, TargetLongitudeDegrees) < 0.0) {
            Bearing = -Bearing;
        }
        pas::store_unaligned<double>(BearingDeltaDegrees.address,
            aMyFunction::HeadingDifferenceDegrees(SourceBearingDegrees, aMyFunction::RadiansToHeadingDegrees(Bearing)));
    }

    void ComputeSphericalDistance(double& Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees,
        double UnusedSourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees,
        double SphereRadius) {
        double TargetPolar = aMyFunction::HeadingDegreesToRadians(TargetPolarAngleDegrees);
        double SourcePolar = aMyFunction::HeadingDegreesToRadians(SourcePolarAngleDegrees);
        double LongitudeDelta = aMyFunction::HeadingDegreesToRadians(
            aMyFunction::WrapHeadingDegrees(static_cast<float>(static_cast<double>(TargetLongitudeDegrees) - SourceLongitudeDegrees)));
        double Value = static_cast<float>(static_cast<float>(System::Cos(TargetPolar) * System::Cos(SourcePolar)) + static_cast<float>(static_cast<float>(System::Sin(TargetPolar) *
                System::Sin(SourcePolar)) * System::Cos(LongitudeDelta)));
        if (Value < -1.0) {
            Value = -1.0;
        } else if (Value > 1.0) {
            Value = 1.0;
        }
        double ArcAngle = MathImports::ArcCos(Value);
        Distance = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(ArcAngle / pas::constant(2.0 * SystemImports::Pi)) * 2.0) * SystemImports::Pi) * SphereRadius);
    }

    // Uses the current sphere radius; ignores Target.BearingDegrees.
    TSphericalBearingDistance GetSphericalBearingAndDistance(TSphericalBearingState Source,
        TSphericalBearingState Target) {
        TSphericalBearingDistance Result{};
        ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Result.BearingDeltaDegrees),
            pas::Var<double>(&Result.Distance), Source.LongitudeDegrees, Source.PolarAngleDegrees,
            Source.BearingDegrees, Target.LongitudeDegrees, Target.PolarAngleDegrees, SphereRadius);
        return Result;
    }

    // Uses the shared sphere radius, camera distance, field of view and projection scale.
    void UpdateSphereProjectionMetrics() {
        EC_Struct::TVector3D V{};
        EC_Struct::TVector3D Target{};
        EC_Struct::TVector3D Up{};
        TMatrix4D View{};
        TMatrix4D Projection{};
        TMatrix4D Combined{};
        V = EC_Struct::MakeVector3D(0.0, 0.0, SphereCameraDistance);
        Target = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
        Up = EC_Struct::MakeVector3D(0.0, 1.0, 0.0);
        View = ab_Global::BuildLookAtMatrix(V, Target, Up);
        Projection = ab_Global::BuildPerspectiveProjectionMatrix(
                static_cast<float>(static_cast<float>(static_cast<double>(SphereCameraDistance) - SphereRadius) - 1.0E+2),
                static_cast<float>(static_cast<float>(static_cast<double>(SphereCameraDistance) + SphereRadius) + 1.0E+2),
                aMyFunction::HeadingDegreesToRadians(SphereFieldOfView),
                static_cast<std::uint32_t>(GR_Main::GameScreenWidth));
        Combined = ab_Global::MultiplyMatrix4D(Projection, View);
        double Angle = static_cast<float>(9.0E+1 - (static_cast<float>(9.0E+1 -
                aMyFunction::RadiansToHeadingDegrees(MathImports::ArcSin(static_cast<float>(static_cast<double>(SphereRadius) /
                    SphereCameraDistance))))));
        V = EC_Struct::MakeVector3D(0.0, 0.0, static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * SphereRadius));
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(0.0, 0.0,
                static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(static_cast<float>(Angle - 15.0))) * SphereRadius));
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereNearHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(0.0, 0.0,
                static_cast<float>(System::Sin(aMyFunction::HeadingDegreesToRadians(static_cast<float>(Angle + 15.0))) * SphereRadius));
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereFarHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(SphereRadius, 0.0, 0.0);
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereProjectedRadius = pas::real_max<double>(std::fabs(static_cast<double>(V.X)),
                std::fabs(static_cast<double>(V.Y)));
    }

    // Requires a nonzero vector.
    EC_Struct::TVector3D NormalizeVector3D(const EC_Struct::TVector3D& Source) {
        EC_Struct::TVector3D Result{};
        double Scale = static_cast<float>(1.0 / System::Sqrt(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Source.X) * Source.X) + static_cast<float>(static_cast<double>(Source.Y) *
                    Source.Y)) + static_cast<float>(static_cast<double>(Source.Z) * Source.Z))));
        Result.X = static_cast<float>(static_cast<double>(Source.X) * Scale);
        Result.Y = static_cast<float>(static_cast<double>(Source.Y) * Scale);
        Result.Z = static_cast<float>(static_cast<double>(Source.Z) * Scale);
        return Result;
    }

    EC_Struct::TVector3D CrossProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B) {
        EC_Struct::TVector3D Result{};
        Result.X = static_cast<float>(static_cast<float>(static_cast<double>(A.Y) * B.Z) - static_cast<float>(static_cast<double>(A.Z) * B.Y));
        Result.Y = static_cast<float>(static_cast<float>(static_cast<double>(A.Z) * B.X) - static_cast<float>(static_cast<double>(A.X) * B.Z));
        Result.Z = static_cast<float>(static_cast<float>(static_cast<double>(A.X) * B.Y) - static_cast<float>(static_cast<double>(A.Y) * B.X));
        return Result;
    }

    double DotProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B) {
        return static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(A.X) * B.X) + static_cast<float>(static_cast<double>(A.Y) * B.Y)) + static_cast<float>(static_cast<double>(A.Z) * B.Z));
    }

    void ClearMatrix4D(TMatrix4D& Matrix) {
        std::int32_t I{};
        std::int32_t J{};
        for (I = 0; I <= 3; ++I) {
            for (J = 0; J <= 3; ++J) {
                Matrix[I][J] = 0.0;
            }
        }
    }

    void SetIdentityMatrix4D(TMatrix4D& Matrix) {
        Matrix[0][0] = 1.0;
        Matrix[1][0] = 0.0;
        Matrix[2][0] = 0.0;
        Matrix[3][0] = 0.0;
        Matrix[0][1] = 0.0;
        Matrix[1][1] = 1.0;
        Matrix[2][1] = 0.0;
        Matrix[3][1] = 0.0;
        Matrix[0][2] = 0.0;
        Matrix[1][2] = 0.0;
        Matrix[2][2] = 1.0;
        Matrix[3][2] = 0.0;
        Matrix[0][3] = 0.0;
        Matrix[1][3] = 0.0;
        Matrix[2][3] = 0.0;
        Matrix[3][3] = 1.0;
    }

    // Rotates clockwise in the XY plane for positive angles.
    TMatrix4D BuildZAxisRotationMatrix(double AngleRadians) {
        TMatrix4D Result{};
        double C = System::Cos(AngleRadians);
        double S = System::Sin(AngleRadians);
        ab_Global::SetIdentityMatrix4D(Result);
        Result[0][0] = C;
        Result[1][1] = C;
        Result[0][1] = -S;
        Result[1][0] = S;
        return Result;
    }

    // Uses the same scale for X and Y; projects NearPlane to depth 0 and FarPlane to depth 1.
    TMatrix4D BuildPerspectiveProjectionMatrix(double NearPlane, double FarPlane, double FovRadians,
        double ProjectionScale) {
        TMatrix4D Result{};
        double C = System::Cos(static_cast<float>(FovRadians * 0.5));
        double S = System::Sin(static_cast<float>(FovRadians * 0.5));
        double Q = static_cast<float>(S / (static_cast<float>(1.0 - static_cast<float>(static_cast<double>(NearPlane) / FarPlane))));
        ab_Global::ClearMatrix4D(Result);
        Result[0][0] = static_cast<float>(static_cast<double>(C) * ProjectionScale);
        Result[1][1] = static_cast<float>(static_cast<double>(C) * ProjectionScale);
        Result[2][2] = Q;
        Result[3][2] = static_cast<float>(static_cast<double>(-Q) * NearPlane);
        Result[2][3] = S;
        return Result;
    }

    // CameraPos must differ from TargetPos; UpVector must not be parallel to the viewing direction.
    TMatrix4D BuildLookAtMatrix(const EC_Struct::TVector3D& CameraPos, const EC_Struct::TVector3D& TargetPos,
        const EC_Struct::TVector3D& UpVector) {
        TMatrix4D Result{};
        EC_Struct::TVector3D Forward{};
        EC_Struct::TVector3D Right{};
        EC_Struct::TVector3D Up{};
        ab_Global::SetIdentityMatrix4D(Result);
        Forward = EC_Struct::MakeVector3D(static_cast<float>(static_cast<double>(TargetPos.X) - CameraPos.X),
                static_cast<float>(static_cast<double>(TargetPos.Y) - CameraPos.Y), static_cast<float>(static_cast<double>(TargetPos.Z) - CameraPos.Z));
        Forward = ab_Global::NormalizeVector3D(Forward);
        Right = ab_Global::CrossProduct3D(UpVector, Forward);
        Up = ab_Global::CrossProduct3D(Forward, Right);
        Right = ab_Global::NormalizeVector3D(Right);
        Up = ab_Global::NormalizeVector3D(Up);
        Result[0][0] = Right.X;
        Result[1][0] = Right.Y;
        Result[2][0] = Right.Z;
        Result[0][1] = Up.X;
        Result[1][1] = Up.Y;
        Result[2][1] = Up.Z;
        Result[0][2] = Forward.X;
        Result[1][2] = Forward.Y;
        Result[2][2] = Forward.Z;
        Result[3][0] = -ab_Global::DotProduct3D(Right, CameraPos);
        Result[3][1] = -ab_Global::DotProduct3D(Up, CameraPos);
        Result[3][2] = -ab_Global::DotProduct3D(Forward, CameraPos);
        return Result;
    }

    // Does not report singularity; zero pivots are replaced by 1e-20.
    TMatrix4D InvertMatrix4D(const TMatrix4D& Matrix) {
        TMatrix4D Result{};
        pas::Array<std::int32_t, 0, 3> Permutations{};
        pas::Array<double, 0, 3> Solution{};
        std::int32_t I{};
        std::int32_t J{};
        double PermutationSign{};
        TMatrix4D Factors{};
        auto SolveMatrix4DLuSystem = [&](const TMatrix4D& Factors) -> void {
            std::int32_t I{};
            std::int32_t J{};
            std::int32_t Pivot{};
            double Sum{};
            std::int32_t FirstNonzero = -1;
            for (I = 0; I <= 3; ++I) {
                Pivot = Permutations[I];
                Sum = Solution[Pivot];
                Solution[Pivot] = Solution[I];
                if (FirstNonzero >= 0) {
                    for (auto cpp_range = pas::for_to<std::int32_t>(FirstNonzero, I - 1); cpp_range.next(J); ) {
                        Sum = static_cast<float>(Sum - static_cast<float>(static_cast<double>(Factors[I][J]) * Solution[J]));
                    }
                } else if (Sum != 0.0) {
                    FirstNonzero = I;
                }
                Solution[I] = Sum;
            }
            for (I = 3; I >= 0; --I) {
                Sum = Solution[I];
                {
                    const std::int32_t cpp_first = I + 1;
                    if (cpp_first <= 3) {
                        for (J = cpp_first; J <= 3; ++J) {
                            Sum = static_cast<float>(Sum - static_cast<float>(static_cast<double>(Factors[I][J]) * Solution[J]));
                        }
                    }
                }
                Solution[I] = static_cast<float>(static_cast<double>(Sum) / Factors[I][I]);
            }
        };
        auto DecomposeMatrix4DLu = [&](TMatrix4D& Matrix, double& PermutationSign) -> void {
            double Big{};
            double Temp{};
            double Sum{};
            double Magnitude{};
            std::int32_t I{};
            std::int32_t Pivot{};
            std::int32_t J{};
            std::int32_t K{};
            pas::Array<double, 0, 3> Scales{};
            PermutationSign = 1.0;
            for (I = 0; I <= 3; ++I) {
                Big = 0.0;
                for (J = 0; J <= 3; ++J) {
                    Magnitude = std::fabs(static_cast<double>(Matrix[I][J]));
                    if (Magnitude > Big) {
                        Big = Magnitude;
                    }
                }
                Scales[I] = static_cast<float>(1.0 / Big);
            }
            for (J = 0; J <= 3; ++J) {
                I = 0;
                while (I < J) {
                    Sum = Matrix[I][J];
                    K = 0;
                    while (K < I) {
                        Sum = static_cast<float>(Sum - static_cast<float>(static_cast<double>(Matrix[I][K]) * Matrix[K][J]));
                        ++K;
                    }
                    Matrix[I][J] = Sum;
                    ++I;
                }
                Pivot = 0;
                Big = 0.0;
                {
                    const std::int32_t cpp_first = J;
                    if (cpp_first <= 3) {
                        for (I = cpp_first; I <= 3; ++I) {
                            Sum = Matrix[I][J];
                            K = 0;
                            while (K < J) {
                                Sum = static_cast<float>(Sum - static_cast<float>(static_cast<double>(Matrix[I][K]) * Matrix[K][J]));
                                ++K;
                            }
                            Matrix[I][J] = Sum;
                            Temp = static_cast<float>(std::fabs(static_cast<double>(Sum)) * Scales[I]);
                            if (Temp >= Big) {
                                Big = Temp;
                                Pivot = I;
                            }
                        }
                    }
                }
                if (J != Pivot) {
                    for (K = 0; K <= 3; ++K) {
                        Temp = Matrix[Pivot][K];
                        Matrix[Pivot][K] = Matrix[J][K];
                        Matrix[J][K] = Temp;
                    }
                    PermutationSign = -PermutationSign;
                    Scales[Pivot] = Scales[J];
                }
                Permutations[J] = Pivot;
                if (Matrix[J][J] == 0.0) {
                    Matrix[J][J] = 1.0E-20;
                }
                if (J != 3) {
                    Temp = static_cast<float>(1.0 / Matrix[J][J]);
                    {
                        const std::int32_t cpp_first_2 = J + 1;
                        if (cpp_first_2 <= 3) {
                            for (I = cpp_first_2; I <= 3; ++I) {
                                Matrix[I][J] = static_cast<float>(static_cast<double>(Matrix[I][J]) * Temp);
                            }
                        }
                    }
                }
            }
        };
        Factors = Matrix;
        DecomposeMatrix4DLu(Factors, PermutationSign);
        for (J = 0; J <= 3; ++J) {
            for (I = 0; I <= 3; ++I) {
                Solution[I] = 0.0;
            }
            Solution[J] = 1.0;
            SolveMatrix4DLuSystem(Factors);
            for (I = 0; I <= 3; ++I) {
                Result[I][J] = Solution[I];
            }
        }
        return Result;
    }

    TMatrix4D MultiplyMatrix4D(const TMatrix4D& Left, const TMatrix4D& Right) {
        TMatrix4D Result{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        ab_Global::ClearMatrix4D(Result);
        for (I = 0; I <= 3; ++I) {
            for (J = 0; J <= 3; ++J) {
                for (K = 0; K <= 3; ++K) {
                    Result[I][J] = static_cast<float>(static_cast<float>(static_cast<double>(Left[K][J]) * Right[I][K]) + Result[I][J]);
                }
            }
        }
        return Result;
    }

    // Includes perspective division; homogeneous W must be nonzero.
    EC_Struct::TVector3D ProjectPointByMatrix(const TMatrix4D& Matrix, pas::ConstRef<EC_Struct::TVector3D> Source) {
        EC_Struct::TVector3D Result{};
        double X{};
        double Y{};
        double Z{};
        double ReciprocalW{};
        // Manual x87 port. Keep extended intermediates and the native Z summation order.
        X = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).X;
        Y = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).Y;
        Z = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).Z;
        ReciprocalW = static_cast<float>(1.0 / (static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(Matrix[0][3] * X) + static_cast<float>(Matrix[1][3] * Y)) + static_cast<float>(Matrix[2][3] * Z)) + Matrix[3][3])));
        Result.X = static_cast<float>((static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(Matrix[0][0] * X) + static_cast<float>(Matrix[1][0] * Y)) + static_cast<float>(Matrix[2][0] * Z)) + Matrix[3][0])) * ReciprocalW);
        Result.Y = static_cast<float>((static_cast<float>(static_cast<float>(static_cast<float>(static_cast<float>(Matrix[0][1] * X) + static_cast<float>(Matrix[1][1] * Y)) + static_cast<float>(Matrix[2][1] * Z)) + Matrix[3][1])) * ReciprocalW);
        Result.Z = static_cast<float>((static_cast<float>(static_cast<float>(Matrix[1][2] * Y) + (static_cast<float>(static_cast<float>(Matrix[0][2] * X) + (static_cast<float>(static_cast<float>(Matrix[2][2] * Z) + Matrix[3][2])))))) * ReciprocalW);
        return Result;
    }

    // Ray points must differ. Rejects tangency. May write HitPoint on false; true requires forward distance greater than 0.001.
    std::uint8_t TryIntersectRayWithSphere(EC_Struct::TVector3D RayOrigin, EC_Struct::TVector3D RayPointOnRay,
        EC_Struct::TVector3D SphereCenter, double SphereRadius, EC_Struct::TVector3D& HitPoint) {
        double OtherT{};
        EC_Struct::TVector3D Direction{};
        EC_Struct::TVector3D CenterDelta{};
        Direction.X = static_cast<float>(static_cast<double>(RayPointOnRay.X) - RayOrigin.X);
        Direction.Y = static_cast<float>(static_cast<double>(RayPointOnRay.Y) - RayOrigin.Y);
        Direction.Z = static_cast<float>(static_cast<double>(RayPointOnRay.Z) - RayOrigin.Z);
        double T = static_cast<float>(1.0 / System::Sqrt(static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(Direction.X) * Direction.X) +
                static_cast<float>(static_cast<double>(Direction.Y) * Direction.Y)) + static_cast<float>(static_cast<double>(Direction.Z) * Direction.Z))));
        Direction.X = static_cast<float>(static_cast<double>(Direction.X) * T);
        Direction.Y = static_cast<float>(static_cast<double>(Direction.Y) * T);
        Direction.Z = static_cast<float>(static_cast<double>(Direction.Z) * T);
        CenterDelta.X = static_cast<float>(static_cast<double>(SphereCenter.X) - RayOrigin.X);
        CenterDelta.Y = static_cast<float>(static_cast<double>(SphereCenter.Y) - RayOrigin.Y);
        CenterDelta.Z = static_cast<float>(static_cast<double>(SphereCenter.Z) - RayOrigin.Z);
        double DistanceSquared = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(CenterDelta.X) * CenterDelta.X) +
            static_cast<float>(static_cast<double>(CenterDelta.Y) * CenterDelta.Y)) + static_cast<float>(static_cast<double>(CenterDelta.Z) * CenterDelta.Z));
        double Projection = static_cast<float>(static_cast<float>(static_cast<float>(static_cast<double>(CenterDelta.X) * Direction.X) + static_cast<float>(static_cast<double>(CenterDelta.Y) *
                Direction.Y)) + static_cast<float>(static_cast<double>(CenterDelta.Z) * Direction.Z));
        double Discriminant = static_cast<float>(static_cast<float>(pas::sqr(static_cast<double>(SphereRadius)) - DistanceSquared) +
            static_cast<float>(static_cast<double>(Projection) * Projection));
        if (Discriminant <= 0.0) {
            return false;
        }
        Discriminant = System::Sqrt(Discriminant);
        if (Projection < Discriminant) {
            T = static_cast<float>(static_cast<double>(Projection) + Discriminant);
            OtherT = static_cast<float>(static_cast<double>(Projection) - Discriminant);
        } else {
            T = static_cast<float>(static_cast<double>(Projection) - Discriminant);
            OtherT = static_cast<float>(static_cast<double>(Projection) + Discriminant);
        }
        if (std::fabs(static_cast<double>(T)) < 0.001) {
            T = OtherT;
        }
        HitPoint.X = static_cast<float>(static_cast<float>(static_cast<double>(Direction.X) * T) + RayOrigin.X);
        HitPoint.Y = static_cast<float>(static_cast<float>(static_cast<double>(Direction.Y) * T) + RayOrigin.Y);
        HitPoint.Z = static_cast<float>(static_cast<float>(static_cast<double>(Direction.Z) * T) + RayOrigin.Z);
        return T > 0.001;
    }

} // namespace ab_Global
