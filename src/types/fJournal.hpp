#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PanelScrollBar {
    struct TPanelScrollBarGI;

} // namespace GI_PanelScrollBar

namespace fJournal {
    struct TfJournal;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfJournal : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfJournal, fPanelMain::TMessageLoopGIWithMainPanel, "TfJournal", 224)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RefreshTelevisionAnimation(GI_MessageLoop::TObjectGI* Sender);
        void ClearEntries();
        void FinishEntries();
        void AddEntrySpacing(std::int32_t Height);
        void AddEntryHeading(pas::WideString Text, pas::WideString MessageText, std::int32_t Compact, std::int32_t RecordIndex);
        void AddEntryText(pas::WideString Text, GI_Main::TTextAlignXGI Align, pas::WideString FontName);
        void ToggleJournalNews(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        static void MainPanelKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        static std::uint8_t HasTelevisionReception();
        void PinEntryClicked(GI_MessageLoop::TObjectGI* Sender);
        void DeleteEntryClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearEntriesConfirmed();
        void ExportEntriesConfirmed();
        void ProcessCallbackTimers() override;
        void SelectMusic() override;
        void RebuildNewsEntries();
        void RebuildJournalEntries();
        void AddRecordClicked(GI_MessageLoop::TObjectGI* Sender);
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void ClearInputClicked(GI_MessageLoop::TObjectGI* Sender);
        void CopyInputClicked(GI_MessageLoop::TObjectGI* Sender);
        void PasteInputClicked(GI_MessageLoop::TObjectGI* Sender);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        GI_PanelScrollBar::TPanelScrollBarGI* InfoPanel;
        std::int32_t ContentHeight;
        std::uint8_t JournalSelected;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fJournal
