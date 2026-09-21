#include "layout/SE_Comet.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_Sound.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Comet.hpp"
#include "units/SE_Planet.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Comet {
    void TCometSE_Create(TCometSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition) {
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
        Self->Animation = nullptr;
        Self->CompletedExplosion = nullptr;
        Self->CurrentExplosion = nullptr;
        Self->FirstTrailEntry = nullptr;
        Self->TrailHistoryCount = 0;
    }

    void TCometSE_Destroy(TCometSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TCometSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        ConfigureLoopSound(u"Comet"_wref.get());
        ConfigureRandomSound(u"Comet"_wref.get());
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
        Animation->SetImagePath(ImagePath);
        {
            WindowsSdk::TPoint contentSize = Animation->GetContentSize();
            GI_GAI::TgaiGI* animation = Animation;
            animation->SetSize(contentSize);
        }
        Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
        Animation->SetDepthByName(DepthExpression);
        Animation->SetPosition(EC_Struct::TruncatePointF(Position));
        Animation->SetPositionModeW(true);
        Animation->SequenceIndex = 0;
        Animation->UpdateAutoGeometry();
        if (SavedFrameIndex < 0 || SavedFrameIndex >= Animation->SequenceFrameCount) {
            SavedFrameIndex = aMyFunction::RandomIntRange(0, Animation->SequenceFrameCount - 1);
        }
        Animation->SetSequenceFrame(SavedFrameIndex);
        Animation->RestartPlayback();
        TrailHistoryCount = 0;
        while (FirstTrailEntry != nullptr) {
            RemoveTrailEntry(FirstTrailEntry);
        }
        StartMotionTimer();
    }

    void TCometSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        StopMotionTimer();
        while (FirstTrailEntry != nullptr) {
            RemoveTrailEntry(FirstTrailEntry);
        }
        if (Animation != nullptr) {
            SavedFrameIndex = Animation->SequenceFrame;
            pas::free(Animation);
            Animation = nullptr;
        }
        if (CompletedExplosion != nullptr) {
            pas::free(CompletedExplosion);
            CompletedExplosion = nullptr;
        }
        if (CurrentExplosion != nullptr) {
            pas::free(CurrentExplosion);
            CurrentExplosion = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TCometSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            if (Animation != nullptr) {
                Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            }
        }
    }

    void TCometSE::ResetTrajectory(float Angle) {
        if (SkipMoves >= 0) {
            if (pas::random(50, &System::RandSeed) < 25) {
                Velocity.X = System::Sin(Angle) * Speed;
                Velocity.Y = System::Cos(Angle) * -Speed;
            } else {
                Velocity.X = System::Sin(Angle) * -Speed;
                Velocity.Y = System::Cos(Angle) * Speed;
            }
            Position.X = aMyFunction::RandomIntRange(-4096, 4096);
            Position.Y = aMyFunction::RandomIntRange(-4096, 4096);
            SkipMoves = -1;
            while (static_cast<long double>(Globals::SpaceViewPosition.X) - (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) < Position.X && static_cast<long double>(Globals::SpaceViewPosition.X) + (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) > Position.X && static_cast<long double>(Globals::SpaceViewPosition.Y) - (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) < Position.Y && static_cast<long double>(Globals::SpaceViewPosition.Y) + (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) > Position.Y) {
                Position.X = aMyFunction::RandomIntRange(-4096, 4096);
                Position.Y = aMyFunction::RandomIntRange(-4096, 4096);
            }
        }
    }

    void TCometSE::StartMotionTimer() {
        StopMotionTimer();
        MoveTimer = Space->CreateTimer(TimerInterval, TimerInterval, pas::bind_method<&TCometSE::AdvanceMotionTimer>(this), 0);
    }

    void TCometSE::StopMotionTimer() {
        if (MoveTimer != nullptr) {
            Space->DeleteTimer(MoveTimer);
            MoveTimer = nullptr;
        }
    }

    void TCometSE::AdvanceMotionTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        AdvanceSteps(1);
    }

    void TCometSE::ApplyAttraction(EC_Struct::TPointF Center, float Strength) {
        double Angle{};
        float Force{};
        EC_Struct::TPointF Delta{};
        Delta = EC_Struct::MakePointF(static_cast<long double>(Position.X) - Center.X, static_cast<long double>(Position.Y) - Center.Y);
        if (std::fabs(static_cast<pas::Extended>(Delta.X)) < 1.0L) {
            Angle = Math::ArcTan2(Delta.Y, 1.0L);
        } else {
            Angle = Math::ArcTan2(Delta.Y, Delta.X);
        }
        if (std::fabs(static_cast<pas::Extended>(Delta.X)) + std::fabs(static_cast<pas::Extended>(Delta.Y)) < 64.0L) {
            Force = 0.0f;
        } else {
            Force = pas::real_divide(Strength, static_cast<long double>(Delta.X) * Delta.X + static_cast<long double>(Delta.Y) * Delta.Y);
        }
        Velocity = aMyFunction::OffsetPointByRadiusAngle(Velocity, -Force, Angle);
        if (std::fabs(static_cast<pas::Extended>(Velocity.X)) > std::fabs(static_cast<pas::Extended>(Velocity.Y))) {
            if (Velocity.X > 7.0L) {
                Velocity.Y = pas::real_divide(Velocity.Y * 7.0L, Velocity.X);
                Velocity.X = 7.0f;
            }
            if (Velocity.X < -7.0L) {
                Velocity.Y = pas::real_divide(Velocity.Y * 7.0L, -Velocity.X);
                Velocity.X = -7.0f;
            }
        } else {
            if (Velocity.Y > 7.0L) {
                Velocity.X = pas::real_divide(Velocity.X * 7.0L, Velocity.Y);
                Velocity.Y = 7.0f;
            }
            if (Velocity.Y < -7.0L) {
                Velocity.X = pas::real_divide(Velocity.X * 7.0L, -Velocity.Y);
                Velocity.Y = -7.0f;
            }
        }
    }

    void TCometSE::ExplosionFinished(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender != nullptr) {
            if (pas::class_cast_if<GI_GAI::TgaiGI*>(Sender) != nullptr) {
                if (CompletedExplosion != nullptr) {
                    pas::free(CompletedExplosion);
                    CompletedExplosion = nullptr;
                }
                if (Sender == CurrentExplosion) {
                    CompletedExplosion = CurrentExplosion;
                    CompletedExplosion->SetSequenceFrame(CompletedExplosion->SequenceFrameCount - 2);
                    CurrentExplosion = nullptr;
                }
            }
        }
    }

    void TCometSE::RemoveTrailEntry(PCometTrailEntry Entry) {
        if (Entry != nullptr) {
            if (Entry->Prev != nullptr) {
                Entry->Prev->Next = Entry->Next;
            }
            if (Entry->Next != nullptr) {
                Entry->Next->Prev = Entry->Prev;
            }
            if (Entry == FirstTrailEntry) {
                FirstTrailEntry = Entry->Next;
            }
            if (Entry->Animation != nullptr) {
                pas::free(Entry->Animation);
            }
            Entry->Animation = nullptr;
            pas::dispose(Entry);
        }
    }

    void TCometSE::ExplodeAndRespawn() {
        GI_GAI::TgaiGI* Control{};
        if (Space != nullptr) {
            if (Globals::FilmSoundEffectsEnabled && GlobalsV::SoundInSpaceEnabled && Space->ContainsMapPoint(Position)) {
                const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"Sound.expl", SysUtils::IntToStr(aMyFunction::RandomIntRange(3, 5))}));
                GR_Sound::TSoundControl* soundManager = GR_Main::SoundManager;
                soundManager->PlaySound(cpp_arg);
            }
            Control = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            Control->SetImagePath(ExplosionPath);
            Control->LoadFrameSequenceFromText(ExplosionFrames);
            Control->SetSequenceFrame(0);
            Control->SetSize(Control->GetContentSize());
            Control->SetOrigin(EC_Struct::HalfPoint(Control->ClientSize));
            Control->SetDepthByName(DepthExpression);
            Control->SetDepth(Control->Depth - 1.0L);
            Control->SetPosition(EC_Struct::TruncatePointF(Position));
            Control->SetPositionModeW(true);
            Control->CycleCompleteCallback = pas::bind_method<&TCometSE::ExplosionFinished>(this);
            Control->RestartPlayback();
            if (CurrentExplosion != nullptr) {
                pas::free(CurrentExplosion);
            }
            CurrentExplosion = Control;
            do {
                switch (pas::random(4, &System::RandSeed)) {
                    case 0: {
                        Position.X = -4096.0f;
                        Position.Y = aMyFunction::RandomIntRange(-4096, 4096);
                        break;
                    }
                    case 1: {
                        Position.X = 4096.0f;
                        Position.Y = aMyFunction::RandomIntRange(-4096, 4096);
                        break;
                    }
                    case 2: {
                        Position.Y = 4096.0f;
                        Position.X = aMyFunction::RandomIntRange(-4096, 4096);
                        break;
                    }
                    case 3: {
                        Position.Y = -4096.0f;
                        Position.X = aMyFunction::RandomIntRange(-4096, 4096);
                        break;
                    }
                }
            } while (!(static_cast<long double>(Globals::SpaceViewPosition.X) - (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) > Position.X || static_cast<long double>(Globals::SpaceViewPosition.X) + (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) < Position.X || static_cast<long double>(Globals::SpaceViewPosition.Y) - (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) > Position.Y || static_cast<long double>(Globals::SpaceViewPosition.Y) + (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) < Position.Y));
            Velocity.X = Velocity.X * 0.5L;
            Velocity.Y = Velocity.Y * 0.5L;
        }
    }

    void TCometSE::AdvanceSteps(std::int32_t Count) {
        PCometTrailEntry NextEntry{};
        PCometTrailEntry Entry{};
        SE_Planet::PPlanetCollisionCircle Circle{};
        while (Count > 0) {
            if (CompletedExplosion != nullptr) {
                pas::free(CompletedExplosion);
                CompletedExplosion = nullptr;
            }
            NextEntry = FirstTrailEntry;
            while (NextEntry != nullptr) {
                Entry = NextEntry;
                NextEntry = NextEntry->Next;
                if (Entry->Finished) {
                    RemoveTrailEntry(Entry);
                } else {
                    pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, EC_Struct::MakePointF(static_cast<long double>(Entry->Position.X) + Entry->Velocity.X, static_cast<long double>(Entry->Position.Y) + Entry->Velocity.Y));
                    if (CurrentExplosion != nullptr) {
                        pas::store_unaligned<EC_Struct::TPointF>(&Entry->Velocity, EC_Struct::HalfPointF(Entry->Velocity));
                    }
                    if (Entry->Animation != nullptr) {
                        Entry->Animation->SetPosition(EC_Struct::TruncatePointF(Entry->Position));
                    }
                }
            }
            ApplyAttraction(EC_Struct::MakePointF(0.0f, 0.0f), StarAttraction);
            if (std::fabs(static_cast<pas::Extended>(Position.X)) < 2.0E+2L && std::fabs(static_cast<pas::Extended>(Position.Y)) < 2.0E+2L && static_cast<long double>(Position.X) * Position.X + static_cast<long double>(Position.Y) * Position.Y < 4.0E+4L) {
                ExplodeAndRespawn();
            }
            Circle = SE_Planet::FirstPlanetCollisionCircle;
            while (Circle != nullptr) {
                ApplyAttraction(EC_Struct::MakePointF(Circle->Position.X, Circle->Position.Y), ObjectAttraction);
                if (std::fabs(static_cast<long double>(Circle->Position.X) - Position.X) < Circle->Radius && std::fabs(static_cast<long double>(Circle->Position.Y) - Position.Y) < Circle->Radius && (static_cast<long double>(Circle->Position.X) - Position.X) * (static_cast<long double>(Circle->Position.X) - Position.X) + (static_cast<long double>(Circle->Position.Y) - Position.Y) * (static_cast<long double>(Circle->Position.Y) - Position.Y) < Circle->RadiusSquared) {
                    ExplodeAndRespawn();
                    break;
                }
                Circle = Circle->Next;
            }
            Position = EC_Struct::MakePointF(static_cast<long double>(Position.X) + Velocity.X, static_cast<long double>(Position.Y) + Velocity.Y);
            if (static_cast<long double>(Globals::SpaceViewPosition.X) - (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) > Position.X || static_cast<long double>(Globals::SpaceViewPosition.Y) - (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) > Position.Y || static_cast<long double>(Globals::SpaceViewPosition.X) + (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) < Position.X || static_cast<long double>(Globals::SpaceViewPosition.Y) + (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) < Position.Y) {
                if (Position.X > 4096.0L && Velocity.X >= 0.0L) {
                    Position.X = -4096.0f;
                }
                if (Position.X < -4096.0L && Velocity.X <= 0.0L) {
                    Position.X = 4096.0f;
                }
                if (Position.Y > 4096.0L && Velocity.Y >= 0.0L) {
                    Position.Y = -4096.0f;
                }
                if (Position.Y < -4096.0L && Velocity.Y <= 0.0L) {
                    Position.Y = 4096.0f;
                }
                while (static_cast<long double>(Globals::SpaceViewPosition.X) - (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) < Position.X && static_cast<long double>(Globals::SpaceViewPosition.X) + (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >> 1) > Position.X && static_cast<long double>(Globals::SpaceViewPosition.Y) - (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) < Position.Y && static_cast<long double>(Globals::SpaceViewPosition.Y) + (static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >> 1) > Position.Y) {
                    Position.X = aMyFunction::RandomIntRange(-4096, 4096);
                    Position.Y = aMyFunction::RandomIntRange(-4096, 4096);
                }
            }
            if (Animation != nullptr) {
                SetPosition(Position);
            }
            --Count;
        }
    }

    void TCometSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        Types::TPoint IntRange{};
        EC_Struct::TPointF Range{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        SkipMoves = 0;
        SavedFrameIndex = -1;
        TimerInterval = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Time"sv)));
        StarAttraction = 2.0E+4f;
        ObjectAttraction = 4.0E+3f;
        if (Block->CountParams(u"Radius"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"Radius"sv)));
            Radius = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"Speed"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"Speed"sv)));
            Speed = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"StarFallStrength"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"StarFallStrength"sv)));
            StarAttraction = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"PlanetFallStrength"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"PlanetFallStrength"sv)));
            ObjectAttraction = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"SkipMoves"_wref.get()) > 0) {
            IntRange = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SkipMoves"sv)));
            SkipMoves = aMyFunction::RandomIntRange(IntRange.X, IntRange.Y);
        }
        if (Block->CountParams(u"MoveAngle"_wref.get()) > 0) {
            IntRange = GI_Main::GetPointGI(pas::view(Block->GetParam(u"MoveAngle"sv)));
            MoveAngle = pas::real_divide(aMyFunction::RandomIntRange(IntRange.X, IntRange.Y) * SystemImports::Pi, 1.8E+2L);
        }
        ExplosionPath = Block->GetParam(u"Explore"sv);
        ExplosionFrames = Block->GetParam(u"ExploreFrame"sv);
        TrailPath = Block->GetParam(u"Track"sv);
        TrailFrames = Block->GetParam(u"TrackFrame"sv);
        ResetTrajectory(MoveAngle);
    }

    void TCometSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TCometSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_GAI::TgaiGI* Control = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        Control->SetImagePath(ImagePath);
        Control->QueueImageLoad(PendingLoads);
        pas::free(Control);
    }

    void TCometSE::p_destroy() {
        SE_Comet::TCometSE_Destroy(this);
    }

} // namespace SE_Comet
