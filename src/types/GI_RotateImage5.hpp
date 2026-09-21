#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_DX.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheHSAI {
    struct TCHSAIControlEC;

} // namespace EC_CacheHSAI

namespace EC_CacheRotateBuf {
    struct TCRotateBufControlEC;

} // namespace EC_CacheRotateBuf

namespace GR_GraphBufPal {
    struct TGraphBufPalGR;

} // namespace GR_GraphBufPal

namespace GI_RotateImage5 {
    struct TRotateImage5GI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRotateImage5GI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TRotateImage5GI, GI_MessageLoop::TObjectGI, "TRotateImage5GI", 8660)
        void p_destroy() override;
        // Preserves cache keys, image storage and FrameTexture.
        void Clear() override;
        // A full turn has 256 steps.
        void SetAngle(std::uint8_t Value);
        // Does not validate against the frame count.
        void SetFrameIndex(std::uint32_t Value);
        void SetAlpha(std::uint8_t Value);
        // Replaces size and origin with a centered square enclosing all rotations.
        void SetImage(pas::WideString Path, Types::TPoint ImageSize, Types::TPoint Pivot);
        // Uses the last rendered image. Alpha must exceed 8 in software, or 0 in hardware.
        std::uint8_t HitTestPixel(Types::TPoint Point);
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void Draw(Types::TRect ClipRect) override;
        std::uint32_t GetFrameCount();
        // Queues an arbitrary HSAI path; does not change this object's image.
        static void QueueImagePath(pas::List* PendingLoads, pas::WideString Path);
        EC_CacheHSAI::TCHSAIControlEC* ImageCache;
        EC_CacheRotateBuf::TCRotateBufControlEC* RotationCache;
        GR_GraphBufPal::TGraphBufPalGR* RotatedImage;
        std::uint8_t RenderedAngle;
        std::uint8_t cpp_padding[3];
        std::uint32_t RenderedFrameIndex;
        std::uint32_t FrameIndex;
        std::uint8_t Angle;
        std::uint8_t Alpha;
        std::uint8_t ImageDirty;
        std::uint8_t cpp_padding_2[1];
        // Optional owned object; purpose unresolved.
        pas::Object* Unknown13C;
        pas::Array<GR_DX::TScreenVertexGR, 0, 3> Vertices;
        Direct3D9::IDirect3DTexture9 FrameTexture;
        // Native table spacing is 257 Singles; only the 256 byte-angle entries are initialized.
        pas::Array<float, 0, 256> TopLeftX;
        pas::Array<float, 0, 256> TopLeftY;
        pas::Array<float, 0, 256> TopRightX;
        pas::Array<float, 0, 256> TopRightY;
        pas::Array<float, 0, 256> BottomRightX;
        pas::Array<float, 0, 256> BottomRightY;
        pas::Array<float, 0, 256> BottomLeftX;
        pas::Array<float, 0, 256> BottomLeftY;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_RotateImage5
