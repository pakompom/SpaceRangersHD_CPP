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

    struct TfLoadRobotSlot {
        pas::WideString Name;
        pas::WideString FileName;
        std::int32_t MapIndex;
        GI_Image::TImageGI* Image;
        std::int32_t Access;
        std::int32_t Side;
        std::uint8_t AlternateBackground;
        std::uint8_t cpp_padding[3];
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
        WindowsSdk::TPoint GetCompletionCounts();
        pas::WideString GetCompletionSummary();
        void SelectMusic() override;
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::DynArray<TfLoadRobotSlot> Entries;
        std::int32_t SelectedIndex;
        std::int32_t HoveredIndex;
        std::int32_t BattleResult;
        pas::DynArray<std::int32_t> CompletionData;
        std::int32_t Category;
        std::int32_t UnlockedAccess;
        pas::WideString KeyHistory;
        std::int32_t Difficulty;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoadRobot
