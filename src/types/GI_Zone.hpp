#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Zone {
    struct TZoneGI;

    enum TZoneKindGI : std::uint8_t {
        zkRect = 0,
        zkCircle = 1,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TZoneGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TZoneGI, GI_MessageLoop::TObjectGI, "TZoneGI", 328)
        void p_destroy() override;
        void SetKind(TZoneKindGI Value);
        // Native no-op: hit zones do not draw.
        void Invalidate() override;
        // Circle mode ignores Active and HitTestDisabled.
        std::uint8_t HitTest(Types::TPoint Point);
        // May invoke cursor enter/leave callbacks.
        void OnActivate() override;
        void OnDeactivate() override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        // Native calls inherited ProcessLeftButtonDown before the zone's up callback.
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadZoneProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        TZoneKindGI Kind;
        std::uint8_t CursorInside;
        std::uint8_t cpp_padding[6];
        GI_MessageLoop::TObjectNotifyEventGI EnterCallback;
        GI_MessageLoop::TObjectNotifyEventGI LeaveCallback;
        // Verified: Context/EAX, Sender/EDX, KeyState/ECX and Point on stack.
        GI_MessageLoop::TObjectMouseEventGI ZoneMouseDownCallback;
        GI_MessageLoop::TObjectMouseEventGI ZoneMouseUpCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Zone
