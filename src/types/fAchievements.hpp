#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SimpleSteamApi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace fAchievements {
    struct TfAchievementsSlot;

    struct TfAchievements;

    struct TfAchievementsSlot {
        pas::WideString Key;
        SimpleSteamApi::PAchievementData Data;
        GI_Image::TImageGI* Background;
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfAchievements : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfAchievements, GI_MessageLoop::TMessageLoopGI, "TfAchievements", 212)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void CloseClicked(GI_MessageLoop::TObjectGI* Sender);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) override;
        std::int32_t AppendRow(std::int32_t UnusedIndex);
        void RebuildAchievementList();
        void BuildRow(GI_MessageLoop::TObjectGI* Owner);
        void RefreshRowBackground(std::int32_t Index);
        void SelectMusic() override;
        static void BuildProgressBars(GI_MessageLoop::TObjectGI* Owner, std::int32_t MinValue, std::int32_t MaxValue, std::int32_t StoredValue, std::int32_t CurrentValue);
        pas::DynArray<TfAchievementsSlot> Rows;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fAchievements
