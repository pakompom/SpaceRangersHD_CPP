#include "layout/SE_Container.hpp"
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
#include "units/GI_AlphaImage.hpp"
#include "units/GI_GAI.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/SE_Container.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Container {
    void TContainerSE::CopyTo(SE_Space::TObjectSE* Destination) {
        SE_Space::TObjectSE::CopyTo(Destination);
        pas::checked_cast<TContainerSE*>(Destination)->ImagePath = ImagePath;
        pas::checked_cast<TContainerSE*>(Destination)->MinimapImagePath = MinimapImagePath;
    }

    void TContainerSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (!IsAttachedToSpace()) {
            ConfigureLoopSound(u"Container"_wref.get());
            ConfigureRandomSound(u"Container"_wref.get());
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            Animation->SetImagePath(ImagePath);
            Animation->SequenceIndex = 0;
            Animation->UpdateAutoGeometry();
            Animation->SetPositionModeW(true);
            Animation->SetDepthByName(DepthExpression);
            Animation->SetPosition(EC_Struct::TruncatePointF(Position));
            {
                WindowsSdk::TPoint contentSize = Animation->GetContentSize();
                GI_GAI::TgaiGI* animation = Animation;
                animation->SetSize(contentSize);
            }
            Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
            {
                std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, Animation->SequenceFrameCount - 1);
                GI_GAI::TgaiGI* animation_2 = Animation;
                animation_2->SetSequenceFrame(randomIntRange);
            }
            Animation->RestartPlayback();
            MinimapImage = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
            MinimapImage->SetPositionModeW(true);
            MinimapImage->SetDepthByName(DepthExpression);
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
            MinimapImage->SetImagePath(MinimapImagePath);
            {
                WindowsSdk::TPoint contentSize_2 = MinimapImage->GetContentSize();
                GI_MessageLoop::TObjectGI* minimapImage = MinimapImage;
                minimapImage->SetSize(contentSize_2);
            }
            MinimapImage->SetOrigin(EC_Struct::HalfPoint(MinimapImage->ClientSize));
        }
    }

    void TContainerSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            if (Animation != nullptr) {
                pas::free(Animation);
                Animation = nullptr;
            }
            if (MinimapImage != nullptr) {
                pas::free(MinimapImage);
                MinimapImage = nullptr;
            }
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TContainerSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
        }
    }

    void TContainerSE::SetDepth(float Value) {
        Animation->SetDepth(Value);
    }

    float TContainerSE::GetDepth() {
        return Animation->Depth;
    }

    std::uint8_t TContainerSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Animation->HitTestCursor();
    }

    void TContainerSE::DrawMap() {
        SE_Process::TProcessSE* CurrentProcess = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (static_cast<long double>(aMyFunction::PointDistanceSquared(Position, CurrentProcess->RadarCenter)) < pas::sqr(CurrentProcess->RadarRange)) {
            MinimapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
    }

    void TContainerSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(pas::concat_wide({GR_Main::GiResourceSuffix(), u"Image"}));
        MinimapImagePath = Block->GetParam(u"ImageMap"_wref.get());
    }

    void TContainerSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_GAI::TgaiGI* Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        Image->SetImagePath(ImagePath);
        Image->QueueImageLoad(PendingLoads);
        pas::free(Image);
        GI_AlphaImage::TAlphaImageGI* MapImage = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Owner);
        MapImage->SetImagePath(MinimapImagePath);
        MapImage->QueueImageLoad(PendingLoads);
        pas::free(MapImage);
    }

} // namespace SE_Container
