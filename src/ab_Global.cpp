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

namespace ab_Global {
    void SolveMatrix4DLuSystem(const TMatrix4D& Factors, pas::Array<std::int32_t, 0, 3>& Permutations, pas::Array<double, 0, 3>& Solution);

    void DecomposeMatrix4DLu(TMatrix4D& Matrix, double& PermutationSign, pas::Array<std::int32_t, 0, 3>& Permutations);

    std::uint8_t ArcadeViewMode = 0;

    SE_Process::TProcessSE* ArcadeSpaceProcess = nullptr;

    std::int32_t ArcadeTickCount{};

    std::int32_t ArcadeFrameCount{};

    std::uint32_t ArcadeMapVersion{};

    ab_Global::TSphericalBearingState SphereViewState{};

    ab_Global::TMatrix4D SphereViewMatrix{};

    ab_Global::TMatrix4D SpherePerspectiveMatrix{};

    ab_Global::TMatrix4D SphereProjectionMatrix{};

    Types::TPoint ArcadeMapViewPosition{};

    Types::TPoint ArcadeMapCenter{};

    Types::TRect ArcadeMapBounds{};

    std::int32_t ArcadeGridMode{};

    EC_Buf::TBufEC* ArcadeMapColorBuffer{};

    std::uint8_t ArcadeAutopilotEnabled{};

    std::uint8_t ArcadeEnemiesDefeated{};

    std::int32_t ArcadeLastInputTick{};

    double SphereRadius = 1.0E+3;

    double SphereCameraDistance = 2.3E+3;

    double SphereNearCameraOffset = 1.3E+3;

    double SphereFarCameraOffset = 2.0E+4;

    double SphereFieldOfView = 88.0;

    double CameraFollowStep = 14.0;

    double CameraLookAheadDistance = 0.0;

    float PlayerDriftTurnStep = 1.8f;

    float PlayerInitialTurnSpeed = 3.3f;

    float PlayerFastTurnSpeed = 2.5f;

    float PlayerSlowTurnSpeed = 3.8f;

    float SphereLowSpeedDrag = 0.007f;

    float SphereHighSpeedDrag = 0.18f;

    std::int32_t ArcadeMapNodeRadius = 60;

    std::int32_t ArcadeMapPanMargin = 200;

    float ArcadePathStep = 4.0f;

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

    pas::Array<std::uint32_t, 0, 35> ArcadeMapPalette = pas::Array<std::uint32_t, 0, 35>{{
        0xff28ac00u, 0x8028ac00u, 0xc028ac00u, 0x0028ac00u, 0xc028ac00u, 0xc0ffffffu, 0xff003cffu, 0x80003cffu,
        0xc0003cffu, 0x00003cffu, 0xc0003cffu, 0xc0ffffffu, 0xffffff00u, 0x80ffff00u, 0xc0ffff00u, 0x00ffff00u,
        0xc0ffff00u, 0xc0ffffffu, 0xffffa636u, 0x80ffa636u, 0xc0ffa636u, 0x00ffa636u, 0xc0ffa636u, 0xc0ffffffu,
        0xffc80000u, 0x80c80000u, 0xc0c80000u, 0x00c80000u, 0xc0c80000u, 0xc0ffffffu, 0xffa6002bu, 0x80a6002bu,
        0xc0a6002bu, 0x00a6002bu, 0xc0a6002bu, 0xc0ffffffu,
    }};

    pas::Array<std::int32_t, 0, 5> BonusRespawnSeconds = pas::Array<std::int32_t, 0, 5>{{40, 60, 80, 100, 100, 150}};

    pas::Array<std::int32_t, 0, 7> BonusDurationSeconds = pas::Array<std::int32_t, 0, 7>{{12, 50, 40, 50, 40, 50, 60, 60}};

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

    TSphericalBearingState MakeSphericalBearingState(double LongitudeDegrees, double PolarAngleDegrees, double BearingDegrees) {
        TSphericalBearingState Result{};
        Result.LongitudeDegrees = LongitudeDegrees;
        Result.PolarAngleDegrees = PolarAngleDegrees;
        Result.BearingDegrees = BearingDegrees;
        return Result;
    }

    EC_Struct::TVector3D SphericalToVector3D(double LongitudeRadians, double PolarAngleRadians, double Radius) {
        EC_Struct::TVector3D Result{};
        EC_Struct::TVector3D V{};
        V.X = System::Sin(PolarAngleRadians) * Radius;
        V.Y = -System::Cos(PolarAngleRadians) * Radius;
        V.Z = 0.0;
        Result.X = System::Sin(LongitudeRadians) * V.X;
        Result.Y = V.Y;
        Result.Z = -System::Cos(LongitudeRadians) * V.X;
        return Result;
    }

