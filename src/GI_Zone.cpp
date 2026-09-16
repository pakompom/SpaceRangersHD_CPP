#include "layout/GI_Zone.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Zone.hpp"
#include "units/GR_Main.hpp"
#include "units/aMyFunction.hpp"

namespace GI_Zone {
    void TZoneGI_Create(TZoneGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

    void TZoneGI_Destroy(TZoneGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TZoneGI::SetKind(TZoneKindGI Value) {
        if (Kind != Value) {
            Kind = Value;
        }
    }

    // Native no-op: hit zones do not draw.
    void TZoneGI::Invalidate() {
    }

    // Circle mode ignores Active and HitTestDisabled.
    std::uint8_t TZoneGI::HitTest(Types::TPoint Point) {
        std::int32_t Diameter{};
        std::uint8_t Result = false;
        if (Kind == zkRect) {
            return ContainsPoint(Point);
        } else if (Kind == zkCircle) {
            if (HitTestBounds.Right - HitTestBounds.Left < HitTestBounds.Bottom - HitTestBounds.Top) {
                Diameter = HitTestBounds.Right - HitTestBounds.Left;
            } else {
                Diameter = HitTestBounds.Bottom - HitTestBounds.Top;
            }
            return pas::sqr(pas::real_divide(Diameter, 2.0L)) >= aMyFunction::PointDistanceSquared(EC_Struct::MakePointF((HitTestBounds.Left + HitTestBounds.Right) / 2, (HitTestBounds.Top + HitTestBounds.Bottom) / 2), EC_Struct::PointToPointF(Point));
        } else {
            return Result;
        }
    }

    // May invoke cursor enter/leave callbacks.
    void TZoneGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        if (HitTest(MessageLoop->GetCursorPoint())) {
            if (!CursorInside) {
                CursorInside = true;
                if (pas::assigned(EnterCallback)) {
                    EnterCallback(this);
                }
            }
        } else if (CursorInside == true) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        if (CursorInside) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
        if (HitTest(MessageLoop->GetCursorPoint())) {
            if (!CursorInside) {
                CursorInside = true;
                if (pas::assigned(EnterCallback)) {
                    EnterCallback(this);
                }
            }
        } else if (CursorInside == true) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
        if (CursorInside) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessMouseMove(KeyState, Point);
        if (HitTest(Point)) {
            if (!CursorInside) {
                CursorInside = true;
                if (pas::assigned(EnterCallback)) {
                    EnterCallback(this);
                }
            }
        } else if (CursorInside == true) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (HitTest(Point)) {
            if (!CursorInside) {
                CursorInside = true;
                if (pas::assigned(EnterCallback)) {
                    EnterCallback(this);
                }
            }
            if (pas::assigned(ZoneMouseDownCallback)) {
                ZoneMouseDownCallback(this, KeyState, Point);
            }
        } else if (CursorInside == true) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    // Native calls inherited ProcessLeftButtonDown before the zone's up callback.
    void TZoneGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (HitTest(Point)) {
            if (!CursorInside) {
                CursorInside = true;
                if (pas::assigned(EnterCallback)) {
                    EnterCallback(this);
                }
            }
            if (pas::assigned(ZoneMouseUpCallback)) {
                ZoneMouseUpCallback(this, KeyState, Point);
            }
        } else if (CursorInside == true) {
            CursorInside = false;
            if (pas::assigned(LeaveCallback)) {
                LeaveCallback(this);
            }
        }
    }

    void TZoneGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        LoadZoneProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TZoneGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadZoneProperties(Block);
    }

    void TZoneGI::LoadZoneProperties(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Value{};
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            Value = Block->GetParam(u"Kind"_wref.get());
            if (Value == u"Rect") {
                SetKind(zkRect);
            } else if (Value == u"Circle") {
                SetKind(zkCircle);
            }
        }
    }

    void TZoneGI::UpdateAutoGeometry() {
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
    }

    void TZoneGI::p_destroy() {
        GI_Zone::TZoneGI_Destroy(this);
    }

} // namespace GI_Zone
