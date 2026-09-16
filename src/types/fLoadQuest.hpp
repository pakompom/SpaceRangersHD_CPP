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

namespace fLoadQuest {
    struct TfLoadQuestSlot;

    struct TfLoadQuest;

    #pragma pack(push, 1)
    struct TfLoadQuestSlot {
        pas::WideString Name;
        pas::WideString Title;
        pas::WideString Description;
        pas::WideString Image;
        pas::WideString Genre;
        std::int32_t Length;
        std::int32_t QuestId;
        GI_Image::TImageGI* BackgroundImage;
        std::int32_t RequiredAccess;
        std::uint8_t AlternateGroup;
        std::uint8_t cpp_padding[3];
        std::int32_t Difficulty;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfLoadQuest : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfLoadQuest, GI_MessageLoop::TMessageLoopGI, "TfLoadQuest", 240)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void ReturnToMenu(GI_MessageLoop::TObjectGI* Sender);
        void QuestListKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        std::int32_t InsertEntryByAccess(std::int32_t RequiredAccess);
        void RebuildQuestList();
        void InitializeQuestRow(GI_MessageLoop::TObjectGI* Row);
        void SelectQuest(std::int32_t Index);
        void UpdateQuestRow(std::int32_t Index);
        void QuestRowMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void QuestRowMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void QuestRowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void QuestRowDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SelectCategory(GI_MessageLoop::TObjectGI* Sender);
        void StartSelectedQuest(GI_MessageLoop::TObjectGI* Sender);
        void ShowSelectedQuestDetails();
        void LoadCompletionData();
        void SaveCompletionData();
        void RecordCompletion(std::int32_t QuestId, std::int32_t Value, std::int32_t Status);
        std::int32_t CalculateAccessLevel();
        WindowsSdk::TPoint GetCompletionCounts();
        pas::WideString GetCompletionSummary();
        void SelectMusic() override;
        fPanelLoad::TfPanelLoad* LoadPanel;
        pas::DynArray<TfLoadQuestSlot> Entries;
        std::int32_t SelectedIndex;
        std::int32_t HoveredIndex;
        pas::DynArray<std::int32_t> CompletionData;
        std::int32_t Category;
        std::int32_t AccessLevel;
        pas::WideString KeyHistory;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoadQuest
