#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_CountBar {
    struct TCountBarGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCountBarGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TCountBarGI, GI_MessageLoop::TObjectGI, "TCountBarGI", 348)
        void p_destroy() override;
        void SetRange(std::int32_t MinValue, std::int32_t MaxValue);
        // Clamps without invoking PositionChangedCallback.
        void SetPositionInternal(std::int32_t Value);
        // Notifies only while Active and when the requested value differs from the previous position.
        void SetPosition_2(std::int32_t Value);
        void UpdateLayout();
        void AutoRepeat(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void DecreasePressed(GI_MessageLoop::TObjectGI* Sender);
        void IncreasePressed(GI_MessageLoop::TObjectGI* Sender);
        void ProcessMouseMove(std::uint32_t KeyState, Types::TPoint Point) override;
        void OnMouseEnter() override;
        void OnMouseLeave() override;
        void ProcessLeftButtonDown(std::uint32_t KeyState, Types::TPoint Point) override;
        void ProcessLeftButtonUp(std::uint32_t KeyState, Types::TPoint Point) override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadCountBarProperties(EC_BlockPar::TBlockParEC* Block);
        std::int32_t Minimum;
        std::int32_t Maximum;
        std::int32_t Position;
        std::int32_t Orientation;
        std::int32_t Step;
        GI_GraphButton::TGraphButtonGI* DecreaseButton;
        GI_GraphButton::TGraphButtonGI* IncreaseButton;
        GI_Image::TImageGI* AfterThumbImage;
        GI_Image::TImageGI* BeforeThumbImage;
        GI_GraphButton::TGraphButtonGI* ThumbButton;
        GI_Image::TImageGI* MarkerImage;
        std::uint8_t cpp_padding[4];
        GI_MessageLoop::TObjectNotifyEventGI PositionChangedCallback;
        GI_MessageLoop::PCallbackTimerGI RepeatTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_CountBar
