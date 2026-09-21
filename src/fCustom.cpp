#include "layout/fCustom.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Expression.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/ThreadCalc.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/aCalc.hpp"
#include "units/aGalaxy.hpp"
#include "units/aScript.hpp"
#include "units/fCustom.hpp"

namespace fCustom {
    // Borrowed while ShowCustomDialog is active; nested custom dialogs restore the previous value.
    GI_MessageLoop::TMessageLoopGI* CurrentCustomDialog = nullptr;

    // Native TfCustomLoop VMT and the surrounding modal launcher establish ownership.
    // Loads the named layout and executes its opening/closing script text. Restores the previous active custom dialog on return.
    std::int32_t ShowCustomDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ScreenName) {
        std::int32_t Result{};
        GI_MessageLoop::TObjectGI* Background{};
        pas::WideString BeforeCode{};
        pas::WideString AfterCode{};
        GI_MessageLoop::TCursorStateGI State{};
        Parent->RootUiObject->OnModalSuspend();
        Parent->CaptureCursorState(&State);
        Parent->SetCursorActive(false);
        Parent->DrawQueuedUpdateRects();
        GR_Main::CaptureScreenBackground(false, 0);
        TfCustomLoop* Dialog = pas::construct_call<TfCustomLoop>(GI_MessageLoop::TMessageLoopGI_Create);
        Dialog->ParentLoop = Parent;
        Parent->ChildLoop = Dialog;
        Dialog->InitializeFromConfig(GR_Main::UiStyleConfig, ScreenName, true);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlock(pas::view(ScreenName))->FindBlock(u"CodeBeforeRun"sv);
        if (Block != nullptr) {
            BeforeCode = Block->ConcatenateValues();
        } else {
            BeforeCode = pas::WideString();
        }
        Block = GR_Main::UiStyleConfig->GetBlock(pas::view(ScreenName))->FindBlock(u"CodeAfterRun"sv);
        if (Block != nullptr) {
            AfterCode = Block->ConcatenateValues();
        } else {
            AfterCode = pas::WideString();
        }
        Dialog->InitializeLayout();
        GI_MessageLoop::TMessageLoopGI* Previous = CurrentCustomDialog;
        {
            try {
                CurrentCustomDialog = Dialog;
                aScript::ExecuteScriptText(BeforeCode, nullptr);
                Background = Dialog->FindControlByPath(u"BGBuf"_wref.get());
                if (Background != nullptr) {
                    GR_Main::CaptureScreenBackground(true, 0);
                    pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(Background)->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
                }
                Result = Dialog->Run();
                aScript::ExecuteScriptText(AfterCode, nullptr);
                Parent->InvalidateViewport();
            } catch (...) {
                Parent->ChildLoop = nullptr;
                CurrentCustomDialog = Previous;
                pas::free(Dialog);
                throw;
            }
            Parent->ChildLoop = nullptr;
            CurrentCustomDialog = Previous;
            pas::free(Dialog);
        }
        Parent->RestoreCursorState(&State);
        Parent->UpdateCursorPosition();
        Parent->RootUiObject->OnModalResume();
        return Result;
    }

    void TfCustomLoop::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
    }

    void TfCustomLoop::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop->ExitCode != 0) {
            if (ExitCode == 0) {
                RequestClose(255);
            }
        }
    }

    void TfCustomLoop::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            if (aGalaxy::Galaxy != nullptr) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(30011);
            }
            aScript::ExecuteGameplayUiCode(Block, Key);
            if (aGalaxy::Galaxy != nullptr) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(40011);
            }
        }
    }

} // namespace fCustom
