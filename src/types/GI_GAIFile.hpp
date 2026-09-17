#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_File {
    struct TFileEC;

} // namespace EC_File

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_GAIFile {
    struct TGAIFileGI;

    struct TGAIFileThreadGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGAIFileGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TGAIFileGI, GI_MessageLoop::TObjectGI, "TGAIFileGI", 424)
        void p_destroy() override;
        void Clear() override;
        void OpenImage();
        void CloseImage();
        void* GetFrameData(std::int32_t FrameIndex);
        void TrimFrameCache();
        std::int32_t GetFrameCount();
        WindowsSdk::TPoint GetContentSize();
        WindowsSdk::TPoint GetContentOrigin();
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        void SetSize(WindowsSdk::TPoint Size) override;
        void SetFrameSequence(pas::WideString Sequence);
        std::int32_t GetSequenceFrame(std::int32_t Index);
        std::int32_t GetFrameDelay(std::int32_t Index);
        void OnDeactivate() override;
        void virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        void AdvanceFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void Draw(WindowsSdk::TRect ClipRect) override;
        EC_File::TFileEC* ImageFile;
        GR_gi::TGaiHeader Header;
        void* FrameDirectory;
        void* FrameBuffers;
        TGAIFileThreadGI* LoaderThread;
        pas::CriticalSection* FrameLock;
        std::int32_t PreloadCount;
        GR_gi::TgiGR* FrameImage;
        GI_MessageLoop::PCallbackTimerGI FrameTimer;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t cpp_padding[2];
        std::int32_t CurrentFrame;
        std::int32_t SequenceFrameCount;
        void* SequenceFrames;
        void* FrameDelays;
        std::uint8_t UsePlaybackBuffer;
        std::uint8_t cpp_padding_2[3];
        GR_GraphBuf::TGraphBufGR* PlaybackBuffer;
        std::int32_t LastBufferedFrame;
        std::uint32_t TransparentColor;
        std::uint8_t cpp_padding_3[4];
        GI_MessageLoop::TObjectNotifyEventGI CycleCompleteCallback;
        std::uint8_t Stopped;
        std::uint8_t cpp_padding_4[3];
        std::uint32_t AutoUpdateFlags;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGAIFileThreadGI : EC_Thread::TThreadEC {
        PAS_CLASS_META(TGAIFileThreadGI, EC_Thread::TThreadEC, "TGAIFileThreadGI", 48)
        void virtual_TThreadEC_Execute() override;
        TGAIFileGI* Owner;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_GAIFile
