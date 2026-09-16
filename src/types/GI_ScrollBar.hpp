#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_ScrollBar {
    struct TScrollBarGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScrollBarGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TScrollBarGI, GI_Panel::TPanelGI, "TScrollBarGI", 472)
        void p_destroy() override;
        // Resets the range to 0..99 and clears callbacks; does not call inherited Clear.
        void Clear() override;
        void SetUpPosition(Types::TPoint Point);
        void SetBeforeThumbBarPosition(Types::TPoint Point);
        void SetThumbTopPosition(Types::TPoint Point);
        void SetThumbCenterPosition(Types::TPoint Point);
        void SetThumbBottomPosition(Types::TPoint Point);
        void SetAfterThumbBarPosition(Types::TPoint Point);
        void SetDownPosition(Types::TPoint Point);
        void SetBeforeThumbBarSize(Types::TPoint Size);
        void SetThumbCenterSize(Types::TPoint Size);
        void SetAfterThumbBarSize(Types::TPoint Size);
        // Returns 0 outside, 1/2 arrows, 3/4 page regions, or 5 thumb; only tests the scrolling axis.
        std::int32_t GetHitRegion(Types::TPoint Point);
        void SetRange(std::int32_t MinValue, std::int32_t MaxValue);
        // Does not invoke PositionChangedCallback.
        void SetPositionInternal(std::int32_t NewPosition);
        // Notifies only while Active and only when the clamped position changes.
        void SetPosition_2(std::int32_t NewPosition);
        void SetSmallChange(std::int32_t Value);
        // A value equal to SmallChange is ignored even if LargeChange differs.
        void SetLargeChange(std::int32_t Value);
        // Caps at Maximum-Minimum+1; no lower bound check.
        void SetPageSize(std::int32_t Value);
        // Value 1 is horizontal; other values use vertical layout.
        void SetOrientation(std::int32_t Value);
        void SetKindCalcMode(std::int32_t Value);
        void SetConfigPath(const pas::WideString& Path) override;
        void SetSize(Types::TPoint Size) override;
        void SetActive(std::uint8_t Enabled) override;
        void UpdateLayout();
        // Uses the up-arrow image for scrollbar thickness.
        void UpdateSizeForOrientation();
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonDoubleClick(std::uint32_t KeyState, Types::TPoint Point) override;
        void StartAutoRepeat(std::int32_t DelayMs, std::int32_t RepeatMs);
        void StopAutoRepeat();
        void AutoRepeat(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadScrollBarProperties(EC_BlockPar::TBlockParEC* Block);
        std::uint8_t cpp_padding[4];
        std::int32_t Minimum;
        std::int32_t Maximum;
        std::int32_t Position;
        std::int32_t LargeChange;
        std::int32_t SmallChange;
        std::int32_t PageSize;
        std::int32_t Orientation;
        std::int32_t CalculationMode;
        // Each image triple is normal, active, down. Up/Down also mean left/right horizontally.
        pas::Array<GI_Image::TImageGI*, 0, 2> UpImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> BeforeThumbBarImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> ThumbTopImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> ThumbCenterImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> ThumbBottomImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> AfterThumbBarImages;
        pas::Array<GI_Image::TImageGI*, 0, 2> DownImages;
        GI_MessageLoop::PCallbackTimerGI RepeatTimer;
        std::int32_t PressedRegion;
        std::int32_t HoveredRegion;
        std::int32_t DragStartPosition;
        GI_Label::TLabelGI* MinimumLabel;
        GI_Label::TLabelGI* MaximumLabel;
        GI_Label::TLabelGI* PositionLabel;
        GI_MessageLoop::TObjectNotifyEventGI PositionChangedCallback;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_ScrollBar
