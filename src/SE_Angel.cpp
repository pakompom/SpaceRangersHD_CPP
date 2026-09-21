#include "layout/SE_Angel.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Space.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/Math.hpp"
#include "units/SE_Angel.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Angel {
    void TAngelSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        std::int32_t Index{};
        if (IsAttachedToSpace()) {
            return;
        }
        ConfigureLoopSound(u"Angel"_wref.get());
        ConfigureRandomSound(u"Angel"_wref.get());
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        FirstEntry = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, EntryCount); cpp_range.next(Index); ) {
            AppendEntry();
        }
        MoveState = 0;
        TargetDelay = 500;
        StartMotionTimer();
    }

    void TAngelSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        StopMotionTimer();
        while (FirstEntry != nullptr) {
            RemoveEntry(FirstEntry);
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TAngelSE::SetPosition(EC_Struct::TPointF APosition) {
        PAngelEntry Entry{};
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Entry = FirstEntry;
            while (Entry != nullptr) {
                if (Entry->Animation != nullptr) {
                    Entry->Animation->SetPosition(EC_Struct::TruncatePointF(aMyFunction::RotateAndTranslatePoint(Entry->Position, APosition, MoveAngle)));
                }
                Entry = Entry->Next;
            }
        }
    }

    void TAngelSE::SetVelocityFromAngle(float Angle) {
        Velocity.X = System::Sin(Angle) * Speed;
        Velocity.Y = System::Cos(Angle) * Speed;
        AdvanceSteps(0);
    }

    void TAngelSE::StartMotionTimer() {
        StopMotionTimer();
        MoveTimer = Space->CreateTimer(TimerInterval, TimerInterval, pas::bind_method<&TAngelSE::AdvanceMotionTimer>(this), 0);
    }

    void TAngelSE::StopMotionTimer() {
        if (MoveTimer != nullptr) {
            Space->DeleteTimer(MoveTimer);
            MoveTimer = nullptr;
        }
    }

    std::int32_t TAngelSE::GetFrameCount(PAngelEntry Entry) {
        std::int32_t Result = 0;
        if (Entry == nullptr) {
            Entry = FirstEntry;
        }
        if (Entry != nullptr && Entry->Animation != nullptr) {
            return Entry->Animation->GetMainImageFrameCount();
        }
        return Result;
    }

    void TAngelSE::ToggleFrameVariants() {
        PAngelEntry Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->Animation != nullptr) {
                if (Entry->FrameVariant == 0) {
                    Entry->FrameVariant = 1;
                } else {
                    --Entry->FrameVariant;
                }
            }
            Entry = Entry->Next;
        }
    }

    void TAngelSE::AdvanceMotionTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        AdvanceSteps(1);
    }

    void TAngelSE::AppendEntry() {
        PAngelEntry Entry{};
        pas::new_value(Entry);
        Entry->Next = FirstEntry;
        Entry->Prev = nullptr;
        if (FirstEntry != nullptr) {
            FirstEntry->Prev = Entry;
        }
        FirstEntry = Entry;
        {
            float randomFloatRange = aMyFunction::RandomFloatRange(-GroupSize.X, GroupSize.X);
            float randomFloatRange_2 = aMyFunction::RandomFloatRange(-GroupSize.Y, GroupSize.Y);
            pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, EC_Struct::MakePointF(randomFloatRange, randomFloatRange_2));
        }
        pas::store_unaligned<EC_Struct::TPointF>(&Entry->Velocity, EC_Struct::MakePointF(0.0f, 0.0f));
        pas::store_unaligned<EC_Struct::TPointF>(&Entry->Target, Entry->Position);
        Entry->Angle = 0.0f;
        Entry->Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
        {
            auto cpp_arg = pas::borrow(*([&] {
                auto cpp_index = pas::random(ImageCount, &System::RandSeed);
                auto* cpp_array = &ImagePaths;
                return &(*cpp_array)[cpp_index];
            }()));
            GI_GAI::TgaiGI* animation = Entry->Animation;
            animation->SetImagePath(cpp_arg.get());
        }
        Entry->Animation->SequenceIndex = 0;
        Entry->Animation->UpdateAutoGeometry();
        {
            WindowsSdk::TPoint contentSize = Entry->Animation->GetContentSize();
            GI_GAI::TgaiGI* animation_2 = Entry->Animation;
            animation_2->SetSize(contentSize);
        }
        Entry->Animation->SetOrigin(EC_Struct::HalfPoint(Entry->Animation->ClientSize));
        Entry->Animation->SetDepthByName(DepthExpression);
        {
            std::int32_t trunc = System::Trunc(static_cast<long double>(Entry->Position.Y) + Position.Y);
            std::int32_t trunc_2 = System::Trunc(static_cast<long double>(Entry->Position.X) + Position.X);
            Entry->Animation->SetPosition(ClassesImports::Point(trunc_2, trunc));
        }
        Entry->Animation->SetPositionModeW(true);
        Entry->Animation->SetSequenceFrame(0);
        Entry->Animation->RestartPlayback();
        Entry->FrameIndex = 0;
        Entry->MovingUp = false;
        Entry->FrameVariant = 0;
    }

    void TAngelSE::RemoveEntry(PAngelEntry Entry) {
        if (Entry != nullptr) {
            if (Entry->Prev != nullptr) {
                Entry->Prev->Next = Entry->Next;
            }
            if (Entry->Next != nullptr) {
                Entry->Next->Prev = Entry->Prev;
            }
            if (Entry == FirstEntry) {
                FirstEntry = Entry->Next;
            }
            if (Entry->Animation != nullptr) {
                pas::free(Entry->Animation);
            }
            pas::dispose(Entry);
        }
    }

    void TAngelSE::AdvanceEntry(PAngelEntry Entry) {
        EC_Struct::TPointF Movement{};
        if (std::fabs(static_cast<long double>(Entry->Position.X) - Entry->Target.X) <= std::fabs(static_cast<pas::Extended>(Entry->Velocity.X)) + 0.2L && std::fabs(static_cast<long double>(Entry->Position.Y) - Entry->Target.Y) <= std::fabs(static_cast<pas::Extended>(Entry->Velocity.Y)) + 0.2L) {
            {
                float randomFloatRange = aMyFunction::RandomFloatRange(-GroupSize.X, GroupSize.X);
                float randomFloatRange_2 = aMyFunction::RandomFloatRange(-GroupSize.Y, GroupSize.Y);
                Movement = EC_Struct::MakePointF(randomFloatRange, randomFloatRange_2);
            }
            pas::store_unaligned<EC_Struct::TPointF>(&Entry->Target, Movement);
            {
                float cpp_arg = pas::real_divide(static_cast<long double>(-Entry->Position.X) + Entry->Target.X, 64.0L);
                float cpp_arg_2 = pas::real_divide(static_cast<long double>(-Entry->Position.Y) + Entry->Target.Y, 64.0L);
                pas::store_unaligned<EC_Struct::TPointF>(&Entry->Velocity, EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
            }
        } else {
            pas::store_unaligned<EC_Struct::TPointF>(&Entry->Position, EC_Struct::MakePointF(static_cast<long double>(Entry->Position.X) + Entry->Velocity.X, static_cast<long double>(Entry->Position.Y) + Entry->Velocity.Y));
        }
        Movement = aMyFunction::RotateAndTranslatePoint(Entry->Velocity, Velocity, MoveAngle);
        if (Entry->Velocity.Y < -0.5L) {
            Entry->MovingUp = true;
        } else {
            Entry->MovingUp = false;
        }
        if (std::fabs(static_cast<pas::Extended>(Movement.X)) < 0.1L) {
            Entry->Angle = Math::ArcTan2(Movement.Y, 0.1L);
        } else {
            Entry->Angle = Math::ArcTan2(Movement.Y, Movement.X);
        }
    }

    void TAngelSE::Wander() {
        SE_Space::TObjectSE* Obj{};
        std::uint32_t Count{};
        std::int32_t Index{};
        if (pas::abs(TurnTicks) == 0) {
            if (pas::random(100, &System::RandSeed) < 10) {
                TurnTicks = aMyFunction::RandomIntRange(-128, 128);
            }
        } else if (TurnTicks < 0) {
            ++TurnTicks;
            MoveAngle = MoveAngle + 0.01L;
            if (MoveAngle > SystemImports::Pi) {
                MoveAngle = MoveAngle - pas::constant(2.0L * SystemImports::Pi);
            }
        } else {
            --TurnTicks;
            MoveAngle = MoveAngle - 0.01L;
            if (MoveAngle < pas::constant(-SystemImports::Pi)) {
                MoveAngle = MoveAngle + pas::constant(2.0L * SystemImports::Pi);
            }
        }
        if (TargetDelay > 0) {
            --TargetDelay;
        }
        if (TargetDelay == 0 && MoveState == 0 && Space != nullptr) {
            Obj = Space->FirstObject;
            Count = 0u;
            while (Obj != nullptr) {
                if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr) {
                    ++Count;
                }
                Obj = Obj->Next;
            }
            Index = pas::random(static_cast<std::int32_t>(Count), &System::RandSeed);
            Obj = Space->FirstObject;
            while (Obj != nullptr) {
                if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr) {
                    if (Index == 0) {
                        Target = Obj->Position;
                        MoveState = 1;
                        break;
                    }
                    --Index;
                }
                Obj = Obj->Next;
            }
        }
        if (TurnTicks == 0) {
            if (MoveState == 2) {
                MoveState = 1;
            }
        }
    }

    void TAngelSE::SeekTarget() {
        EC_Struct::TPointF Delta{};
        float Angle{};
        Delta = EC_Struct::MakePointF(static_cast<long double>(Target.X) - Position.X, static_cast<long double>(Target.Y) - Position.Y);
        if (std::fabs(static_cast<pas::Extended>(Delta.X)) < 0.2L) {
            Angle = Math::ArcTan2(Delta.Y, 0.2L);
        } else {
            Angle = Math::ArcTan2(Delta.Y, Delta.X);
        }
        if (std::fabs(static_cast<long double>(Angle) - MoveAngle) < SystemImports::Pi) {
            if (Angle - 0.01L > MoveAngle) {
                MoveAngle = MoveAngle + 0.02L;
            } else if (Angle + 0.01L < MoveAngle) {
                MoveAngle = MoveAngle - 0.02L;
            }
        } else {
            if (MoveAngle < Angle) {
                MoveAngle = MoveAngle - 0.02L;
            } else {
                MoveAngle = MoveAngle + 0.02L;
            }
            if (MoveAngle > SystemImports::Pi) {
                MoveAngle = MoveAngle - pas::constant(2.0L * SystemImports::Pi);
            }
            if (MoveAngle < pas::constant(-SystemImports::Pi)) {
                MoveAngle = MoveAngle + pas::constant(2.0L * SystemImports::Pi);
            }
        }
        if (std::fabs(static_cast<long double>(Position.X) - Target.X) < 64.0L && std::fabs(static_cast<long double>(Position.Y) - Target.Y) < 64.0L) {
            MoveState = 0;
            TargetDelay = 500 + pas::random(500, &System::RandSeed);
        } else if (pas::random(1000, &System::RandSeed) < 10) {
            TurnTicks = aMyFunction::RandomIntRange(-128, 128);
            MoveState = 2;
        }
    }

    void TAngelSE::UpdateHeading() {
        switch (MoveState) {
            case 0: Wander(); break;
            case 2: Wander(); break;
            case 1: SeekTarget(); break;
            default: Wander(); break;
        }
        Velocity = aMyFunction::PointFromRadiusAngle(Speed, MoveAngle);
    }

    void TAngelSE::AdvanceSteps(std::int32_t Count) {
        PAngelEntry Entry{};
        while (Count > 0) {
            ++FrameIndex;
            {
                std::int32_t cpp_right = GetFrameCount(nullptr);
                if (FrameIndex >= cpp_right) {
                    FrameIndex = 0;
                }
            }
            UpdateHeading();
            Entry = FirstEntry;
            while (Entry != nullptr) {
                AdvanceEntry(Entry);
                Entry = Entry->Next;
            }
            ToggleFrameVariants();
            Position = EC_Struct::MakePointF(static_cast<long double>(Position.X) + Velocity.X, static_cast<long double>(Position.Y) + Velocity.Y);
            if (FirstEntry != nullptr) {
                SetPosition(Position);
            }
            --Count;
        }
    }

    void TAngelSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        Types::TPoint IntRange{};
        EC_Struct::TPointF Range{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        SizeRange = ClassesImports::Point(32, 64);
        GroupSize = EC_Struct::MakePointF(32.0f, 64.0f);
        MoveAngle = 0.0f;
        FrameIndex = 0;
        EntryCount = 4;
        TimerInterval = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Time"sv)));
        if (Block->CountParams(u"Size"_wref.get()) > 0) {
            SizeRange = GI_Main::GetPointGI(pas::view(Block->GetParam(u"Size"sv)));
        }
        if (Block->CountParams(u"Speed"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"Speed"sv)));
            Speed = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"MoveAngle"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"MoveAngle"sv)));
            MoveAngle = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        }
        if (Block->CountParams(u"WorldPos"_wref.get()) > 0) {
            Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"WorldPos"sv)));
            {
                float randomFloatRange = aMyFunction::RandomFloatRange(Range.X, Range.Y);
                float randomFloatRange_2 = aMyFunction::RandomFloatRange(Range.X, Range.Y);
                Position = EC_Struct::MakePointF(randomFloatRange, randomFloatRange_2);
            }
        }
        if (Block->CountParams(u"GroupSize"_wref.get()) > 0) {
            GroupSize = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"GroupSize"sv)));
        }
        if (Block->CountParams(u"AngelCount"_wref.get()) > 0) {
            IntRange = GI_Main::GetPointGI(pas::view(Block->GetParam(u"AngelCount"sv)));
            EntryCount = aMyFunction::RandomIntRange(IntRange.X, IntRange.Y);
        }
        ImageCount = 0;
        while (Block->CountParams(pas::concat_wide({u"Image", EC_Str::IntToWideString(ImageCount)})) > 0) {
            ImagePaths[ImageCount] = Block->GetParam(pas::view(pas::concat_wide({u"Image", EC_Str::IntToWideString(ImageCount)})));
            ++ImageCount;
            if (ImageCount == 8) {
                break;
            }
        }
        SetVelocityFromAngle(MoveAngle);
    }

    void TAngelSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TAngelSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_RotateImage5::TRotateImage5GI* Control = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, Owner);
        GI_RotateImage5::TRotateImage5GI::QueueImagePath(PendingLoads, ImagePath);
        pas::free(Control);
    }

} // namespace SE_Angel
