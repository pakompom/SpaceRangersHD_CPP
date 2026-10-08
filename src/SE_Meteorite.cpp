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
        MoveTimer = Space->CreateTimer(TimerInterval, TimerInterval,
                pas::bind_method<&TMeteoriteSE::AdvanceMotion>(this), 0);
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
        return static_cast<float>(static_cast<double>(Globals::SpaceViewPosition.X) - Width) < Point.X &&
            static_cast<float>(static_cast<double>(Globals::SpaceViewPosition.X) + Width) > Point.X &&
            static_cast<float>(static_cast<double>(Globals::SpaceViewPosition.Y) - Height) < Point.Y &&
            static_cast<float>(static_cast<double>(Globals::SpaceViewPosition.Y) + Height) > Point.Y;
    }

    void TMeteoriteSE::PlaceRandomly() {
        float Radius{};
        std::int32_t Bound{};
        if (Space != nullptr) {
            Radius = reinterpret_cast<SE_Process::TProcessSE*>(Space->Process)->SystemRadius;
            Bound = pas::real_round(Radius);
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
                Bound = pas::real_round(Radius);
                {
                    float randomIntRange = aMyFunction::RandomIntRange(-Bound, Bound);
                    float randomIntRange_2 = aMyFunction::RandomIntRange(-Bound, Bound);
                    EndPoint = EC_Struct::MakePointF(randomIntRange, randomIntRange_2);
                }
                StartPoint = EC_Struct::MakePointF(static_cast<float>(EndPoint.X + static_cast<float>(System::Sin(static_cast<float>(SystemImports::Pi + Angle)) * (static_cast<float>(Radius * 4.0)))),
                        static_cast<float>(EndPoint.Y - static_cast<float>(System::Cos(static_cast<float>(SystemImports::Pi + Angle)) * (static_cast<float>(Radius * 4.0)))));
            } while (!(aMyFunction::SegmentIntersectsRectEdges(StartPoint, EndPoint,
                EC_Struct::MakePointF(static_cast<float>(-Radius * 0x1.3333333333334p+0), static_cast<float>(-Radius * 0x1.3333333333334p+0)), EC_Struct::MakePointF(static_cast<float>(0x1.3333333333334p+0 * Radius), static_cast<float>(0x1.3333333333334p+0 * Radius)),
                Intersection) && !TMeteoriteSE::IsNearView(Intersection)));
            SetPosition(Intersection);
        }
    }

    void TMeteoriteSE::AdvanceMotion(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        SetPosition(EC_Struct::MakePointF(static_cast<float>(Position.X + static_cast<float>(System::Sin(Angle) * Speed)),
            static_cast<float>(Position.Y - static_cast<float>(System::Cos(Angle) * Speed))));
        float Limit = static_cast<float>(reinterpret_cast<SE_Process::TProcessSE*>(Space->Process)->SystemRadius * 0x1.4ccccccccccccp+0);
        if ((-Limit > Position.X || Position.X > Limit || -Limit > Position.Y || Position.Y > Limit) &&
            !TMeteoriteSE::IsNearView(Position)) {
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
