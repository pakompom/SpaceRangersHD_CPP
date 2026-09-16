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

    // Native record RTTI.
    #pragma pack(push, 1)
    struct TfLoadQuestSlot {
        pas::WideString Name;
        pas::WideString Title;
        pas::WideString Description;
        pas::WideString Image;
        pas::WideString Genre;
        std::int32_t Length;
        // // -1 for nonnumeric names.
        std::int32_t QuestId;
        // // Borrowed from the row control.
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
        // Returns a zero-based index; inserts after entries with equal access.
        std::int32_t InsertEntryByAccess(std::int32_t RequiredAccess);
        void RebuildQuestList();
        void InitializeQuestRow(GI_MessageLoop::TObjectGI* Row);
        // Index is zero-based; locked entries clear the selection.
        void SelectQuest(std::int32_t Index);
        void UpdateQuestRow(std::int32_t Index);
        void QuestRowMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void QuestRowMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void QuestRowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void QuestRowDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SelectCategory(GI_MessageLoop::TObjectGI* Sender);
        void StartSelectedQuest(GI_MessageLoop::TObjectGI* Sender);
        // Loads the quest with HeaderOnly enabled.
        void ShowSelectedQuestDetails();
        // QuestComplate.dat: CRC32, Int32 count, then Int32 entries.
        void LoadCompletionData();
        void SaveCompletionData();
        // QuestId must be 0..9999. Higher status wins; equal status minimizes an existing nonzero Value.
        void RecordCompletion(std::int32_t QuestId, std::int32_t Value, std::int32_t Status);
        // Advances past a group when all but one quest is completed.
        std::int32_t CalculateAccessLevel();
        // X is completed, Y is total; includes only numeric quests with positive Access.
        WindowsSdk::TPoint GetCompletionCounts();
        // The displayed total includes groups below 3.
        pas::WideString GetCompletionSummary();
        // Empty implementation.
        void SelectMusic() override;
        // // Owned.
        fPanelLoad::TfPanelLoad* LoadPanel;
        // // Owned dynamic array, indexed from zero.
        pas::DynArray<TfLoadQuestSlot> Entries;
        std::int32_t SelectedIndex;
        std::int32_t HoveredIndex;
        // // Dynamic array: auxiliary value, status pairs indexed by quest ID.
        pas::DynArray<std::int32_t> CompletionData;
        std::int32_t Category;
        std::int32_t AccessLevel;
        pas::WideString KeyHistory;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoadQuest
