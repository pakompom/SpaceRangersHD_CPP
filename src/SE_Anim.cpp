#include "layout/SE_Anim.hpp"
#include "types/Classes.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/SE_Anim.hpp"
#include "units/SE_Space.hpp"

namespace SE_Anim {
    void TAnimSE_Destroy(TAnimSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TAnimSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (!IsAttachedToSpace()) {
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
            Animation->SetImagePath(ImagePath);
            Animation->SequenceIndex = 0;
            Animation->UpdateAutoGeometry();
            Animation->SetPositionModeW(true);
            Animation->SetDepthByName(DepthExpression);
            Animation->SetPosition(EC_Struct::TruncatePointF(Position));
            Animation->SetOrigin(ImageOrigin);
            {
                WindowsSdk::TPoint contentSize = Animation->GetContentSize();
                GI_GAI::TgaiGI* animation = Animation;
                animation->SetSize(contentSize);
            }
            Animation->CycleCompleteCallback = pas::bind_method<&TAnimSE::AnimationCycleComplete>(this);
            Animation->RestartPlayback();
        }
    }

    void TAnimSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            Animation->SetActive(false);
            pas::free(Animation);
            Animation = nullptr;
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TAnimSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Animation->SetPosition(EC_Struct::TruncatePointF(Position));
        }
    }

    void TAnimSE::AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (!LoopAnimation) {
            DetachFromSpace();
            if (pas::assigned(FinishedCallback)) {
                FinishedCallback(this);
            }
        }
    }

    std::uint8_t TAnimSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        return Animation->HitTestCursor();
    }

    void TAnimSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"_wref.get());
        if (Block->CountParams(u"LoopAnim"_wref.get()) > 0) {
            LoopAnimation = GI_Main::ParseEnabledNameGI(Block->GetParam(u"LoopAnim"_wref.get()));
        }
        if (Block->CountParams(u"SmeImage"_wref.get()) > 0) {
            ImageOrigin = GI_Main::GetPointGI(Block->GetParam(u"SmeImage"_wref.get()));
        }
    }

    void TAnimSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
        if (Block->CountParams(u"LoopAnim"_wref.get()) > 0) {
            LoopAnimation = GI_Main::ParseEnabledNameGI(Block->GetParam(u"LoopAnim"_wref.get()));
        }
        if (Block->CountParams(u"SmeImage"_wref.get()) > 0) {
            ImageOrigin = GI_Main::GetPointGI(Block->GetParam(u"SmeImage"_wref.get()));
        }
    }

    void TAnimSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_GAI::TgaiGI* Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
        Image->SetImagePath(ImagePath);
        Image->QueueImageLoad(PendingLoads);
        pas::free(Image);
    }

    void TAnimSE::p_destroy() {
        SE_Anim::TAnimSE_Destroy(this);
    }

} // namespace SE_Anim
