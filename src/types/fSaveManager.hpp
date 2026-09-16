#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace fSaveManager {
    struct TSMSlot;

    struct TfSaveManager;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfSaveManager : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfSaveManager, GI_MessageLoop::TMessageLoopGI, "TfSaveManager", 228)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RebuildSlotControls();
        void InitializeSlotPanel(GI_Panel::TPanelGI* Panel);
        void RefreshSlot(std::int32_t SlotIndex, std::uint8_t UnusedEditingFlag);
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void LoadClicked(GI_MessageLoop::TObjectGI* Sender);
        void SaveClicked(GI_MessageLoop::TObjectGI* Sender);
        void DeleteClicked(GI_MessageLoop::TObjectGI* Sender);
        void SlotMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SlotDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SlotKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        static std::uint8_t AcceptSaveNameCharacter(GI_MessageLoop::TObjectGI* Sender, char16_t Character);
        void SelectSlot(std::int32_t SlotIndex);
        void ClearSlotSelection();
        std::uint8_t AutoSaveExists();
        static pas::WideString GetAutoSavePath();
        std::int32_t FindAutoSaveSlot();
        static pas::WideString BuildCurrentSaveDescription();
        pas::WideString BuildUniqueSavePath(const pas::WideString& FileName, std::int32_t& SuffixIndex);
        static pas::WideString GetSaveConfigPath(const pas::WideString& FileName);
        std::uint8_t QuickSaveExists(std::int32_t SlotIndex);
        static pas::WideString GetQuickSavePath(std::int32_t SlotIndex);
        static pas::WideString GetTurnSavePath();
        void SlotMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void SlotMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ScanSaveFiles();
        std::uint8_t IsSlotEmpty(std::int32_t SlotIndex);
        std::int32_t FindNewestSlot();
        static std::int32_t ReadSaveVersion(pas::WideString FileName);
        void LoadSavePreviews(pas::WideString FileName);
        void FinishPreviewDelay(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SelectMusic() override;
        std::int32_t SelectedSlot;
        pas::List* Slots;
        GI_MessageLoop::PCallbackTimerGI PreviewTimer;
        GR_Sound::TSoundBufferControl* PreviewSound;
        std::uint8_t Closing;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TSaveManagerMode : std::uint8_t {
        smmLoad = 0,
        smmSave = 1,
    };

    #pragma pack(push, 1)
    struct TSMSlot {
        pas::WideString FileName;
        pas::WideString DisplayName;
        std::int32_t Turn;
        std::int32_t Money;
        pas::WideString PilotName;
        pas::WideString RaceName;
        Windows::TFileTime LocalWriteTime;
    };
    #pragma pack(pop)

    using PSMSlot = TSMSlot*;

} // namespace fSaveManager
