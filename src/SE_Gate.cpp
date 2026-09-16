#include "layout/SE_Gate.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_RotateImageGAI.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Gate.hpp"
#include "units/SE_Space.hpp"
#include "units/System.hpp"

namespace SE_Gate {
    void TGateSE_Create(TGateSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition) {
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
        Self->State = 0;
        Self->TextRed = 1.0f;
        Self->TextGreen = 1.0f;
        Self->TextBlue = 1.0f;
    }

    void TGateSE_Destroy(TGateSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TGateSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        ConfigureLoopSound(u"Gate"_wref.get());
        ConfigureRandomSound(u"Gate"_wref.get());
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        TickCount = 0;
        Image = pas::construct_call<GI_RotateImageGAI::TRotateImageGaiGI>(GI_RotateImageGAI::TRotateImageGaiGI_Create, Space->MapPanel);
        Image->SetPositionModeW(true);
        Image->SetDepthByName(DepthExpression);
        {
            std::int32_t round = System::Round(Position.Y);
            std::int32_t round_2 = System::Round(Position.X);
            Image->SetPosition(ClassesImports::Point(round_2, round));
        }
        Image->SetAngle(Angle + 128);
        Image->SetAlpha(255);
        std::int32_t ImageSize = GR_Main::GiScalePixels(std::min<std::int32_t>(200, Size.X));
        Image->SetImage(u"Bm.Gate2.00?NoConvertPF"_w, ClassesImports::Point(ImageSize, ImageSize), ClassesImports::Point(ImageSize / 2, ImageSize / 2));
        TextLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Space->MapPanel);
        TextLabel->SetActive(false);
        TextLabel->SetFontName(GlobalsV::NormalFontName);
        TextLabel->SetPositionModeW(true);
        TextLabel->SetDepthByName(DepthExpression);
        TextLabel->SetSize(ClassesImports::Point(150, 20));
        {
            std::int32_t round_3 = System::Round(Position.Y + 5.0E+1L + ImageSize / 2 - 4.0E+1L);
            std::int32_t round_4 = System::Round(Position.X - pas::real_divide(TextLabel->ClientSize.X, 2.0L));
            TextLabel->SetPosition(ClassesImports::Point(round_4, round_3));
        }
        TextLabel->SetText(LabelText);
        TextLabel->SetWordWrapEnabled(false);
        TextLabel->SetTextAlignX(GI_Main::taxCenter);
        TextLabel->SetTextAlignY(GI_Main::tayAuto);
        RebuildStateGraphics();
    }

    void TGateSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            Image->SetActive(false);
            pas::free(Image);
            Image = nullptr;
            if (TextLabel != nullptr) {
                TextLabel->SetActive(false);
                pas::free(TextLabel);
                TextLabel = nullptr;
            }
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TGateSE::SetSize(Types::TPoint Value) {
        if (Size.X != Value.X || Size.Y != Value.Y) {
            SE_Space::TObjectSE::SetSize(Value);
        }
    }

    std::uint8_t TGateSE::GetAngle() {
        return Angle;
    }

    void TGateSE::SetAngle(std::uint8_t Value) {
        Angle = Value;
        if (IsAttachedToSpace()) {
            Image->SetAngle(Angle + 128);
        }
    }

    pas::WideString TGateSE::GetText() {
        return LabelText;
    }

    void TGateSE::SetText(const pas::WideString& Value) {
        LabelText = Value;
        if (IsAttachedToSpace()) {
            if (TextLabel != nullptr) {
                TextLabel->SetText(LabelText);
            }
        }
    }

    void TGateSE::Open() {
        if (State == 0) {
            State = 1;
            StateStep = 0;
            if (IsAttachedToSpace()) {
                RebuildStateGraphics();
            }
        }
    }

    void TGateSE::Close() {
        if (State == 2) {
            State = 3;
            StateStep = 0;
            if (IsAttachedToSpace()) {
                RebuildStateGraphics();
            }
        }
    }

    void TGateSE::SetState(std::int32_t Value) {
        State = Value;
        StateStep = 0;
        if (IsAttachedToSpace()) {
            RebuildStateGraphics();
        }
    }

    void TGateSE::RebuildStateGraphics() {
        if (State == 0) {
            Image->SetActive(false);
            if (TextLabel != nullptr) {
                TextLabel->SetActive(false);
            }
        } else if (State == 1) {
            Image->AnimationIndex = 0;
            Image->UpdateAutoGeometry();
            if (StateStep >= 0 && Image->FrameCount > StateStep) {
                Image->SetActive(true);
                Image->SetFrame(StateStep);
                if (TextLabel != nullptr) {
                    TextLabel->SetActive(false);
                }
            } else {
                Image->SetActive(false);
                if (TextLabel != nullptr) {
                    TextLabel->SetActive(false);
                }
            }
        } else if (State == 2) {
            Image->AnimationIndex = 1;
            Image->UpdateAutoGeometry();
            if (StateStep >= 0 && Image->FrameCount > StateStep) {
                Image->SetActive(true);
                Image->SetFrame(StateStep);
                if (TextLabel != nullptr) {
                    TextLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackNormalizedRgb(TextRed, TextGreen, TextBlue));
                    TextLabel->SetActive(true);
                }
            } else {
                Image->SetActive(false);
                if (TextLabel != nullptr) {
                    TextLabel->SetActive(false);
                }
            }
        } else if (State == 3) {
            Image->AnimationIndex = 2;
            Image->UpdateAutoGeometry();
            if (StateStep >= 0 && Image->FrameCount > StateStep) {
                Image->SetActive(true);
                Image->SetFrame(StateStep);
                if (TextLabel != nullptr) {
                    TextLabel->SetActive(false);
                }
            } else {
                Image->SetActive(false);
                if (TextLabel != nullptr) {
                    TextLabel->SetActive(false);
                }
            }
        }
    }

    void TGateSE::AdvanceAnimation(void* UnusedTimer, std::int32_t UnusedData) {
        ++StateStep;
        if (State == 0) {
            return;
        } else if (State == 1) {
            if (Image->FrameCount <= StateStep) {
                State = 2;
                StateStep = 0;
                RebuildStateGraphics();
            } else {
                Image->SetFrame(StateStep);
            }
        } else if (State == 2) {
            if (Image->FrameCount <= StateStep) {
                StateStep = 0;
                RebuildStateGraphics();
            } else {
                Image->SetFrame(StateStep);
            }
        } else if (State == 3) {
            if (Image->FrameCount <= StateStep) {
                State = 0;
                StateStep = 0;
                RebuildStateGraphics();
            } else {
                Image->SetFrame(StateStep);
            }
        }
    }

    void TGateSE::Advance() {
        SE_Space::TObjectSE::Advance();
        ++TickCount;
        if (TickCount % 5 == 0) {
            AdvanceAnimation(nullptr, 0);
        }
    }

    void TGateSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
    }

    void TGateSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TGateSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    void TGateEffectSE_Create(TGateEffectSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition) {
        SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
    }

    void TGateEffectSE_Destroy(TGateEffectSE* Self) {
        SE_Space::TObjectSE_Destroy(Self);
    }

    void TGateEffectSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        TickCount = 0;
        Image = pas::construct_call<GI_RotateImageGAI::TRotateImageGaiGI>(GI_RotateImageGAI::TRotateImageGaiGI_Create, Space->MapPanel);
        Image->SetPositionModeW(true);
        Image->SetDepthByName(DepthExpression);
        {
            std::int32_t round = System::Round(Position.Y);
            std::int32_t round_2 = System::Round(Position.X);
            Image->SetPosition(ClassesImports::Point(round_2, round));
        }
        Image->SetAngle(Angle + 128);
        Image->SetAlpha(255);
        Image->SetActive(true);
        std::int32_t ImageSize = GR_Main::GiScalePixels(std::min<std::int32_t>(200, Size.X));
        Image->SetImage(u"Bm.Gate2.GateEffect?NoConvertPF"_w, ClassesImports::Point(ImageSize, ImageSize), ClassesImports::Point(ImageSize / 2, ImageSize / 2));
        RebuildStateGraphics();
    }

    void TGateEffectSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            Image->SetActive(false);
            pas::free(Image);
            Image = nullptr;
            StateStep = 0;
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TGateEffectSE::SetSize(Types::TPoint Value) {
        if (Size.X != Value.X || Size.Y != Value.Y) {
            SE_Space::TObjectSE::SetSize(Value);
        }
    }

    std::uint8_t TGateEffectSE::GetAngle() {
        return Angle;
    }

    void TGateEffectSE::SetAngle(std::uint8_t Value) {
        Angle = Value;
        if (IsAttachedToSpace()) {
            Image->SetAngle(Angle + 128);
        }
    }

    void TGateEffectSE::RebuildStateGraphics() {
        Image->AnimationIndex = 0;
        Image->UpdateAutoGeometry();
        Image->SetActive(true);
        Image->SetFrame(StateStep);
    }

    void TGateEffectSE::AdvanceAnimation(void* UnusedTimer, std::int32_t UnusedData) {
        if (IsAttachedToSpace()) {
            ++StateStep;
            if (Image->FrameCount <= StateStep) {
                StateStep = 0;
                DetachFromSpace();
            } else {
                RebuildStateGraphics();
            }
        }
    }

    void TGateEffectSE::Advance() {
        SE_Space::TObjectSE::Advance();
        ++TickCount;
        if (TickCount % 5 == 0) {
            AdvanceAnimation(nullptr, 0);
        }
    }

    void TGateEffectSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
    }

    void TGateEffectSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TGateEffectSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
    }

    void TGateSE::p_destroy() {
        SE_Gate::TGateSE_Destroy(this);
    }

    void TGateEffectSE::p_destroy() {
        SE_Gate::TGateEffectSE_Destroy(this);
    }

} // namespace SE_Gate
