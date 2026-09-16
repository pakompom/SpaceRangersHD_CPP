#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_AImage {
    struct TAImageGI;

} // namespace GI_AImage

namespace GI_AlphaImage {
    struct TAlphaImageGI;

} // namespace GI_AlphaImage

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_GI {
    struct TgiGI;

} // namespace GI_GI

namespace GI_GraphBuf {
    struct TGraphBufGI;

} // namespace GI_GraphBuf

namespace GI_SimpleImage {
    struct TSimpleImageGI;

} // namespace GI_SimpleImage

namespace GI_TransImage {
    struct TTransImageGI;

} // namespace GI_TransImage

namespace GI_Image {
    struct TImageGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TImageGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TImageGI, GI_MessageLoop::TObjectGI, "TImageGI", 324)
        void p_destroy() override;
        void Clear() override;
        void SetImagePath(pas::WideString Path);
        pas::WideString GetImagePath();
        Types::TPoint GetContentSize();
        Types::TPoint GetContentOrigin();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetHalfAlpha(std::uint8_t Value);
        std::uint8_t GetAlpha();
        void SetAlpha(std::uint8_t Value);
        void SetSize(Types::TPoint Size) override;
        void SetOrigin(Types::TPoint Origin) override;
        std::uint8_t HitTestPixel(Types::TPoint Point);
        Types::TPoint GetVisualCenter();
        void RestartPlayback();
        void StopPlayback();
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        void QueueImageLoad(pas::List* PendingLoads) override;
        void SetHardwareMirrorHorizontal(std::uint8_t Value);
        GI_SimpleImage::TSimpleImageGI* SimpleImageControl;
        GI_TransImage::TTransImageGI* TransImageControl;
        GI_AlphaImage::TAlphaImageGI* AlphaImageControl;
        GI_GI::TgiGI* GiImageControl;
        GI_AImage::TAImageGI* AnimImageControl;
        GI_GAI::TgaiGI* GaiImageControl;
        GI_GraphBuf::TGraphBufGI* GraphBufControl;
        pas::WideString ImagePath;
        std::uint32_t AutoUpdateFlags;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Image
