#include "layout/SE_Asteroid.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Image.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SE_Asteroid.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Asteroid {
    void TAsteroidSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
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
        Animation->SetSequenceFrame(SavedSequenceFrameIndex);
        Animation->RestartPlayback();
        Size = Animation->ClientSize;
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

    void TAsteroidSE::DetachFromSpace() {
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

    void TAsteroidSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            MapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
        }
    }

    std::int32_t TAsteroidSE::GetSequenceFrameIndex() {
        if (Animation == nullptr) {
            return SavedSequenceFrameIndex;
        }
        return Animation->SequenceFrame;
    }

    void TAsteroidSE::SetSequenceFrameIndex(std::int32_t FrameIndex) {
        SavedSequenceFrameIndex = FrameIndex;
        if (Animation != nullptr) {
            Animation->SetSequenceFrame(FrameIndex);
        }
    }

    // Requires an attached space.
    void TAsteroidSE::DrawMap() {
        SE_Process::TProcessSE* cpp_with = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, cpp_with->RadarCenter)) < pas::sqr(cpp_with->RadarRange)) {
            MapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
    }

    std::uint8_t TAsteroidSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Animation->HitTestCursor();
    }

    void TAsteroidSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"_wref.get());
        MapImagePath = Block->GetParam(u"ImageMap"_wref.get());
    }

    void TAsteroidSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TAsteroidSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
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

} // namespace SE_Asteroid
