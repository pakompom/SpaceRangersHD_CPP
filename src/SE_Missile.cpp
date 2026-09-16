#include "layout/SE_Missile.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/GR_Main.hpp"
#include "units/SE_Missile.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Missile {
    void TMissileSE_Destroy(TMissileSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    // Native diagnostic name: TMissileSE.Connect.
    void TMissileSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        std::int32_t Stage = 0;
        try {
            if (IsAttachedToSpace()) {
                return;
            }
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            Stage = 1;
            Image = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, nullptr);
            Stage = 2;
            if (Space->MapPanel != nullptr) {
                Space->MapPanel->AttachOwnedChild(Image);
            }
            Stage = 3;
            Image->SetPositionModeW(true);
            Image->SetDepthByName(DepthExpression);
            {
                std::int32_t trunc = System::Trunc(Position.Y);
                std::int32_t trunc_2 = System::Trunc(Position.X);
                Image->SetPosition(ClassesImports::Point(trunc_2, trunc));
            }
            Image->SetAngle(Angle);
            Image->SetAlpha(255);
            Stage = 4;
            {
                std::int32_t round_3 = System::Round(ImageScale * 16.0L);
                std::int32_t round_4 = System::Round(ImageScale * 16.0L);
                Types::TPoint point = ClassesImports::Point(round_4, round_3);
                std::int32_t round = System::Round(ImageScale * 32.0L);
                std::int32_t round_2 = System::Round(ImageScale * 32.0L);
                Types::TPoint point_2 = ClassesImports::Point(round_2, round);
                pas::WideString cpp_arg = pas::concat_wide({u"Bm.", GraphKey});
                Image->SetImage(std::move(cpp_arg), point_2, point);
            }
            Stage = 5;
            Image->SetFrameIndex(0u);
            AnimationTimer = Space->CreateTimer(50, 50, pas::bind_method<&TMissileSE::AdvanceAnimationTimer>(this), 0);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe("TMissileSE.Connect"_a);
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(GraphKey));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"lastLabel=", EC_Str::IntToWideString(GI_RotateImage5::RotateImageConstructionStage)})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"self=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(this)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"sp=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(ASpace)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"FSpace=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Space)))})));
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"FImage=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Image)))})));
                if (Space != nullptr) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"PGI=", EC_Str::IntToWideString(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Space->MapPanel)))})));
                }
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TMissileSE.Connect, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TMissileSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (AnimationTimer != nullptr) {
            Space->DeleteTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        Image->SetActive(false);
        pas::free(Image);
        Image = nullptr;
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TMissileSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            std::int32_t trunc = System::Trunc(APosition.Y);
            std::int32_t trunc_2 = System::Trunc(APosition.X);
            Image->SetPosition(ClassesImports::Point(trunc_2, trunc));
        }
    }

    std::uint8_t TMissileSE::GetAngle() {
        return Angle;
    }

    void TMissileSE::SetAngle(std::uint8_t Value) {
        Angle = Value;
        if (IsAttachedToSpace()) {
            Image->SetAngle(Angle);
        }
    }

    std::uint8_t TMissileSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Image->HitTestPixel(Image->MessageLoop->GetCursorPoint());
    }

    void TMissileSE::AdvanceAnimationTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        Types::PRect Bounds = reinterpret_cast<Types::PRect>(&Image->HitTestBounds);
        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) * -0.1L <= Bounds->Right && static_cast<std::uint32_t>(GR_Main::GameScreenWidth) * 1.1L >= Bounds->Left && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) * -0.1L <= Bounds->Bottom && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) * 1.1L >= Bounds->Top) {
            Image->SetFrameIndex(Image->FrameIndex + 1);
            {
                std::int32_t cpp_left = Image->GetFrameCount();
                if (cpp_left <= static_cast<std::int32_t>(Image->FrameIndex)) {
                    Image->SetFrameIndex(0u);
                }
            }
        }
    }

    void TMissileSE::DrawMap() {
        std::int32_t X{};
        std::int32_t Y{};
        SE_Process::TProcessSE* CurrentProcess = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, CurrentProcess->RadarCenter)) < pas::sqr(CurrentProcess->RadarRange)) {
            X = System::Round(static_cast<long double>(Position.X) * Space->MinimapScale) + pas::shr(GR_Main::RenderScratchBuffer->Width, 1);
            Y = System::Round(static_cast<long double>(Position.Y) * Space->MinimapScale) + pas::shr(GR_Main::RenderScratchBuffer->Height, 1);
            if (X >= 0 && GR_Main::RenderScratchBuffer->Width > X && Y >= 0 && GR_Main::RenderScratchBuffer->Height > Y) {
                GR_Main::RenderScratchBuffer->SetPixel16(X, Y, GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0));
            }
        }
    }

    void TMissileSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        SetAngle(0);
        if (Block->CountParams(u"Scale"_wref.get()) > 0) {
            ImageScale = EC_Str::ExtractDecimalToSingleW(Block->GetParam(u"Scale"_wref.get()));
        } else {
            ImageScale = 1.0f;
        }
    }

    // Native empty override.
    void TMissileSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
    }

    // Native empty override.
    void TMissileSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    void TMissileSE::p_destroy() {
        SE_Missile::TMissileSE_Destroy(this);
    }

} // namespace SE_Missile
