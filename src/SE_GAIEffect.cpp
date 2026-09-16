#include "layout/SE_GAIEffect.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_Sound.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_GAIEffect.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"

namespace SE_GAIEffect {
    void TGAIEffectSE_Create(TGAIEffectSE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition) {
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
        Self->DurationScale = 1.0f;
    }

    void TGAIEffectSE_Destroy(TGAIEffectSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TGAIEffectSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        std::int32_t StepDuration{};
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
        Animation->SetPositionModeW(true);
        Animation->SetPosition(ImagePosition);
        Animation->SequenceIndex = 0;
        Animation->UpdateAutoGeometry();
        Animation->StopAutoPlayback();
        StepsPerFrame = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(DurationScale * 2.0E+2L, Animation->SequenceFrameCount)));
        if (GlobalsV::FilmSpeed == 0) {
            StepDuration = 18;
        } else if (GlobalsV::FilmSpeed == 1) {
            StepDuration = 14;
        } else {
            StepDuration = 10;
        }
        std::int32_t Duration = Animation->SequenceFrameCount * StepDuration * StepsPerFrame;
        Animation->SetOneCycleDuration(Duration);
        if (Globals::FilmSoundEffectsEnabled) {
            if (GlobalsV::SoundInSpaceEnabled) {
                if (Space->ContainsMapPoint(EC_Struct::PointToPointF(Animation->LocalPosition))) {
                    GR_Main::SoundManager->PlaySound(SoundPath);
                }
            }
        }
        StepIndex = 0;
    }

    void TGAIEffectSE::DetachFromSpace() {
        if (Animation != nullptr) {
            pas::free(Animation);
            Animation = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TGAIEffectSE::SetImagePosition(Types::TPoint Value) {
        ImagePosition = Value;
    }

    void TGAIEffectSE::SetDurationScale(float Value) {
        DurationScale = Value;
    }

    void TGAIEffectSE::Advance() {
        if (IsAttachedToSpace()) {
            if (Animation != nullptr) {
                if (Animation->SequenceFrame == Animation->SequenceFrameCount - 1) {
                    pas::free(Animation);
                    Animation = nullptr;
                    DetachFromSpace();
                } else if (pas::imod(StepIndex, StepsPerFrame) == 0) {
                    Animation->SetSequenceFrame(Animation->SequenceFrame + 1);
                }
            }
            ++StepIndex;
        }
    }

    void TGAIEffectSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"GAI"_wref.get());
        if (Block->CountParams(u"Sound"_wref.get()) > 0) {
            SoundPath = Block->GetParam(u"Sound"_wref.get());
        }
    }

    void TGAIEffectSE::p_destroy() {
        SE_GAIEffect::TGAIEffectSE_Destroy(this);
    }

} // namespace SE_GAIEffect