    void VectorToSphericalAngles(EC_Struct::TVector3D Vector, double& LongitudeDegrees, double& PolarAngleDegrees) {
        double Radius = System::Sqrt(pas::sqr(static_cast<pas::Extended>(Vector.X)) + pas::sqr(static_cast<pas::Extended>(Vector.Y)) + pas::sqr(static_cast<pas::Extended>(Vector.Z)));
        PolarAngleDegrees = aMyFunction::RadiansToHeadingDegrees(MathImports::ArcCos(pas::real_divide(-Vector.Y, Radius)));
        LongitudeDegrees = aMyFunction::RadiansToHeadingDegrees(SystemImports::Pi - Math::ArcTan2(Vector.X, Vector.Z));
    }

    void AdvanceSphericalBearingState(double& LongitudeDegrees, double& PolarAngleDegrees, double& BearingDegrees, double SphereRadius, double ArcDistance) {
        double InvSin{};
        std::uint8_t Reverse{};
        if (ArcDistance < 0.0L) {
            Reverse = true;
            ArcDistance = -ArcDistance;
            BearingDegrees = aMyFunction::WrapHeadingDegrees(BearingDegrees + 1.8E+2L);
        } else {
            Reverse = false;
        }
        double OldBearing = aMyFunction::HeadingDegreesToRadians(BearingDegrees);
        double ArcAngle = pas::real_divide(ArcDistance, pas::constant(2.0L * SystemImports::Pi) * SphereRadius) * SystemImports::Pi * 2.0L;
        double OldPolar = aMyFunction::HeadingDegreesToRadians(PolarAngleDegrees);
        pas::Extended cpp_left_2 = System::Cos(OldPolar);
        pas::Extended cpp_left = cpp_left_2 * System::Cos(ArcAngle);
        pas::Extended cpp_left_4 = System::Sin(OldPolar);
        pas::Extended cpp_left_3 = cpp_left_4 * System::Sin(ArcAngle);
        double NewPolar = MathImports::ArcCos(cpp_left + cpp_left_3 * System::Cos(OldBearing));
        if (NewPolar < 1.0E-5L) {
            InvSin = 99999999.0;
        } else {
            InvSin = pas::real_divide(1.0L, System::Sin(NewPolar));
        }
        pas::Extended cpp_left_6 = System::Sin(OldPolar);
        pas::Extended cpp_left_5 = cpp_left_6 * System::Cos(ArcAngle);
        pas::Extended cpp_left_8 = System::Cos(OldPolar);
        pas::Extended cpp_left_7 = cpp_left_8 * System::Sin(ArcAngle);
        double Value = (cpp_left_5 - cpp_left_7 * System::Cos(OldBearing)) * InvSin;
        if (Value < -1.0L) {
            Value = -1.0;
        } else if (Value > 1.0L) {
            Value = 1.0;
        }
        double LongitudeDelta = MathImports::ArcCos(Value);
        {
            pas::Extended cpp_left_10 = System::Sin(ArcAngle);
            pas::Extended cpp_left_9 = cpp_left_10 * System::Cos(OldPolar);
            pas::Extended cpp_left_12 = System::Cos(ArcAngle);
            pas::Extended cpp_left_11 = cpp_left_12 * System::Sin(OldPolar);
            Value = (cpp_left_9 - cpp_left_11 * System::Cos(OldBearing)) * InvSin;
        }
        if (Value < -1.0L) {
            Value = -1.0;
        } else if (Value > 1.0L) {
            Value = 1.0;
        }
        double NewBearing = MathImports::ArcCos(Value);
        if (BearingDegrees > 1.8E+2L) {
            BearingDegrees = aMyFunction::WrapHeadingDegrees(aMyFunction::RadiansToHeadingDegrees(SystemImports::Pi + NewBearing));
            LongitudeDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(LongitudeDegrees) - aMyFunction::RadiansToHeadingDegrees(LongitudeDelta));
        } else {
            BearingDegrees = aMyFunction::WrapHeadingDegrees(aMyFunction::RadiansToHeadingDegrees(SystemImports::Pi - NewBearing));
            LongitudeDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(LongitudeDegrees) + aMyFunction::RadiansToHeadingDegrees(LongitudeDelta));
        }
        PolarAngleDegrees = aMyFunction::RadiansToHeadingDegrees(NewPolar);
        if (Reverse) {
            BearingDegrees = aMyFunction::WrapHeadingDegrees(BearingDegrees + 1.8E+2L);
        }
    }

    TSphericalBearingState AdvanceSphericalStateOnCurrentSphere(TSphericalBearingState Source, double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        ab_Global::AdvanceSphericalBearingState(Result.LongitudeDegrees, Result.PolarAngleDegrees, Result.BearingDegrees, SphereRadius, ArcDistance);
        return Result;
    }

    TSphericalBearingState AdvanceSphericalStateAlongBearing(TSphericalBearingState Source, double TravelBearingDegrees, double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        double RelativeBearing = aMyFunction::HeadingDifferenceDegrees(TravelBearingDegrees, Result.BearingDegrees);
        ab_Global::AdvanceSphericalBearingState(Result.LongitudeDegrees, Result.PolarAngleDegrees, TravelBearingDegrees, SphereRadius, ArcDistance);
        Result.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(TravelBearingDegrees) + RelativeBearing);
        return Result;
    }

    TSphericalBearingState AdvanceSphericalStateAndTravelBearing(TSphericalBearingState Source, double& TravelBearingDegrees, double ArcDistance) {
        TSphericalBearingState Result{};
        Result = Source;
        double RelativeBearing = aMyFunction::HeadingDifferenceDegrees(TravelBearingDegrees, Result.BearingDegrees);
        ab_Global::AdvanceSphericalBearingState(Result.LongitudeDegrees, Result.PolarAngleDegrees, TravelBearingDegrees, SphereRadius, ArcDistance);
        Result.BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(TravelBearingDegrees) + RelativeBearing);
        return Result;
    }

    void ComputeSphericalBearingAndDistance(double& BearingDeltaDegrees, pas::Var<double> Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double SourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius) {
        double TargetPolar = aMyFunction::HeadingDegreesToRadians(TargetPolarAngleDegrees);
        double SourcePolar = aMyFunction::HeadingDegreesToRadians(SourcePolarAngleDegrees);
        double LongitudeDelta = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(TargetLongitudeDegrees) - SourceLongitudeDegrees));
        pas::Extended cpp_left_2 = System::Cos(TargetPolar);
        pas::Extended cpp_left = cpp_left_2 * System::Cos(SourcePolar);
        pas::Extended cpp_left_4 = System::Sin(TargetPolar);
        pas::Extended cpp_left_3 = cpp_left_4 * System::Sin(SourcePolar);
        double Value = cpp_left + cpp_left_3 * System::Cos(LongitudeDelta);
        if (Value < -1.0L) {
            Value = -1.0;
        } else if (Value > 1.0L) {
            Value = 1.0;
        }
        double ArcAngle = MathImports::ArcCos(Value);
        pas::store_unaligned<double>(Distance.address, static_cast<double>(pas::real_divide(ArcAngle, pas::constant(2.0L * SystemImports::Pi)) * 2.0L * SystemImports::Pi * SphereRadius));
        if (ArcAngle == 0.0L) {
            BearingDeltaDegrees = 0.0;
            return;
        }
        {
            pas::Extended cpp_left_7 = System::Sin(SourcePolar);
            pas::Extended cpp_left_6 = cpp_left_7 * System::Cos(TargetPolar);
            pas::Extended cpp_left_9 = System::Cos(SourcePolar);
            pas::Extended cpp_left_8 = cpp_left_9 * System::Sin(TargetPolar);
            pas::Extended cpp_left_5 = cpp_left_6 - cpp_left_8 * System::Cos(LongitudeDelta);
            Value = pas::real_divide(cpp_left_5, System::Sin(ArcAngle));
        }
        if (Value < -1.0L) {
            Value = -1.0;
        } else if (Value > 1.0L) {
            Value = 1.0;
        }
        double Bearing = MathImports::ArcCos(Value);
        if (aMyFunction::HeadingDifferenceDegrees(SourceLongitudeDegrees, TargetLongitudeDegrees) < 0.0L) {
            Bearing = -Bearing;
        }
        BearingDeltaDegrees = aMyFunction::HeadingDifferenceDegrees(SourceBearingDegrees, aMyFunction::RadiansToHeadingDegrees(Bearing));
    }

    void ComputeSphericalDistance(double& Distance, double SourceLongitudeDegrees, double SourcePolarAngleDegrees, double UnusedSourceBearingDegrees, double TargetLongitudeDegrees, double TargetPolarAngleDegrees, double SphereRadius) {
        double TargetPolar = aMyFunction::HeadingDegreesToRadians(TargetPolarAngleDegrees);
        double SourcePolar = aMyFunction::HeadingDegreesToRadians(SourcePolarAngleDegrees);
        double LongitudeDelta = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(static_cast<long double>(TargetLongitudeDegrees) - SourceLongitudeDegrees));
        pas::Extended cpp_left_2 = System::Cos(TargetPolar);
        pas::Extended cpp_left = cpp_left_2 * System::Cos(SourcePolar);
        pas::Extended cpp_left_4 = System::Sin(TargetPolar);
        pas::Extended cpp_left_3 = cpp_left_4 * System::Sin(SourcePolar);
        double Value = cpp_left + cpp_left_3 * System::Cos(LongitudeDelta);
        if (Value < -1.0L) {
            Value = -1.0;
        } else if (Value > 1.0L) {
            Value = 1.0;
        }
        double ArcAngle = MathImports::ArcCos(Value);
        Distance = pas::real_divide(ArcAngle, pas::constant(2.0L * SystemImports::Pi)) * 2.0L * SystemImports::Pi * SphereRadius;
    }

    TSphericalBearingDistance GetSphericalBearingAndDistance(TSphericalBearingState Source, TSphericalBearingState Target) {
        TSphericalBearingDistance Result{};
        ab_Global::ComputeSphericalBearingAndDistance(Result.BearingDeltaDegrees, pas::Var<double>(&Result.Distance), Source.LongitudeDegrees, Source.PolarAngleDegrees, Source.BearingDegrees, Target.LongitudeDegrees, Target.PolarAngleDegrees, SphereRadius);
        return Result;
    }

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
        Projection = ab_Global::BuildPerspectiveProjectionMatrix(static_cast<long double>(SphereCameraDistance) - SphereRadius - 1.0E+2L, static_cast<long double>(SphereCameraDistance) + SphereRadius + 1.0E+2L, aMyFunction::HeadingDegreesToRadians(SphereFieldOfView), static_cast<std::uint32_t>(GR_Main::GameScreenWidth));
        Combined = ab_Global::MultiplyMatrix4D(Projection, View);
        double Angle = 9.0E+1L - (9.0E+1L - aMyFunction::RadiansToHeadingDegrees(MathImports::ArcSin(pas::real_divide(SphereRadius, SphereCameraDistance))));
        V = EC_Struct::MakeVector3D(0.0, 0.0, System::Sin(aMyFunction::HeadingDegreesToRadians(Angle)) * SphereRadius);
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(0.0, 0.0, System::Sin(aMyFunction::HeadingDegreesToRadians(Angle - 15.0L)) * SphereRadius);
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereNearHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(0.0, 0.0, System::Sin(aMyFunction::HeadingDegreesToRadians(Angle + 15.0L)) * SphereRadius);
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereFarHorizonDepth = V.Z;
        V = EC_Struct::MakeVector3D(SphereRadius, 0.0, 0.0);
        V = ab_Global::ProjectPointByMatrix(Combined, pas::ConstRef<EC_Struct::TVector3D>(&V));
        SphereProjectedRadius = pas::real_max<pas::Extended>(std::fabs(static_cast<pas::Extended>(V.X)), std::fabs(static_cast<pas::Extended>(V.Y)));
    }

    std::uint8_t IsDepthBeforeSphereHorizon(double ProjectedDepth) {
        return ProjectedDepth < SphereHorizonDepth;
    }

    EC_Struct::TVector3D NormalizeVector3D(const EC_Struct::TVector3D& Source) {
        EC_Struct::TVector3D Result{};
        double Scale = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Source.X) * Source.X + static_cast<long double>(Source.Y) * Source.Y + static_cast<long double>(Source.Z) * Source.Z));
        Result.X = static_cast<long double>(Source.X) * Scale;
        Result.Y = static_cast<long double>(Source.Y) * Scale;
        Result.Z = static_cast<long double>(Source.Z) * Scale;
        return Result;
    }

    EC_Struct::TVector3D CrossProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B) {
        EC_Struct::TVector3D Result{};
        Result.X = static_cast<long double>(A.Y) * B.Z - static_cast<long double>(A.Z) * B.Y;
        Result.Y = static_cast<long double>(A.Z) * B.X - static_cast<long double>(A.X) * B.Z;
        Result.Z = static_cast<long double>(A.X) * B.Y - static_cast<long double>(A.Y) * B.X;
        return Result;
    }

    double DotProduct3D(const EC_Struct::TVector3D& A, const EC_Struct::TVector3D& B) {
        return static_cast<long double>(A.X) * B.X + static_cast<long double>(A.Y) * B.Y + static_cast<long double>(A.Z) * B.Z;
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

    TMatrix4D BuildPerspectiveProjectionMatrix(double NearPlane, double FarPlane, double FovRadians, double ProjectionScale) {
        TMatrix4D Result{};
        double C = System::Cos(FovRadians * 0.5L);
        double S = System::Sin(FovRadians * 0.5L);
        double Q = pas::real_divide(S, 1.0L - pas::real_divide(NearPlane, FarPlane));
        ab_Global::ClearMatrix4D(Result);
        Result[0][0] = static_cast<long double>(C) * ProjectionScale;
        Result[1][1] = static_cast<long double>(C) * ProjectionScale;
        Result[2][2] = Q;
        Result[3][2] = static_cast<long double>(-Q) * NearPlane;
        Result[2][3] = S;
        return Result;
    }

    TMatrix4D BuildLookAtMatrix(const EC_Struct::TVector3D& CameraPos, const EC_Struct::TVector3D& TargetPos, const EC_Struct::TVector3D& UpVector) {
        TMatrix4D Result{};
        EC_Struct::TVector3D Forward{};
        EC_Struct::TVector3D Right{};
        EC_Struct::TVector3D Up{};
        ab_Global::SetIdentityMatrix4D(Result);
        Forward = EC_Struct::MakeVector3D(static_cast<long double>(TargetPos.X) - CameraPos.X, static_cast<long double>(TargetPos.Y) - CameraPos.Y, static_cast<long double>(TargetPos.Z) - CameraPos.Z);
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

    TMatrix4D InvertMatrix4D(const TMatrix4D& Matrix) {
        TMatrix4D Result{};
        pas::Array<std::int32_t, 0, 3> Permutations{};
        pas::Array<double, 0, 3> Solution{};
        std::int32_t I{};
        std::int32_t J{};
        double PermutationSign{};
        TMatrix4D Factors{};
        Factors = Matrix;
        ab_Global::DecomposeMatrix4DLu(Factors, PermutationSign, Permutations);
        for (J = 0; J <= 3; ++J) {
            for (I = 0; I <= 3; ++I) {
                Solution[I] = 0.0;
            }
            Solution[J] = 1.0;
            ab_Global::SolveMatrix4DLuSystem(Factors, Permutations, Solution);
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
                    Result[I][J] = static_cast<long double>(Left[K][J]) * Right[I][K] + Result[I][J];
                }
            }
        }
        return Result;
    }

    EC_Struct::TVector3D ProjectPointByMatrix(const TMatrix4D& Matrix, pas::ConstRef<EC_Struct::TVector3D> Source) {
        EC_Struct::TVector3D Result{};
        pas::Extended X{};
        pas::Extended Y{};
        pas::Extended Z{};
        pas::Extended ReciprocalW{};
        X = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).X;
        Y = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).Y;
        Z = pas::load_unaligned<EC_Struct::TVector3D>(Source.address).Z;
        ReciprocalW = pas::real_divide(1.0L, Matrix[0][3] * X + Matrix[1][3] * Y + Matrix[2][3] * Z + Matrix[3][3]);
        Result.X = (Matrix[0][0] * X + Matrix[1][0] * Y + Matrix[2][0] * Z + Matrix[3][0]) * ReciprocalW;
        Result.Y = (Matrix[0][1] * X + Matrix[1][1] * Y + Matrix[2][1] * Z + Matrix[3][1]) * ReciprocalW;
        Result.Z = (Matrix[1][2] * Y + (Matrix[0][2] * X + (Matrix[2][2] * Z + Matrix[3][2]))) * ReciprocalW;
        return Result;
    }

    std::uint8_t TryIntersectRayWithSphere(EC_Struct::TVector3D RayOrigin, EC_Struct::TVector3D RayPointOnRay, EC_Struct::TVector3D SphereCenter, double SphereRadius, EC_Struct::TVector3D& HitPoint) {
        double OtherT{};
        EC_Struct::TVector3D Direction{};
        EC_Struct::TVector3D CenterDelta{};
        Direction.X = static_cast<long double>(RayPointOnRay.X) - RayOrigin.X;
        Direction.Y = static_cast<long double>(RayPointOnRay.Y) - RayOrigin.Y;
        Direction.Z = static_cast<long double>(RayPointOnRay.Z) - RayOrigin.Z;
        double T = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Direction.X) * Direction.X + static_cast<long double>(Direction.Y) * Direction.Y + static_cast<long double>(Direction.Z) * Direction.Z));
        Direction.X = static_cast<long double>(Direction.X) * T;
        Direction.Y = static_cast<long double>(Direction.Y) * T;
        Direction.Z = static_cast<long double>(Direction.Z) * T;
        CenterDelta.X = static_cast<long double>(SphereCenter.X) - RayOrigin.X;
        CenterDelta.Y = static_cast<long double>(SphereCenter.Y) - RayOrigin.Y;
        CenterDelta.Z = static_cast<long double>(SphereCenter.Z) - RayOrigin.Z;
        double DistanceSquared = static_cast<long double>(CenterDelta.X) * CenterDelta.X + static_cast<long double>(CenterDelta.Y) * CenterDelta.Y + static_cast<long double>(CenterDelta.Z) * CenterDelta.Z;
        double Projection = static_cast<long double>(CenterDelta.X) * Direction.X + static_cast<long double>(CenterDelta.Y) * Direction.Y + static_cast<long double>(CenterDelta.Z) * Direction.Z;
        double Discriminant = pas::sqr(static_cast<pas::Extended>(SphereRadius)) - DistanceSquared + static_cast<long double>(Projection) * Projection;
        if (Discriminant <= 0.0L) {
            return false;
        }
        Discriminant = System::Sqrt(Discriminant);
        if (Projection < Discriminant) {
            T = static_cast<long double>(Projection) + Discriminant;
            OtherT = static_cast<long double>(Projection) - Discriminant;
        } else {
            T = static_cast<long double>(Projection) - Discriminant;
            OtherT = static_cast<long double>(Projection) + Discriminant;
        }
        if (std::fabs(static_cast<pas::Extended>(T)) < 0.001L) {
            T = OtherT;
        }
        HitPoint.X = static_cast<long double>(Direction.X) * T + RayOrigin.X;
        HitPoint.Y = static_cast<long double>(Direction.Y) * T + RayOrigin.Y;
        HitPoint.Z = static_cast<long double>(Direction.Z) * T + RayOrigin.Z;
        return T > 0.001L;
    }

    void SolveMatrix4DLuSystem(const TMatrix4D& Factors, pas::Array<std::int32_t, 0, 3>& Permutations, pas::Array<double, 0, 3>& Solution) {
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
                    Sum = Sum - static_cast<long double>(Factors[I][J]) * Solution[J];
                }
            } else if (Sum != 0.0L) {
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
                        Sum = Sum - static_cast<long double>(Factors[I][J]) * Solution[J];
                    }
                }
            }
            Solution[I] = pas::real_divide(Sum, Factors[I][I]);
        }
    }

    void DecomposeMatrix4DLu(TMatrix4D& Matrix, double& PermutationSign, pas::Array<std::int32_t, 0, 3>& Permutations) {
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
                Magnitude = std::fabs(static_cast<pas::Extended>(Matrix[I][J]));
                if (Magnitude > Big) {
                    Big = Magnitude;
                }
            }
            Scales[I] = pas::real_divide(1.0L, Big);
        }
        for (J = 0; J <= 3; ++J) {
            I = 0;
            while (I < J) {
                Sum = Matrix[I][J];
                K = 0;
                while (K < I) {
                    Sum = Sum - static_cast<long double>(Matrix[I][K]) * Matrix[K][J];
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
                            Sum = Sum - static_cast<long double>(Matrix[I][K]) * Matrix[K][J];
                            ++K;
                        }
                        Matrix[I][J] = Sum;
                        Temp = std::fabs(static_cast<pas::Extended>(Sum)) * Scales[I];
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
            if (Matrix[J][J] == 0.0L) {
                Matrix[J][J] = 1.0E-20;
            }
            if (J != 3) {
                Temp = pas::real_divide(1.0L, Matrix[J][J]);
                {
                    const std::int32_t cpp_first_2 = J + 1;
                    if (cpp_first_2 <= 3) {
                        for (I = cpp_first_2; I <= 3; ++I) {
                            Matrix[I][J] = static_cast<long double>(Matrix[I][J]) * Temp;
                        }
                    }
                }
            }
        }
    }

} // namespace ab_Global
