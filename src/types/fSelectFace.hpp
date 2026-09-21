#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"

namespace fSelectFace {
    struct TfSelectFace;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfSelectFace : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfSelectFace, GI_MessageLoop::TMessageLoopGI, "TfSelectFace", 272)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void PlayerNameMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void RefreshPortrait();
        void PreviousPortraitClicked(GI_MessageLoop::TObjectGI* Sender);
        void NextPortraitClicked(GI_MessageLoop::TObjectGI* Sender);
        void SelectRace(GI_MessageLoop::TObjectGI* Sender);
        void RaceClicked(GI_MessageLoop::TObjectGI* Sender);
        void ApplyClicked(GI_MessageLoop::TObjectGI* Sender);
        void CancelClicked(GI_MessageLoop::TObjectGI* Sender);
        void RefreshPact();
        void PlayerNameChanged(GI_MessageLoop::TObjectGI* Sender);
        std::uint8_t ValidatePlayerName(pas::WideString Name);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void SelectMusic() override;
        void ProcessCallbackTimers() override;
        aGalaxyStruct::TOwnerId PlayerRace;
        std::uint8_t cpp_padding[3];
        std::int32_t CaptainPortraitIndex;
        pas::Array<std::int32_t, 0, 4> LastPortraitByRace;
        // Bound to the player-name edit control in OnOpen.
        pas::WideString PlayerName;
        std::uint8_t PlayerNameEdited;
        std::uint8_t cpp_padding_2[3];
        pas::Array<std::int32_t, 0, 4> NationalityCosts;
        std::int32_t AvailableMoney;
        std::int32_t AcceptedCost;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fSelectFace
