#include "layout/GI_SBPath.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SBPath.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native class ownership follows reference/unit_ownership.json.
namespace GI_SBPath {
    void TSBPathGI_Create(TSBPathGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ThumbImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->Minimum = 0;
        Self->Maximum = 100;
        Self->Position = 0;
        Self->HitRadius = 40;
        Self->UpdateThumbPosition();
    }

    void TSBPathGI_Destroy(TSBPathGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TSBPathGI::Clear() {
        PointCount = 0;
        Points = nullptr;
        ThumbImage->Clear();
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TSBPathGI::SetImagePath(pas::WideString Path) {
        ThumbImage->SetImagePath(Path);
        {
            Types::TPoint contentSize = ThumbImage->GetContentSize();
            GI_Image::TImageGI* thumbImage = ThumbImage;
            thumbImage->SetSize(contentSize);
        }
        ThumbImage->SetOrigin(ClassesImports::Point(ThumbImage->ClientSize.X / 2, ThumbImage->ClientSize.Y / 2));
    }

    void TSBPathGI::SetPositionValue(std::int32_t Value) {
        if (Position == Value) {
            return;
        }
        if (Value < Minimum) {
            Value = Minimum;
        }
        if (Value > Maximum) {
            Value = Maximum;
        }
        if (Position == Value) {
            return;
        }
        Position = Value;
        UpdateThumbPosition();
        if (pas::assigned(ChangeCallback)) {
            ChangeCallback(this);
        }
    }

    void TSBPathGI::UpdateThumbPosition() {
        if (PointCount < 1) {
            return;
        }
        if (Maximum - Minimum < 1) {
            ThumbImage->SetPosition(Points[0]);
        } else {
            ThumbImage->SetPosition(Points[System::Round(pas::real_divide(Position - Minimum, Maximum - Minimum) * (PointCount - 1))]);
        }
    }

    std::int32_t TSBPathGI::PositionFromPointIndex(std::int32_t Index) {
        if (PointCount < 2) {
            return Minimum;
        }
        return System::Round(pas::real_divide(Index, PointCount - 1) * (Maximum - Minimum) + Minimum);
    }

    std::int32_t TSBPathGI::FindClosestPoint(Types::TPoint Point, std::int32_t& DistanceSquared) {
        std::int32_t Distance{};
        std::int32_t I{};
        std::int32_t BestDistance = 99999999;
        std::int32_t BestIndex = -1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PointCount - 1); cpp_range.next(I); ) {
            Distance = pas::sqr(Point.X - Points[I].X) + pas::sqr(Point.Y - Points[I].Y);
            if (Distance < BestDistance) {
                BestDistance = Distance;
                BestIndex = I;
            }
        }
        DistanceSquared = BestDistance;
        return BestIndex;
    }

    void TSBPathGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        Dragging = false;
    }

    void TSBPathGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        Dragging = false;
    }

    void TSBPathGI::OnMouseEnter() {
        GI_MessageLoop::TObjectGI::OnMouseEnter();
    }

    void TSBPathGI::OnMouseLeave() {
        GI_MessageLoop::TObjectGI::OnMouseLeave();
    }

    void TSBPathGI::ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) {
        std::int32_t Distance{};
        GI_MessageLoop::TObjectGI::ProcessLeftButtonDown(KeyState, Point);
        if (PointCount < 1) {
            return;
        }
        std::int32_t Index = FindClosestPoint(ToLocalPoint(Point), Distance);
        if (pas::sqr(HitRadius) > Distance) {
            Position = PositionFromPointIndex(Index);
            UpdateThumbPosition();
            Dragging = true;
            if (pas::assigned(ChangeCallback)) {
                ChangeCallback(this);
            }
        } else {
            Dragging = false;
        }
    }

    void TSBPathGI::ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) {
        GI_MessageLoop::TObjectGI::ProcessLeftButtonUp(KeyState, Point);
        Dragging = false;
    }

    void TSBPathGI::ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) {
        std::int32_t Distance{};
        GI_MessageLoop::TObjectGI::ProcessMouseMove(KeyState, LocalPosition);
        if (!Dragging) {
            return;
        }
        std::int32_t Index = FindClosestPoint(ToLocalPoint(Point), Distance);
        if (pas::sqr(HitRadius) > Distance) {
            Position = PositionFromPointIndex(Index);
            UpdateThumbPosition();
            Dragging = true;
            if (pas::assigned(ChangeCallback)) {
                ChangeCallback(this);
            }
        } else {
            Dragging = false;
        }
    }

    void TSBPathGI_LoadFromConfigPath(TSBPathGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadPathProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TSBPathGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadPathProperties(Block);
    }

    void TSBPathGI::LoadPathProperties(EC_BlockPar::TBlockParEC* Block) {
        EC_BlockPar::TBlockParEC* Path{};
        std::int32_t I{};
        if (Block->CountBlocks(u"Path"_wref.get()) > 0) {
            Points = nullptr;
            Path = Block->GetBlock(u"Path"_wref.get());
            PointCount = Path->GetParamCount();
            Points.set_length(PointCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, PointCount - 1); cpp_range.next(I); ) {
                Points[I] = GI_Main::GetPointGI(Path->GetParamValue(I));
            }
        }
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            SetImagePath(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"Min"_wref.get()) > 0) {
            Minimum = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Min"_wref.get())));
        }
        if (Block->CountParams(u"Max"_wref.get()) > 0) {
            Maximum = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Max"_wref.get())));
        }
        if (Minimum > Maximum) {
            Minimum = Maximum;
        }
        if (Block->CountParams(u"Position"_wref.get()) > 0) {
            SetPositionValue(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Position"_wref.get()))));
        }
        if (Block->CountParams(u"RadiusHit"_wref.get()) > 0) {
            HitRadius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"RadiusHit"_wref.get())));
        }
        UpdateThumbPosition();
    }

    void TSBPathGI::p_destroy() {
        GI_SBPath::TSBPathGI_Destroy(this);
    }

    void TSBPathGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_SBPath::TSBPathGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_SBPath
