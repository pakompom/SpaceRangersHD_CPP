#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"

namespace GI_MessageBox {
    struct TMessageBoxGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMessageBoxGI : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TMessageBoxGI, GI_MessageLoop::TMessageLoopGI, "TMessageBoxGI", 228)
        void OnOpen() override;
        void AcceptClick(GI_MessageLoop::TObjectGI* Sender);
        void CancelClick(GI_MessageLoop::TObjectGI* Sender);
        void DialogKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey);
        void ProcessCallbackTimers() override;
        pas::WideString MessageText;
        // // mbg* option bits.
        std::uint32_t Options;
        // // Stored by ShowMessageBoxGI; no recovered reader.
        std::int32_t UnusedOption;
        std::int32_t OffsetX;
        std::int32_t OffsetY;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Native OnOpen () reads the button, icon and text-alignment bits.
    inline constexpr std::int32_t mbgOK = 0x00000001;

    inline constexpr std::int32_t mbgCancel = 0x00000002;

    // Present in callers; no recovered reader.
    inline constexpr std::int32_t mbgUnused04 = 0x00000004;

    inline constexpr std::int32_t mbgWarning = 0x00000008;

    inline constexpr std::int32_t mbgQuestion = 0x00000010;

    inline constexpr std::int32_t mbgError = 0x00000020;

    inline constexpr std::int32_t mbgLeftAlign = 0x00000040;

    // Native AcceptClick / CancelClick return these when no exit is pending.
    inline constexpr std::int32_t mbgResultOK = 1;

    inline constexpr std::int32_t mbgResultCancel = 2;

} // namespace GI_MessageBox
