#include "layout/SE_Meteorite.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Process.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SE_Meteorite.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Meteorite {
    void TMeteoriteSE_Create(TMeteoriteSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition) {
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
    }

    void TMeteoriteSE_Destroy(TMeteoriteSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TMeteoriteSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
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
        Animation->RestartPlayback();
        PlaceRandomly();
        MoveTimer = Space->CreateTimer(TimerInterval, TimerInterval, pas::bind_method<&TMeteoriteSE::AdvanceMotion>(this), 0);
    }

    void TMeteoriteSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (MoveTimer != nullptr) {
            Space->DeleteTimer(MoveTimer);
            MoveTimer = nullptr;
        }
        if (Animation != nullptr) {
            pas::free(Animation);
            Animation = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TMeteoriteSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
        }
    }

    std::uint8_t TMeteoriteSE::IsNearView(EC_Struct::TPointF Point) {
        float Width = static_cast<std::uint32_t>(GR_Main::GameScreenWidth);
        float Height = static_cast<std::uint32_t>(GR_Main::GameScreenHeight);
        return static_cast<long double>(Globals::SpaceViewPosition.X) - Width < Point.X && static_cast<long double>(Globals::SpaceViewPosition.X) + Width > Point.X && static_cast<long double>(Globals::SpaceViewPosition.Y) - Height < Point.Y && static_cast<long double>(Globals::SpaceViewPosition.Y) + Height > Point.Y;
    }

    void TMeteoriteSE::PlaceRandomly() {
        float Radius{};
        std::int32_t Bound{};
        if (Space != nullptr) {
            Radius = reinterpret_cast<SE_Process::TProcessSE*>(Space->Process)->SystemRadius;
            Bound = System::Round(Radius);
            {
                float randomIntRange = aMyFunction::RandomIntRange(-Bound, Bound);
                float randomIntRange_2 = aMyFunction::RandomIntRange(-Bound, Bound);
                SetPosition(EC_Struct::MakePointF(randomIntRange, randomIntRange_2));
            }
        }
    }

    void TMeteoriteSE::RestartOutsideView() {
        float Radius{};
        std::int32_t Bound{};
        EC_Struct::TPointF StartPoint{};
        EC_Struct::TPointF EndPoint{};
        EC_Struct::TPointF Intersection{};
        if (Space != nullptr) {
            Radius = reinterpret_cast<SE_Process::TProcessSE*>(Space->Process)->SystemRadius;
            do {
                Bound = System::Round(Radius);
                {
                    float randomIntRange = aMyFunction::RandomIntRange(-Bound, Bound);
                    float randomIntRange_2 = aMyFunction::RandomIntRange(-Bound, Bound);
                    EndPoint = EC_Struct::MakePointF(randomIntRange, randomIntRange_2);
                }
                {
                    float cpp_arg = EndPoint.X + System::Sin(SystemImports::Pi + Angle) * (Radius * 4.0L);
                    float cpp_arg_2 = EndPoint.Y - System::Cos(SystemImports::Pi + Angle) * (Radius * 4.0L);
                    StartPoint = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
                }
            } while (!(aMyFunction::SegmentIntersectsRectEdges(StartPoint, EndPoint, EC_Struct::MakePointF(-Radius * 1.2L, -Radius * 1.2L), EC_Struct::MakePointF(1.2L * Radius, 1.2L * Radius), Intersection) && static_cast<std::uint8_t>(TMeteoriteSE::IsNearView(Intersection) ^ 1)));
            SetPosition(Intersection);
        }
    }

    void TMeteoriteSE::AdvanceMotion(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        {
            float cpp_arg = Position.X + System::Sin(Angle) * Speed;
            float cpp_arg_2 = Position.Y - System::Cos(Angle) * Speed;
            SetPosition(EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
        }
        float Limit = reinterpret_cast<SE_Process::TProcessSE*>(Space->Process)->SystemRadius * 1.3L;
        if ((-Limit > Position.X || Position.X > Limit || -Limit > Position.Y || Position.Y > Limit) && static_cast<std::uint8_t>(TMeteoriteSE::IsNearView(Position) ^ 1)) {
            RestartOutsideView();
        }
    }

    void TMeteoriteSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        EC_Struct::TPointF Range{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"sv);
        TimerInterval = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParam(u"Time"sv)));
        Range = GI_Main::GetFloatPointGI(pas::view(Block->GetParam(u"Speed"sv)));
        Speed = aMyFunction::RandomFloatRange(Range.X, Range.Y);
        Angle = aMyFunction::HeadingDegreesToRadians(EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Angle"sv)));
    }

    void TMeteoriteSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TMeteoriteSE::p_destroy() {
        SE_Meteorite::TMeteoriteSE_Destroy(this);
    }

} // namespace SE_Meteorite
