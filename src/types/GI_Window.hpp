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

namespace GI_Window {
    struct TWindowGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TWindowGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TWindowGI, GI_Panel::TPanelGI, "TWindowGI", 380)
        void p_destroy() override;
        Types::TPoint AlignSizeToBorderTiles(Types::TPoint Size);
        void UpdateBorderLayout();
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadWindowProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        std::uint8_t cpp_padding[4];
        GI_Image::TImageGI* LeftImage;
        GI_Image::TImageGI* RightImage;
        GI_Image::TImageGI* TopImage;
        GI_Image::TImageGI* BottomImage;
        GI_Image::TImageGI* TopLeftImage;
        GI_Image::TImageGI* TopRightImage;
        GI_Image::TImageGI* BottomLeftImage;
        GI_Image::TImageGI* BottomRightImage;
        GI_Image::TImageGI* TextureImage;
        Types::TRect WorkSubRect;
        Types::TPoint MinimumSize;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Window
