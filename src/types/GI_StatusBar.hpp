#pragma once
#include "runtime_support.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_StatusBar {
    struct TStatusBarGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStatusBarGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TStatusBarGI, GI_Panel::TPanelGI, "TStatusBarGI", 356)
        void p_destroy() override;
        // Preserves Value; resets the range to 0..100.
        void Clear() override;
        // If MinValue exceeds MaxValue, lowers MinValue to MaxValue. Does not clamp the stored Value.
        void SetRange(double MinValue, double MaxValue);
        void SetValue(double NewValue);
        void SetSize(Types::TPoint Size) override;
        void UpdateImageLayout();
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadStatusProperties(EC_BlockPar::TBlockParEC* Block);
        std::uint8_t cpp_padding[4];
        double Minimum;
        double Maximum;
        double Value;
        GI_Image::TImageGI* LeftImage;
        GI_Image::TImageGI* CenterImage;
        GI_Image::TImageGI* RightImage;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_StatusBar
