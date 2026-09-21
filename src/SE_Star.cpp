#include "layout/SE_Star.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Star.hpp"

namespace SE_Star {
    void TStarSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        ConfigureLoopSound(u"Star"_wref.get());
        ConfigureRandomSound(u"Star"_wref.get());
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        if (!GlobalsV::AnimStar) {
            StaticImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
            StaticImage->SetImagePath(StaticImagePath);
            StaticImage->SetPositionModeW(true);
            StaticImage->SetDepthByName(DepthExpression);
            StaticImage->SetPosition(EC_Struct::TruncatePointF(Position));
            {
                Types::TPoint contentSize = StaticImage->GetContentSize();
                GI_Image::TImageGI* staticImage = StaticImage;
                staticImage->SetSize(contentSize);
            }
            StaticImage->SetOrigin(EC_Struct::HalfPoint(StaticImage->ClientSize));
        } else {
            Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            Animation->SetImagePath(AnimationPath);
            {
                const pas::WideString& cpp_arg = pas::concat_wide({u"[65,0-", EC_Str::IntToWideString(Animation->GetMainImageFrameCount() - 1), u"]"});
                GI_GAI::TgaiGI* animation = Animation;
                animation->LoadFrameSequenceFromText(cpp_arg);
            }
            Animation->SetPositionModeW(true);
            Animation->SetDepthByName(DepthExpression);
            Animation->SetPosition(EC_Struct::TruncatePointF(Position));
            {
                WindowsSdk::TPoint contentSize_2 = Animation->GetContentSize();
                GI_GAI::TgaiGI* animation_2 = Animation;
                animation_2->SetSize(contentSize_2);
            }
            Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
            Animation->SetSequenceFrame(SavedSequenceFrameIndex);
            Animation->RestartPlayback();
        }
        MapImage = pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        MapImage->SetPositionModeW(true);
        MapImage->SetDepthByName(DepthExpression);
        MapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
        MapImage->SetOrigin(MapImageOrigin);
        MapImage->SetImagePath(MapImagePath);
        {
            WindowsSdk::TPoint contentSize_3 = MapImage->GetContentSize();
            GI_MessageLoop::TObjectGI* mapImage = MapImage;
            mapImage->SetSize(contentSize_3);
        }
    }

    void TStarSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (Animation != nullptr) {
            SavedSequenceFrameIndex = Animation->SequenceFrame;
            pas::free(Animation);
            Animation = nullptr;
        }
        if (StaticImage != nullptr) {
            pas::free(StaticImage);
            StaticImage = nullptr;
        }
        if (MapImage != nullptr) {
            pas::free(MapImage);
            MapImage = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TStarSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            if (StaticImage != nullptr) {
                StaticImage->SetPosition(EC_Struct::TruncatePointF(APosition));
            }
            if (Animation != nullptr) {
                Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            }
            MapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
        }
    }

    std::int32_t TStarSE::GetSequenceFrameIndex() {
        if (Animation == nullptr) {
            return SavedSequenceFrameIndex;
        }
        return Animation->SequenceFrame;
    }

    void TStarSE::SetSequenceFrameIndex(std::int32_t FrameIndex) {
        SavedSequenceFrameIndex = FrameIndex;
        if (Animation != nullptr) {
            Animation->SetSequenceFrame(FrameIndex);
        }
    }

    std::uint8_t TStarSE::HitTestCursor() {
        std::uint8_t Result = false;
        if (!IsAttachedToSpace()) {
            return false;
        }
        if (StaticImage != nullptr) {
            Result = StaticImage->HitTestCursor();
        }
        if (Animation != nullptr) {
            return Animation->HitTestCursor();
        }
        return Result;
    }

    void TStarSE::DrawMap() {
        MapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
    }

    void TStarSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        StaticImagePath = Block->GetParam(u"Image"sv);
        AnimationPath = Block->GetParam(u"Anim"sv);
        MapImagePath = Block->GetParam(u"ImageMap"sv);
        ImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImage"sv)));
        MapImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImageMap"sv)));
    }

    void TStarSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        if (!GlobalsV::AnimStar) {
            GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
            cpp_with->SetImagePath(StaticImagePath);
            cpp_with->QueueImageLoad(PendingLoads);
            pas::free(cpp_with);
        } else {
            GI_GAI::TgaiGI* cpp_with_2 = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
            cpp_with_2->SetImagePath(AnimationPath);
            cpp_with_2->QueueImageLoad(PendingLoads);
            pas::free(cpp_with_2);
        }
        {
            GI_GI::TgiGI* cpp_with_3 = pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, Owner);
            cpp_with_3->SetImagePath(MapImagePath);
            cpp_with_3->QueueImageLoad(PendingLoads);
            pas::free(cpp_with_3);
        }
    }

} // namespace SE_Star
