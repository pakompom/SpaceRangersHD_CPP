#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGAI {
    struct TCGaiControlEC;

} // namespace EC_CacheGAI

namespace GI_StarField {
    struct TStarFieldPixel;

    struct TStarFieldPoint;

    struct TStarFieldList;

    struct TStarFieldGI;

    using PStarFieldPixel = TStarFieldPixel*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarFieldGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TStarFieldGI, GI_Panel::TPanelGI, "TStarFieldGI", 400)
        void p_destroy() override;
        void SetBackgroundImage(const pas::WideString& Path);
        void ClearProjectedPixels();
        void GrowPixelBuffers();
        void RebuildProjectedPixels();
        void SetViewPosition(EC_Struct::TPointF Position);
        void SetSize(WindowsSdk::TPoint Size) override;
        void MarkViewDirty();
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadStarFieldProperties(EC_BlockPar::TBlockParEC* Block);
        // Empty in native code.
        void Invalidate() override;
        // Updates GlobalsV.SkipSavedPixelRestore from the background rectangle change.
        void UpdateBackgroundBounds();
        void ErasePreviousFrame() override;
        void DrawBackground(WindowsSdk::TRect ClipRect);
        void PrepareFrameDraw() override;
        void DrawUpdateRects(WindowsSdk::TRect ClipRect) override;
        // Draws all projected pixels, ignoring ClipRect.
        void Draw(WindowsSdk::TRect ClipRect) override;
        void CommitFrameDraw() override;
        std::uint8_t cpp_padding[4];
        EC_CacheGAI::TCGaiControlEC* BackgroundCache;
        TStarFieldList* Stars;
        EC_Struct::TPointF ViewPosition;
        std::int32_t Unknown150;
        std::uint8_t ViewDirty;
        std::uint8_t cpp_padding_2[3];
        PStarFieldPixel Pixels;
        std::int32_t PixelCapacity;
        std::int32_t PixelCount;
        PStarFieldPixel PreviousPixels;
        std::int32_t PreviousPixelCount;
        float BackgroundScale;
        WindowsSdk::TRect PreviousBackgroundBounds;
        WindowsSdk::TRect BackgroundBounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PStarFieldPoint = TStarFieldPoint*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarFieldList : pas::Object {
        PAS_CLASS_META(TStarFieldList, pas::Object, "TStarFieldList", 16)
        void p_destroy() override;
        void Clear();
        // Grows by 100 when incremented Count reaches Capacity.
        PStarFieldPoint AllocatePoint();
        // Depth must be nonzero; retains the low 16 bits of Color.
        void AddPoint(float X, float Y, float Depth, std::int32_t Color);
        PStarFieldPoint Points;
        std::int32_t Count;
        std::int32_t Capacity;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TStarFieldPoint {
        float X;
        float Y;
        float Depth;
        float InverseDepth;
        std::uint16_t Color;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TStarFieldPixel {
        std::int32_t ByteOffset;
        WindowsSdk::TPoint Position;
        std::uint16_t Color;
        std::uint16_t SavedPixel;
    };
    #pragma pack(pop)

} // namespace GI_StarField
