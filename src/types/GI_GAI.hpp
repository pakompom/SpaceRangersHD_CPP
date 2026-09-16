#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_CacheGAI {
    struct TCGaiControlEC;

} // namespace EC_CacheGAI

namespace EC_CacheGI {
    struct TCGiControlEC;

} // namespace EC_CacheGI

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_GAI {
    struct TgaiGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TgaiGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TgaiGI, GI_MessageLoop::TObjectGI, "TgaiGI", 380)
        void p_destroy() override;
        // Preserves animation state.
        void Clear() override;
        // Resets sequence position even when the key is unchanged.
        void SetImagePath(const pas::WideString& ImagePath);
        pas::WideString GetImagePath();
        void SetFirstFrameImagePath(const pas::WideString& ImagePath);
        pas::WideString GetFirstFrameImagePath();
        // Does not validate the index.
        void SetSequenceFrame(std::int32_t FrameInSequence);
        // Accepted out-of-range positions become zero.
        void SetFramePosition(std::int32_t FrameInSequence, std::uint8_t ForwardOnly);
        // Returns zero in FirstFrameOnly mode.
        std::int32_t GetMainImageFrameCount();
        void StopAutoPlayback();
        // Does not reset frame position; single-frame sequences remain timer-free.
        void RestartPlayback();
        WindowsSdk::TPoint GetContentSize();
        WindowsSdk::TPoint GetContentOrigin();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetAlpha(std::uint8_t Value);
        void SetSize(WindowsSdk::TPoint Size) override;
        void ClearFrameSequence();
        // Accepts ascending and descending ranges; changes the playback timer unless stopped.
        void LoadFrameSequenceFromText(const pas::WideString& FrameSpec);
        std::int32_t GetSequenceCount();
        // Does not validate the index.
        std::int32_t GetSequenceFrameSourceIndex(std::int32_t FrameInSequence);
        // Does not validate the index.
        void SetFrameDelay(std::int32_t FrameInSequence, std::int32_t DelayMs);
        // Does not validate the index.
        std::int32_t GetFrameDelay(std::int32_t FrameInSequence);
        // Black pixels do not count as hits; composed playback may require an existing composition buffer.
        std::uint8_t HitTestPixel(WindowsSdk::TPoint Point);
        void SetActive(std::uint8_t Value) override;
        void OnDeactivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadAnimationProperties(EC_BlockPar::TBlockParEC* Block);
        // Also rebuilds frame tables when SequenceIndex is nonnegative.
        void UpdateAutoGeometry() override;
        // Enables StopAfterOneCycle; frame delays are rounded to milliseconds with a minimum of one.
        void SetOneCycleDuration(std::int32_t DurationMs);
        void AdvanceAutoFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void Invalidate() override;
        void SetHardwareMirrorHorizontal(std::uint8_t Value);
        void Draw(WindowsSdk::TRect ClipRect) override;
        // Skips the main GAI in FirstFrameOnly mode.
        void PrimeImageCaches();
        void QueueImageLoad(pas::List* PendingLoads) override;
        EC_CacheGAI::TCGaiControlEC* MainImageCache;
        EC_CacheGI::TCGiControlEC* FirstFrameImageCache;
        GI_MessageLoop::PCallbackTimerGI AutoFrameTimer;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t Alpha;
        std::uint8_t cpp_padding[1];
        // Playback position is within the selected sequence, not the source image.
        std::int32_t SequenceFrame;
        std::int32_t SequenceFrameCount;
        std::int32_t* SequenceFrameIndexTable;
        std::int32_t* SequenceFrameDelayTable;
        std::int32_t SequenceIndex;
        std::uint8_t UsesPlaybackBuffer;
        std::uint8_t cpp_padding_2[3];
        GR_GraphBuf::TGraphBufGR* CachedPlaybackGraphBuf;
        std::int32_t LastCachedFrameIndex;
        std::uint32_t TransparentColor;
        std::uint8_t cpp_padding_3[4];
        GI_MessageLoop::TObjectNotifyEventGI CycleCompleteCallback;
        GI_MessageLoop::TObjectNotifyEventGI FrameAdvancedCallback;
        std::uint8_t SkipImageUpdateRect;
        std::uint8_t StopPlaybackRequested;
        std::uint8_t StopAfterOneCycle;
        std::uint8_t cpp_padding_4[1];
        pas::WideString StartSoundName;
        std::uint8_t FirstFrameOnly;
        std::uint8_t cpp_padding_5[3];
        std::uint32_t AutoUpdateFlags;
        // Passed to hardware texture drawing only.
        std::uint8_t HardwareMirrorHorizontal;
        std::uint8_t cpp_padding_6[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_GAI
