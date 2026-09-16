#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fLoadRobot {
    struct TfLoadRobotSlot;

    struct TfLoadRobot;

    // Native RTTI name and managed-field offsets.
    struct TfLoadRobotSlot {
        pas::WideString Name;
        // Passed to FRun when starting the battle.
        pas::WideString FileName;
        // -1 for loose map files absent from configured maps.
        std::int32_t MapIndex;
        // Borrowed child of the entry panel.
        GI_Image::TImageGI* Image;
        // Unlock group, zero for loose maps.
        std::int32_t Access;
        // Red=1, Green=2, Blue=4.
        std::int32_t Side;
        // Alternates at each unlock group.
        std::uint8_t AlternateBackground;
        std::uint8_t cpp_padding[3];
        // -1 for an unknown duration.
        std::int32_t Length;
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfLoadRobot : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfLoadRobot, GI_MessageLoop::TMessageLoopGI, "TfLoadRobot", 248)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CloseClick(GI_MessageLoop::TObjectGI* Sender);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        std::int32_t InsertEntry(std::int32_t Access);
        void RebuildEntries();
        void BuildEntryPanel(GI_MessageLoop::TObjectGI* Panel);
        void SelectEntry(std::int32_t Index);
        void UpdateEntryImage(std::int32_t Index);
        void EntryMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void EntryMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void EntryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void EntryDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void CategoryClick(GI_MessageLoop::TObjectGI* Sender);
        void DifficultyClick(GI_MessageLoop::TObjectGI* Sender);
        void StartClick(GI_MessageLoop::TObjectGI* Sender);
        void UpdateSelectionDetails();
        void LoadCompletionData();
        void SaveCompletionData();
        void RecordCompletion(std::int32_t MapId, std::int32_t Score, std::int32_t Level);
        std::int32_t GetUnlockedAccess();
        // X completed, Y eligible; groups greater than -1 are eligible.
        WindowsSdk::TPoint GetCompletionCounts();
        // Menu summary; displayed total includes groups 0..2.
        pas::WideString GetCompletionSummary();
        void SelectMusic() override;
        // Owned.
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::DynArray<TfLoadRobotSlot> Entries;
        std::int32_t SelectedIndex;
        std::int32_t HoveredIndex;
        // Nonzero preserves Entries during the planetary-battle transition.
        std::int32_t BattleResult;
        pas::DynArray<std::int32_t> CompletionData;
        std::int32_t Category;
        std::int32_t UnlockedAccess;
        pas::WideString KeyHistory;
        // Dif1/Dif2/Dif3 controls.
        std::int32_t Difficulty;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoadRobot
