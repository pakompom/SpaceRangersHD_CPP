#include "layout/SE_Hole.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Image.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SE_Hole.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Hole {
    void THoleSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        ConfigureLoopSound(u"Hole"_wref.get());
        ConfigureRandomSound(u"Hole"_wref.get());
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
        if (State == 1) {
            Animation->SetImagePath(ImagePath);
        } else {
            Animation->SetImagePath(ImagePath);
        }
        Animation->CycleCompleteCallback = pas::bind_method<&THoleSE::AnimationCycleComplete>(this);
        {
            WindowsSdk::TPoint contentSize = Animation->GetContentSize();
            GI_GAI::TgaiGI* animation = Animation;
            animation->SetSize(contentSize);
        }
        Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
        Animation->SetDepthByName(DepthExpression);
        Animation->SetPosition(EC_Struct::TruncatePointF(Position));
        Animation->SetPositionModeW(true);
        if (State == 1) {
            Animation->SequenceIndex = 0;
        } else {
            Animation->SequenceIndex = 1;
        }
        Animation->UpdateAutoGeometry();
        if (State == 1) {
            SavedSequenceFrameIndex = 0;
        }
        Animation->SetSequenceFrame(SavedSequenceFrameIndex);
        Animation->RestartPlayback();
        MapImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
        MapImage->SetPositionModeW(true);
        MapImage->SetDepthByName(DepthExpression);
        MapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
        MapImage->SetImagePath(MapImagePath);
        {
            Types::TPoint contentSize_2 = MapImage->GetContentSize();
            GI_Image::TImageGI* mapImage = MapImage;
            mapImage->SetSize(contentSize_2);
        }
        {
            Types::TPoint halfPoint = EC_Struct::HalfPoint(MapImage->GetContentSize());
            GI_Image::TImageGI* mapImage_2 = MapImage;
            mapImage_2->SetOrigin(halfPoint);
        }
    }

    void THoleSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (Animation != nullptr) {
            SavedSequenceFrameIndex = Animation->SequenceFrame;
            pas::free(Animation);
            Animation = nullptr;
        }
        if (MapImage != nullptr) {
            pas::free(MapImage);
            MapImage = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void THoleSE::SetState(std::int32_t Value) {
        if (State == 1 && Value == 0 && IsAttachedToSpace()) {
            return;
        }
        State = Value;
    }

    void THoleSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            MapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
        }
    }

    void THoleSE::DrawMap() {
        SE_Process::TProcessSE* cpp_with = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, cpp_with->RadarCenter)) < pas::sqr(cpp_with->RadarRange)) {
            MapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
    }

    std::uint8_t THoleSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Animation->HitTestCursor();
    }

    void THoleSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"_wref.get());
        MapImagePath = Block->GetParam(u"ImageMap"_wref.get());
        if (Block->CountParams(u"Radius"_wref.get()) > 0) {
            HitRadius = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"Radius"_wref.get()));
        } else {
            HitRadius = 80;
        }
        if (Block->CountParams(u"GalaxyImage"_wref.get()) > 0) {
            GalaxyImagePath = Block->GetParam(u"GalaxyImage"_wref.get());
        } else {
            GalaxyImagePath = u"GI,Bm.FormGalaxy.BlackHole"_w;
        }
        if (Block->CountParams(u"GalaxyPriority"_wref.get()) > 0) {
            GalaxyPriority = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"GalaxyPriority"_wref.get()));
        } else {
            GalaxyPriority = 80;
        }
        if (Block->CountParams(u"NamePath"_wref.get()) > 0) {
            NameTextPath = Block->GetParam(u"NamePath"_wref.get());
        } else {
            NameTextPath = u"FormInfo.HoleName"_w;
        }
        if (Block->CountParams(u"TextPath"_wref.get()) > 0) {
            InfoTextPath = Block->GetParam(u"TextPath"_wref.get());
        } else {
            InfoTextPath = u"FormInfo.HoleText"_w;
        }
    }

    void THoleSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void THoleSE::AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (State == 1) {
            Animation->SetImagePath(ImagePath);
            {
                WindowsSdk::TPoint contentSize = Animation->GetContentSize();
                GI_GAI::TgaiGI* animation = Animation;
                animation->SetSize(contentSize);
            }
            Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
            Animation->SequenceIndex = 1;
            Animation->UpdateAutoGeometry();
            Animation->SetSequenceFrame(0);
            Animation->RestartPlayback();
        } else if (State == 2) {
            if (Animation->SequenceIndex == 2) {
                DetachFromSpace();
            } else {
                Animation->SetImagePath(ImagePath);
                {
                    WindowsSdk::TPoint contentSize_2 = Animation->GetContentSize();
                    GI_GAI::TgaiGI* animation_2 = Animation;
                    animation_2->SetSize(contentSize_2);
                }
                Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
                Animation->SequenceIndex = 2;
                Animation->UpdateAutoGeometry();
                Animation->SetSequenceFrame(0);
                Animation->RestartPlayback();
            }
        }
    }

    void THoleSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        {
            GI_GAI::TgaiGI* cpp_with = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
            cpp_with->SetImagePath(ImagePath);
            cpp_with->QueueImageLoad(PendingLoads);
            pas::free(cpp_with);
        }
        {
            GI_Image::TImageGI* cpp_with_2 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
            cpp_with_2->SetImagePath(MapImagePath);
            cpp_with_2->QueueImageLoad(PendingLoads);
            pas::free(cpp_with_2);
        }
    }

} // namespace SE_Hole
