#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace Robot {
    struct TRobotCallbacks;

    struct TRobotDisplaySettingsPrefix;

    struct TRobotInterfacePrefix;

    struct TRobotTextImage;

    using PRobotCallbacks = TRobotCallbacks*;

    using TRobotInitialize = pas::StdcallProc<void(PRobotCallbacks)>;

    using TRobotAction = pas::StdcallProc<void()>;

    using TRobotSupportQuery = pas::StdcallProc<std::int32_t()>;

    using PRobotDisplaySettings = TRobotDisplaySettingsPrefix*;

    using TRobotRun = pas::StdcallProc<std::int32_t(std::uint32_t, std::uint32_t, char16_t*, PRobotDisplaySettings, char16_t*, char16_t*, char16_t*, char16_t*, char16_t*, WindowsSdk::PInteger)>;

    // Dispatch table returned by GetRobotInterface.
    #pragma pack(push, 1)
    struct TRobotInterfacePrefix {
        TRobotInitialize Initialize;
        TRobotAction Finalize;
        // Zero allows entry.
        TRobotSupportQuery Support;
        TRobotRun Run;
    };
    #pragma pack(pop)

    using PRobotInterfacePrefix = TRobotInterfacePrefix*;

    using TRobotPlaySound = pas::StdcallProc<void(char16_t*)>;

    using TRobotCreateSound = pas::StdcallProc<GR_Sound::TSoundBufferControl*(char16_t*, std::int32_t, std::int32_t)>;

    using TRobotSoundAction = pas::StdcallProc<void(GR_Sound::TSoundBufferControl*)>;

    using TRobotSoundQuery = pas::StdcallProc<std::int32_t(GR_Sound::TSoundBufferControl*)>;

    using TRobotSoundSetValue = pas::StdcallProc<void(GR_Sound::TSoundBufferControl*, float)>;

    using TRobotSoundGetValue = pas::StdcallProc<float(GR_Sound::TSoundBufferControl*)>;

    using PRobotTextImage = TRobotTextImage*;

    using TRobotRenderText = pas::StdcallProc<void(char16_t*, char16_t*, std::uint32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, Types::PRect, PRobotTextImage)>;

    using TRobotFreeText = pas::StdcallProc<void(PRobotTextImage)>;

    using TRobotProgress = pas::StdcallProc<void(float)>;

    using TRobotGetVolume = pas::StdcallProc<float()>;

    using TRobotSetVolume = pas::StdcallProc<void(float)>;

    // Rangers callbacks passed to MatrixGame.dll.
    #pragma pack(push, 1)
    struct TRobotCallbacks {
        TRobotPlaySound PlaySound;
        TRobotCreateSound CreateSound;
        TRobotSoundAction FreeSound;
        TRobotSoundAction StartSound;
        TRobotSoundQuery IsSoundPlaying;
        TRobotSoundSetValue SetSoundVolume;
        TRobotSoundSetValue SetSoundPan;
        TRobotSoundGetValue GetSoundVolume;
        TRobotSoundGetValue GetSoundPan;
        TRobotRenderText RenderText;
        TRobotFreeText FreeText;
        TRobotProgress SetProgress;
        TRobotAction PlayMusic;
        TRobotAction ReleaseTextures;
        TRobotGetVolume GetMusicVolume;
        TRobotSetVolume SetMusicVolume;
    };
    #pragma pack(pop)

    // Owned raster returned to the DLL.
    #pragma pack(push, 1)
    struct TRobotTextImage {
        GR_GraphBuf::TGraphBufGR* Buffer;
        void* Pixels;
        std::int32_t Pitch;
        std::int32_t Width;
        std::int32_t Height;
    };
    #pragma pack(pop)

    // Native settings block.
    #pragma pack(push, 1)
    struct TRobotDisplaySettingsPrefix {
        // Borrowed, no interface reference counting.
        void* Direct3D;
        // Borrowed.
        void* Device;
        std::uint8_t ShowStencilShadows;
        std::uint8_t ShowProjShadows;
        std::uint8_t SelectEx;
        std::uint8_t LandTexturesGloss;
        std::uint8_t ObjTexturesGloss;
        std::uint8_t SoftwareCursor;
        std::uint8_t Sky;
        std::uint8_t RobotShadow;
        std::int32_t ColorDepth;
        std::int32_t ScreenWidth;
        std::int32_t ScreenHeight;
        // Zero in windowed mode.
        std::int32_t RefreshRate;
        float Brightness;
        float Contrast;
        std::int32_t FSAASamples;
        std::int32_t Anisotropy;
        float MaxDistance;
        std::uint8_t VSync;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    using TGetRobotInterface = pas::StdcallProc<PRobotInterfacePrefix()>;

} // namespace Robot
