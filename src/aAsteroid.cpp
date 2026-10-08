#include "layout/aAsteroid.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/SE_Asteroid.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aEFilm.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/Math.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/aAsteroid.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"

namespace aAsteroid {
    const float AsteroidGravitationalConstant = 6.6720414E-11f;

    void TAsteroid_Create(TAsteroid* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextAsteroidId;
            ++aGalaxy::Galaxy->NextAsteroidId;
        }
    }

    void TAsteroid_Destroy(TAsteroid* Self) {
        if (Self->GraphObject != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->GraphObject));
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Requires an unassigned GraphObject.
    void TAsteroid::Init(aGalaxy::TStar* Star, const pas::WideString& GraphKey) {
        CurrentStar = Star;
        {
            SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Asteroid"sv, GraphKey,
                ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphObject = pas::Var<SE_Space::TObjectSE*>(&GraphObject);
            SE_Space::RetainSpaceObject(graphObject, createSpaceObjectByName);
        }
        Respawn();
    }

    void TAsteroid::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(Id);
        Buffer->AddWideStringZ(GraphObject->GraphKey);
        Buffer->AddSingle(PhysicsPosition.X);
        Buffer->AddSingle(PhysicsPosition.Y);
        Buffer->AddSingle(Velocity.X);
        Buffer->AddSingle(Velocity.Y);
        Buffer->AddSingle(Mass);
        Buffer->AddIntegerValue(MineralCount);
    }

    // Caller sets CurrentStar. Requires an unassigned GraphObject.
    void TAsteroid::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        Id = Buffer->GetUInt32();
        if (Galaxy->NextAsteroidId <= Id) {
            Galaxy->NextAsteroidId = Id + 1;
        }
        {
            SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Asteroid::TAsteroidSE>(SE_Space::TObjectSE_Create,
                Buffer->ReadWideString(), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphObject = pas::Var<SE_Space::TObjectSE*>(&GraphObject);
            SE_Space::RetainSpaceObject(graphObject, cpp_arg);
        }
        PhysicsPosition.X = Buffer->GetSingle();
        PhysicsPosition.Y = Buffer->GetSingle();
        Velocity.X = Buffer->GetSingle();
        Velocity.Y = Buffer->GetSingle();
        Mass = Buffer->GetSingle();
        InverseMass = static_cast<float>(1.0 / Mass);
        GravityForceFactor = static_cast<float>(static_cast<float>(static_cast<double>(AsteroidGravitationalConstant) * Mass) * AsteroidCentralMass);
        MineralCount = Buffer->GetInt32();
        Position.X = static_cast<float>(PhysicsPosition.X * AsteroidWorldScale);
        Position.Y = static_cast<float>(PhysicsPosition.Y * AsteroidWorldScale);
    }

    void TAsteroid::RespawnIfOutsideSystem() {
        if (static_cast<float>(static_cast<float>(static_cast<double>(Position.X) * Position.X) + static_cast<float>(static_cast<double>(Position.Y) * Position.Y)) >
            pas::sqr(CurrentStar->MapDiameter)) {
            Respawn();
        }
    }

    void TAsteroid::PrepareTurnMovement(std::int32_t StartStepIndex, std::uint8_t RecordFilm) {
        if (RecordFilm) {
            FilmObject = Globals::PrimaryFilm->AddObject(Id, GraphObject, 0, 0);
            Globals::PrimaryFilm->SetObjectPosition(StartStepIndex, FilmObject, Position);
            Globals::PrimaryFilm->AttachObject(StartStepIndex, FilmObject);
        }
    }

    void TAsteroid::AdvanceOrbitStep(std::int32_t StepIndex, std::uint8_t RecordFilm) {
        IntegrateMotion(static_cast<float>(pas::constant(static_cast<double>(aGalaxyStruct::BaseMovementStepsPerTurn)) /
            CurrentStar->MovementStepCount));
        if (RecordFilm) {
            Globals::PrimaryFilm->SetObjectPosition(StepIndex, FilmObject, Position);
        }
    }

    // Keeps the ID and visual. May spawn another asteroid under the galaxy's special mode.
    void TAsteroid::Respawn() {
        Mass = 1.0E+6f;
        InverseMass = static_cast<float>(1.0 / Mass);
        GravityForceFactor = static_cast<float>(static_cast<float>(static_cast<double>(AsteroidGravitationalConstant) * Mass) * AsteroidCentralMass);
        float Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::NextRandomIntRange(0, 360,
            CurrentStar->RandomState));
        float Radius = static_cast<float>(static_cast<float>(static_cast<float>(CurrentStar->MapDiameter / 2.0) + 8.0E+2) + 2.0E+3);
        Radius = static_cast<float>(static_cast<double>(Radius) + aMyFunction::NextRandomIntRange(0, 1000, CurrentStar->RandomState));
        if (static_cast<double>(Radius) > CurrentStar->MapDiameter) {
            std::int32_t cpp_right = aMyFunction::NextRandomIntRange(0, 100, CurrentStar->RandomState);
            Radius = CurrentStar->MapDiameter - 50 - cpp_right;
        }
        Position.X = static_cast<float>(System::Sin(Angle) * Radius);
        Position.Y = static_cast<float>(-System::Cos(Angle) * Radius);
        PhysicsPosition.X = static_cast<float>(Position.X * 0x1.3de4355555556p+27);
        PhysicsPosition.Y = static_cast<float>(Position.Y * 0x1.3de4355555556p+27);
        float Speed = aMyFunction::NextRandomIntRange(0, 3000, CurrentStar->RandomState) + 7000;
        Angle = Math::ArcTan2(static_cast<float>(0.0 - Position.X), -(static_cast<float>(0.0 - Position.Y)));
        {
            double cpp_left = aMyFunction::HeadingDegreesToRadians(aMyFunction::NextRandomIntRange(-10, 10,
                CurrentStar->RandomState) + 25);
            Angle = static_cast<float>(Angle + static_cast<float>(cpp_left * (2 * aMyFunction::NextRandomIntRange(0, 1, CurrentStar->RandomState) - 1)));
        }
        Velocity.X = static_cast<float>(System::Sin(Angle) * Speed);
        Velocity.Y = static_cast<float>(-System::Cos(Angle) * Speed);
        MineralCount = aMyFunction::NextRandomIntRange(20, 99, CurrentStar->RandomState);
        if (aGalaxy::Galaxy != nullptr) {
            if (aPlayer::GetPlayer() != nullptr) {
                if (aPlayer::GetPlayer()->CurrentStar != CurrentStar || !aPlayer::GetPlayer()->InNormalSpace()) {
                    if (aGalaxy::Galaxy->GodModEnabled == 2 && aMyFunction::NextRandomIntRange(0, 100,
                            CurrentStar->RandomState) > 50) {
                        SpawnSiblingAsteroidInCurrentStar();
                    }
                }
            }
        }
    }

    // The new asteroid belongs to CurrentStar.Asteroids; it does not copy this asteroid's visual or motion.
    void TAsteroid::SpawnSiblingAsteroidInCurrentStar() {
        pas::WideString Text{};
        std::int32_t Index{};
        std::int32_t Variant{};
        if (CurrentStar->BackgroundImage < 10) {
            Text = ([&] {
                    const pas::WideString& cpp_arg = pas::concat_wide({u"0",
                        EC_Str::IntToWideString(CurrentStar->BackgroundImage)});
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(
                        u"StyleAsteroid"_wref.get());
                    return blockByPath->GetParam(pas::view(cpp_arg));
                }());
        } else {
            Text = ([&] {
                    const pas::WideString& intToWideString = EC_Str::IntToWideString(CurrentStar->BackgroundImage);
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(
                        u"StyleAsteroid"_wref.get());
                    return blockByPath_2->GetParam(pas::view(intToWideString));
                }());
        }
        Index = aMyFunction::NextRandomIntRange(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) / 2 - 1,
                CurrentStar->RandomState) * 2;
        std::int32_t VariantCount = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text),
            Index + 1, u","sv)));
        Text = EC_Str::ExtractDelimitedPartW(pas::view(Text), Index, u","sv);
        Variant = aMyFunction::NextRandomIntRange(0, VariantCount - 1, CurrentStar->RandomState);
        TAsteroid* Asteroid = pas::construct_call<TAsteroid>(TAsteroid_Create);
        if (Variant < 10) {
            Asteroid->Init(CurrentStar, pas::concat_wide({u"Asteroid.", Text, u"0", EC_Str::IntToWideString(Variant)}));
        } else {
            Asteroid->Init(CurrentStar, pas::concat_wide({u"Asteroid.", Text, EC_Str::IntToWideString(Variant)}));
        }
        pas::list_add(CurrentStar->Asteroids, reinterpret_cast<void*>(Asteroid));
    }

    void TAsteroid::IntegrateMotion(float TimeScale) {
        float DeltaX = static_cast<float>(0.0 - Position.X);
        float DeltaY = static_cast<float>(0.0 - Position.Y);
        float DistanceSquared = static_cast<float>(static_cast<float>(static_cast<double>(DeltaX) * DeltaX) + static_cast<float>(static_cast<double>(DeltaY) * DeltaY));
        float InverseDistance = static_cast<float>(1.0 / System::Sqrt(DistanceSquared));
        if (DistanceSquared < 1.0E+4) {
            DistanceSquared = 1.0E+4f;
        }
        float Force = static_cast<float>(GravityForceFactor / (static_cast<float>(DistanceSquared * AsteroidInverseScaleSquared)));
        float ForceX = static_cast<float>(static_cast<float>(static_cast<double>(DeltaX) * InverseDistance) * Force);
        float ForceY = static_cast<float>(static_cast<float>(static_cast<double>(DeltaY) * InverseDistance) * Force);
        float AccelX = static_cast<float>(static_cast<double>(ForceX) * InverseMass);
        float AccelY = static_cast<float>(static_cast<double>(ForceY) * InverseMass);
        Velocity.X = static_cast<float>(Velocity.X + static_cast<float>(static_cast<float>(static_cast<double>(AccelX) * TimeScale) * 19968.0));
        Velocity.Y = static_cast<float>(Velocity.Y + static_cast<float>(static_cast<float>(static_cast<double>(AccelY) * TimeScale) * 19968.0));
        PhysicsPosition.X = static_cast<float>(PhysicsPosition.X + static_cast<float>(static_cast<float>(static_cast<double>(Velocity.X) * TimeScale) * 19968.0));
        PhysicsPosition.Y = static_cast<float>(PhysicsPosition.Y + static_cast<float>(static_cast<float>(static_cast<double>(Velocity.Y) * TimeScale) * 19968.0));
        Position.X = static_cast<float>(PhysicsPosition.X * AsteroidWorldScale);
        Position.Y = static_cast<float>(PhysicsPosition.Y * AsteroidWorldScale);
    }

    // Writes Count future positions at TimeScale=1, excluding the current position, then restores the live motion state. Caller supplies Count * 8 bytes.
    void TAsteroid::WritePredictedPositions(EC_Struct::PPointF Positions, std::int32_t Count) {
        EC_Struct::TPointF SavedPosition{};
        EC_Struct::TPointF SavedPhysicsPosition{};
        EC_Struct::TPointF SavedVelocity{};
        std::int32_t Index{};
        SavedPosition = Position;
        SavedPhysicsPosition = PhysicsPosition;
        SavedVelocity = Velocity;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            IntegrateMotion(1.0f);
            EC_Mem::WriteSingleEC(Positions, Position.X);
            Positions = static_cast<EC_Struct::PPointF>(EC_Mem::AddPointerOffset(Positions, 4));
            EC_Mem::WriteSingleEC(Positions, Position.Y);
            Positions = static_cast<EC_Struct::PPointF>(EC_Mem::AddPointerOffset(Positions, 4));
        }
        Position = SavedPosition;
        PhysicsPosition = SavedPhysicsPosition;
        Velocity = SavedVelocity;
    }

    pas::WideString TAsteroid::GetDisplayName() {
        pas::WideString Result{};
        Result = aConst::LocalizedText(u"Asteroid.Name"_wref.get());
        aMyFunction::ReplaceTextToken(Result, u"<Number>"_w, EC_Str::IntToWideString(Id),
            aMyFunction::TextHighlightColorTag);
        return Result;
    }

    pas::WideString TAsteroid::GetInfoText() {
        pas::WideString Result{};
        Result = aConst::LocalizedText(u"Asteroid.Text"_wref.get());
        aMyFunction::ReplaceTextToken(Result, u"<Number>"_w, EC_Str::IntToWideString(Id),
            aMyFunction::TextHighlightColorTag);
        float Speed = System::Sqrt(static_cast<float>(pas::sqr(static_cast<double>(Velocity.X)) +
            pas::sqr(static_cast<double>(Velocity.Y))));
        Speed = static_cast<float>(static_cast<float>(static_cast<float>(Speed * pas::constant(static_cast<double>(aGalaxyStruct::BaseMovementStepsPerTurn))) * 19968.0) *
                AsteroidWorldScale);
        aMyFunction::ReplaceTextToken(Result, u"<Speed>"_w, EC_Str::IntToWideString(pas::real_round(Speed)),
            aMyFunction::TextHighlightColorTag);
        aMyFunction::ReplaceTextToken(Result, u"<Count>"_w, EC_Str::IntToWideString(MineralCount),
            aMyFunction::TextHighlightColorTag);
        return Result;
    }

    void TAsteroid::p_destroy() {
        aAsteroid::TAsteroid_Destroy(this);
    }

} // namespace aAsteroid
