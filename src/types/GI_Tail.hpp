#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGAI {
    struct TCGaiControlEC;

} // namespace EC_CacheGAI

namespace GI_Tail {
    struct TTailSegmentGI;

    struct TTailGI;

    #pragma pack(push, 1)
    struct TTailSegmentGI {
        std::uint8_t Active;
        std::uint8_t cpp_padding[3];
        std::int32_t FrameIndex;
        EC_Struct::TPointF Position;
        EC_Struct::TPointF Velocity;
        Types::TPoint PixelPosition;
    };
    #pragma pack(pop)

    using PTailSegmentGI = TTailSegmentGI*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTailGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TTailGI, GI_MessageLoop::TObjectGI, "TTailGI", 352)
        void p_destroy() override;
        // Preserves timers and emission state.
        void ClearSegments();
        // Requires at least one GAI sequence. Existing segments are kept.
        void SetImagePath(const pas::WideString& ImagePath);
        pas::WideString GetImagePath();
        // Reuses the last inactive slot or grows by 16. Growth can invalidate earlier pointers; only Active is initialized.
        PTailSegmentGI AllocateSegment();
        void AdvanceSegmentFrames(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void MoveSegments(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Suppresses emission within squared distance 0.001 of the last live segment.
        void EmitSegment(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void OffsetSegments(EC_Struct::TPointF Delta);
        // Deactivation cancels timers. Drawing restarts them when Emitting is true.
        void SetActive(std::uint8_t Enabled) override;
        // Disabling emission leaves existing segments animating.
        void SetEmitting(std::uint8_t Enabled);
        void Invalidate() override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Empty in the native binary.
        static void LoadTailProperties(EC_BlockPar::TBlockParEC* Block);
        // Empty; does not call inherited UpdateAutoGeometry.
        void UpdateAutoGeometry() override;
        void Draw(Types::TRect ClipRect) override;
        // Ignores ClipRect; uses the message loop's update rectangles.
        void DrawUpdateRects(Types::TRect ClipRect) override;
        EC_CacheGAI::TCGaiControlEC* ImageCache;
        std::int32_t FrameCount;
        std::int32_t SegmentCapacity;
        pas::DynArray<TTailSegmentGI> Segments;
        Types::TPoint ImageSize;
        std::int32_t LastSegmentIndex;
        EC_Struct::TPointF EmitterPosition;
        EC_Struct::TPointF SegmentVelocity;
        GI_MessageLoop::PCallbackTimerGI FrameTimer;
        GI_MessageLoop::PCallbackTimerGI MoveTimer;
        GI_MessageLoop::PCallbackTimerGI EmitTimer;
        std::int32_t EmitIntervalMs;
        std::uint8_t Emitting;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Tail
