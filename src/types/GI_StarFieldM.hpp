#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/Types.hpp"

namespace GI_StarFieldM {
    struct TMovingStarPixel;

    struct TStarFieldMGI;

    using PMovingStarPixel = TMovingStarPixel*;

    // Native constructor allocates $400 bytes and iterates 32 colors
    // per row and 16 rows. Palette reads retain native helpers.
    using TMovingStarPalette = pas::Array<std::uint16_t, 0, 31>;

    using TMovingStarColorTable = pas::Array<TMovingStarPalette, 0, 15>;

    using PMovingStarColorTable = TMovingStarColorTable*;

    // Native VMT.
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStarFieldMGI : GI_Panel::TPanelGI {
        PAS_CLASS_META(TStarFieldMGI, GI_Panel::TPanelGI, "TStarFieldMGI", 376)
        void p_destroy() override;
        void OnActivate() override;
        void OnDeactivate() override;
        void ClearStars();
        void GrowStars();
        PMovingStarPixel AllocateStar();
        void InitializeStar(PMovingStarPixel Star);
        void SeedStars();
        void AdvanceStars();
        void RedirectStars();
        void AnimateStars(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SetViewPosition(EC_Struct::TPointF Position);
        void Invalidate() override;
        void ErasePreviousFrame() override;
        void PrepareFrameDraw() override;
        void DrawUpdateRects(Types::TRect ClipRect) override;
        void Draw(Types::TRect ClipRect) override;
        void CommitFrameDraw() override;
        std::uint8_t cpp_padding[4];
        PMovingStarPixel Stars;
        std::int32_t StarCount;
        std::int32_t Capacity;
        EC_Struct::TPointF FocusPoint;
        EC_Struct::TPointF ViewPosition;
        float TargetHeading;
        float CurrentHeading;
        float TargetFocusDistance;
        float CurrentFocusDistance;
        std::int32_t MotionTicks;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
        // Owns sixteen rows of 32 RGB words; initialization currently selects row zero.
        PMovingStarColorTable ColorTable;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TMovingStarPixel {
        std::int32_t ByteOffset;
        std::int32_t PreviousByteOffset;
        std::uint8_t cpp_padding[8];
        std::uint16_t SavedPixel;
        std::uint8_t cpp_padding_2[2];
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Velocity;
        EC_Struct::TPointF Acceleration;
        EC_Struct::TPointF Direction;
        Types::TPoint PixelPosition;
        std::int32_t PaletteIndex;
        std::uint16_t Color;
        std::uint8_t cpp_padding_3[2];
        float ColorPosition;
        float ColorStep;
    };
    #pragma pack(pop)

} // namespace GI_StarFieldM
