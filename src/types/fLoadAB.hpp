#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace fLoadAB {
    struct TfLoadABSlot;

    struct TfLoadAB;

    // Native RTTI name and managed-field offsets.
    struct TfLoadABSlot {
        pas::WideString Name;
        pas::WideString MapName;
        pas::WideString ImageName;
        // Produced by the native multiline helper.
        pas::WideString Description;
        std::int32_t ConfigIndex;
        // Borrowed from the row control.
        GI_Image::TImageGI* BackgroundImage;
        std::int32_t Difficulty;
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfLoadAB : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfLoadAB, GI_MessageLoop::TMessageLoopGI, "TfLoadAB", 228)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void ReturnToMenu(GI_MessageLoop::TObjectGI* Sender);
        void ArenaListKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        std::int32_t AppendEntry(std::int32_t IgnoredAccess);
        void RebuildArenaList();
        void InitializeArenaRow(GI_MessageLoop::TObjectGI* Row);
        void SelectArena(std::int32_t Index);
        void UpdateArenaRow(std::int32_t Index);
        void ArenaRowMouseEnter(GI_MessageLoop::TObjectGI* Sender);
        void ArenaRowMouseLeave(GI_MessageLoop::TObjectGI* Sender);
        void ArenaRowMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void ArenaRowDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void SelectCategory(GI_MessageLoop::TObjectGI* Sender);
        void StartSelectedArena(GI_MessageLoop::TObjectGI* Sender);
        void ShowSelectedArenaDetails();
        // Native empty hook, retained during catalog rebuild.
        static void PrepareCatalog();
        // Returns the configured ABMap entry count for the main menu.
        static pas::WideString GetCatalogSummary();
        void SelectMusic() override;
        pas::DynArray<TfLoadABSlot> Entries;
        std::int32_t SelectedIndex;
        std::int32_t HoveredIndex;
        std::int32_t Category;
        pas::WideString KeyHistory;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoadAB
