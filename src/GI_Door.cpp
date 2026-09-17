#include "layout/GI_Door.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/GI_Door.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"

// Native class ownership follows reference/unit_ownership.json.
namespace GI_Door {
    void TDoorGI_Create(TDoorGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Image = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Self);
    }

    void TDoorGI_Destroy(TDoorGI* Self) {
        Self->StopStepTimer();
        pas::free(Self->Image);
        Self->Image = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TDoorGI::Clear() {
    }

    void TDoorGI::SetStepTime(std::int32_t Value) {
        if (StepTime != Value) {
            StepTime = Value;
            if (FrameStep != 0) {
                StartStepTimer();
            }
        }
    }

    void TDoorGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        Image->SetSize(Size);
    }

    void TDoorGI::StartStepTimer() {
        if (FrameStep != 0) {
            StopStepTimer();
            StepTimer = MessageLoop->ScheduleCallbackTimer(StepTime, StepTime, pas::bind_method<&TDoorGI::StepFrame>(this), 0);
        }
    }

    void TDoorGI::StopStepTimer() {
        if (StepTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(StepTimer);
            StepTimer = nullptr;
        }
    }

    void TDoorGI::StepFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Frame = Image->SequenceFrame + FrameStep;
        if (Frame <= 0) {
            Image->SetSequenceFrame(0);
            StopStepTimer();
            FrameStep = 0;
        } else if (Frame >= Image->SequenceFrameCount - 1) {
            Image->SetSequenceFrame(Image->SequenceFrameCount - 1);
            StopStepTimer();
            FrameStep = 0;
        } else {
            Image->SetSequenceFrame(Frame);
        }
    }

    void TDoorGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        Image->SetSequenceFrame(0);
        StopStepTimer();
    }

    void TDoorGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        Image->SetSequenceFrame(0);
        StopStepTimer();
    }

    void TDoorGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        if (!IsOccludedAtPoint(MessageLoop->GetCursorPoint())) {
            FrameStep = 1;
            StartStepTimer();
        }
    }

    void TDoorGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        if (!IsOccludedAtPoint(MessageLoop->GetCursorPoint())) {
            FrameStep = -1;
            StartStepTimer();
        }
    }

    void TDoorGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessMouseMove(KeyState, Point);
        if (IsOccludedAtPoint(Point)) {
            FrameStep = -1;
            if (StepTimer == nullptr) {
                StartStepTimer();
            }
        } else {
            FrameStep = 1;
            if (StepTimer == nullptr) {
                StartStepTimer();
            }
        }
    }

    void TDoorGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        if (!IsOccludedAtPoint(MessageLoop->GetCursorPoint())) {
            if (pas::assigned(ClickCallback)) {
                ClickCallback(this);
            }
        }
    }

    void TDoorGI_LoadFromConfigPath(TDoorGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadDoorProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TDoorGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadDoorProperties(Block);
    }

    void TDoorGI::LoadDoorProperties(EC_BlockPar::TBlockParEC* Block) {
        FrameStep = 0;
        if (Block->CountParams(u"StepTime"_wref.get()) > 0) {
            SetStepTime(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"StepTime"_wref.get()))));
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            Image->SetImagePath(Block->GetParam(u"Image"_wref.get()));
            Image->SequenceIndex = 0;
            Image->UpdateAutoGeometry();
        }
    }

    void TDoorGI::p_destroy() {
        GI_Door::TDoorGI_Destroy(this);
    }

    void TDoorGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Door::TDoorGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Door
