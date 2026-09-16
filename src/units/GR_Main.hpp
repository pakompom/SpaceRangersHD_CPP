#pragma once
#include "types/Direct3D9.hpp"
#include "types/EC_OKGF.hpp"
#include "types/EC_Str.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Main.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Cache {
    struct TCacheEC;

} // namespace EC_Cache

namespace EC_Data {
    struct TDataEC;

} // namespace EC_Data

namespace EC_Thread {
    struct TThreadEC;

} // namespace EC_Thread

namespace GR_GraphBufPal {
    struct TGraphBufPalGR;

} // namespace GR_GraphBufPal

namespace GR_Music {
    struct TMusicControl;

} // namespace GR_Music

namespace GR_Sound {
    struct TSoundControl;

} // namespace GR_Sound

namespace GR_Main {
    // Message pump returns without sleeping when active; cleared during device loss.
    extern std::uint8_t RuntimeActive;

    extern std::uint8_t VSyncEnabled;

    // User setting PathGrow; permits extending the shared path-node pool.
    extern std::uint8_t PathGrowEnabled;

    // User setting ShowSystemMouse; uses Windows cursor handles instead of drawing the image child.
    extern std::uint8_t ShowSystemMouse;

    extern GR_GraphBuf::TGraphBufGR* ScreenRenderBuffer;

    extern GR_GraphBuf::TGraphBufGR* RenderScratchBuffer;

    // Second shared scratch buffer, also used for captured screen backgrounds.
    extern GR_GraphBuf::TGraphBufGR* AuxRenderBuffer;

    extern pas::WideString SelectedLanguage;

    extern pas::WideString RequestedLanguage;

    extern pas::WideString AvailableLanguageCodes;

    extern pas::WideString OverrideGameUserDirectory;

    extern GR_GraphBuf::TPixelFormatGR* CurrentPixelFormat;

    extern std::int32_t GameScreenWidth;

    extern std::int32_t GameScreenHeight;

    extern std::int32_t PresentationWidth;

    extern std::int32_t PresentationHeight;

    extern WindowsSdk::TPoint ViewportOffset;

    // Enables scaled or panned software presentation and mouse-coordinate conversion.
    extern std::uint8_t AlternateViewportEnabled;

    extern WindowsSdk::TRect GameScreenRect;

    extern WindowsSdk::TRect PresentationRect;

    extern WindowsSdk::TRect ScrollInteriorRect;

    extern std::uint32_t MainWindowHandle;

    extern pas::DynArray<EC_Str::TWideCasePair> WideCaseTable;

    extern EC_BlockPar::TBlockParEC* InstallConfig;

    extern EC_BlockPar::TBlockParEC* LanguageInstallConfig;

    extern EC_BlockPar::TBlockParEC* UserSettingsConfig;

    extern EC_BlockPar::TBlockParEC* MainDataConfig;

    extern EC_BlockPar::TBlockParEC* LanguageDataConfig;

    extern EC_BlockPar::TBlockParEC* UiStyleConfig;

    extern pas::WideString SelectedMods;

    extern pas::WideString SelectedModsDisplaySuffix;

    extern pas::WideString LoadedSaveModSet;

    // Startup failure guard; other writers remain to be recovered.
    extern std::uint8_t SuppressModRetryPrompt;

    extern std::uint8_t SkipModsOnReload;

    extern std::uint8_t WindowedModeRequested;

    extern std::uint8_t ExitScreenLoop;

    extern std::uint8_t FullFrameRedrawRequested;

    extern float DisplayBrightness;

    extern float DisplayContrast;

    extern float RobotBrightness;

    extern float RobotContrast;

    extern std::uint8_t OffscreenFrameUpdated;

    extern std::uint32_t OffscreenLastPresentationTick;

    // 256 RGB565 colors blended between (8,32,255) and (200,128,128).
    extern void* InterfaceBlendPalette;

    extern std::int32_t RequestedRefreshRate;

    extern std::uint8_t PresentWithoutLimit;

    extern std::uint8_t DisableHardwareVertexProcessing;

    extern std::uint8_t DisableMultithreadFlag;

    extern std::uint8_t DisableTripleBuffer;

    // Owns TBlockParEC entries loaded from selected mods' Install.txt files.
    extern pas::List* ModInstallConfigs;

    // Owns the selected mods' language-specific install blocks.
    extern pas::List* ModLanguageInstallConfigs;

    extern std::uint8_t ApplyEditableSaveOnLoad;

    extern pas::WideString EditableSaveFileName;

    extern EC_BlockPar::TBlockParEC* ModShipNameConfig;

    extern EC_BlockPar::TBlockParEC* ModRuinNameConfig;

    // User-supplied galaxy seed, edited by CheatSeed.
    extern pas::WideString NewGameSeedText;

    extern std::uint8_t CacheLoadLoggingEnabled;

    extern GR_Sound::TSoundControl* SoundManager;

    extern GR_Music::TMusicControl* MusicManager;

    extern std::uint8_t ShowFrameRate;

    extern std::uint8_t RecordingFrames;

    extern pas::List* RecordingFrameBuffers;

    extern GR_Main::TCursorUnit* FirstRegisteredCursor;

    extern GR_Main::TCursorUnit* LastRegisteredCursor;

    extern std::uint8_t BuildVersionMismatch;

    extern pas::CriticalSection* SessionLogLock;

    // Receives Ctrl+Shift keys when Alt is not held.
    extern GR_Main::TDebugKeyCallbackGR DebugKeyCallback;

    // Gates message-loop cursor selection and restoration.
    extern std::uint8_t CustomCursorEnabled;

    extern std::uint32_t DirectXVersion;

    extern std::int32_t RecordingFrameCount;

    // 1000 div FilmFPS; native does not check for zero.
    extern std::int32_t RecordingFrameInterval;

    extern std::uint32_t LastRecordingFrameTick;

    // Keep these zero-filled globals consecutive and in this order. Native
    // VerifyStartupModuleChecksum subtracts 8 from StartupChecksumAnchor's address;
    // other routines access each variable directly. DCC32 preserves this storage order.
    // Signed integrity marker: positive after a failed startup module checksum, negative after a clean check; reset by TMessageLoopGI.Present.
    extern std::int32_t UnknownPresentState;

    extern std::uint32_t LastMouseMessageTick;

    // Checksum helper accesses UnknownPresentState at byte offset -8; original anchor meaning unresolved.
    extern std::int32_t StartupChecksumAnchor;

    // Set across MatrixGame Run, including its exception handler.
    extern std::uint8_t RobotBattleActive;

    // Counts CentralProcessor registry subkeys, with a minimum of one.
    extern std::int32_t ProcessorCoreCount;

    extern Direct3D9::IDirect3D9 Direct3D;

    extern Direct3D9::IDirect3DDevice9 Direct3DDevice;

    extern Direct3D9::IDirect3DTexture9 OffscreenTexture;

    // Fill/crop instead of fitting the entire video frame.
    extern std::uint8_t OffscreenFillViewport;

    extern std::uint8_t UseDesktopDisplayMode;

    extern std::int32_t GameDisplayModeCount;

    extern std::int32_t SelectedGameDisplayMode;

    extern std::int32_t SmallestGameDisplayMode;

    extern std::uint8_t UseAutomaticRobotDisplayMode;

    extern std::int32_t RobotDisplayModeCount;

    extern std::int32_t SelectedRobotDisplayMode;

    extern std::uint8_t AltResolutionSwitch;

    extern std::uint32_t LastPresentationTick;

    extern std::uint32_t PresentationFrameRate;

    extern EC_Thread::TThreadEC* RuntimeWatchdog;

    extern std::int32_t PresentationDepth;

    extern EC_BlockPar::TBlockParEC* EditableSaveBlock;

    // Borrowed MainDataConfig.Data block; contains StyleColor.
    extern EC_BlockPar::TBlockParEC* GameDataConfig;

    // Optional user-directory newgame.txt.
    extern EC_BlockPar::TBlockParEC* NewGameSettingsConfig;

    // Borrowed MainDataConfig.ZPos depth-name table.
    extern EC_BlockPar::TBlockParEC* UiDepthConfig;

    extern EC_Data::TDataEC* CacheDataRoot;

    extern EC_Cache::TCacheEC* GlobalCache;

    extern pas::TextFile SessionLog;

    extern GR_GraphBuf::TGraphBufGR* SavePreviewGraph;

    extern GR_GraphBuf::TGraphBufGR* SecondarySavePreviewGraph;

    extern std::int64_t PerformanceCounterFrequency;

    extern std::uint32_t DebugCommandMessage;

    // Consumed by to skip the separate exception-log copy.
    extern std::uint8_t SuppressExceptionLogCopy;

    extern GR_Main::TBlendPixel16 BlendPixel16;

    // OKGF_Triangle_16 callback; cdecl pixel, pitch, vertex/color arguments.
    extern GR_Main::TTriangleRasterizer16 TriangleRasterizer16;

    // OKGF_LineIp_16 callback; cdecl pixel, pitch, vertex/color arguments.
    extern GR_Main::TLineRasterizer16 LineRasterizer16;

    // Assigned by Rangers.start; no native reads indexed.
    extern pas::Proc<void()> RuntimeExitCheckCallback1;

    // Assigned by Rangers.start; no native reads indexed.
    extern pas::Proc<void()> RuntimeExitCheckCallback2;

    extern GR_Main::TRuntimeCallbackGR OnMessageIdle;

    extern GR_Main::TRuntimeCallbackGR OnMessageResume;

    // Owned here: direct startup/helper accesses; other units use reference cell.
    extern GR_Main::TCCInterface* CCInterface;

    extern std::uint32_t RuntimeStartupTick;

    extern std::uint32_t MainRuntimeThreadId;

    extern GR_Main::TDisplayModeGR DesktopDisplayMode;

    extern Direct3D9::TD3DPresentParameters Direct3DPresentParameters;

    extern Direct3D9::TD3DPresentParameters PreviousPresentParameters;

    extern pas::DynArray<GR_Main::TDisplayModeGR> GameDisplayModes;

    extern pas::DynArray<GR_Main::TDisplayModeGR> RobotDisplayModes;

    extern std::int32_t ExtraScreenWidth;

    extern std::int32_t ExtraScreenHeight;

    extern pas::WideString CachedGameUserDirectory;

    // Cleared by settings initialization; no retained reader found, original meaning unresolved.
    extern std::uint32_t StartupState;

    extern std::uint32_t ScreenCenterX;

    extern std::uint32_t ScreenCenterY;

    extern std::uint32_t LastWindowMessageTick;

    extern std::uint8_t MessageIdle;

    TCursorUnit* AddCursorUnit();

    void RemoveCursorUnit(TCursorUnit* Cursor);

    // Case-sensitive lookup; raises when absent.
    TCursorUnit* FindCursorByName(const pas::WideString& Name);

    // Uses a zero key/button state.
    void PostMouseMoveMessage();

    // Native uses an explicit indirect jump into a generated fault sequence if the watchdog stops.
    void CheckRuntimeWatchdog();

    std::int32_t PAS_STDCALL MainWindowProc(std::uint32_t Window, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam);

    // Creates a 300x225 RGB preview and equally sized scratch buffer.
    void CaptureSavePreview();

    void FreeSavePreviewBuffers();

    // Returns a trailing directory separator.
    pas::WideString GetGameUserDirectory();

    void CreateStartupLogFile();

    // Samples the low 32 bits of RDTSC over 200ms; returns 1500 on an exception.
    double MeasureCpuClockMHz();

    // ANSI registry API, fixed 2048-byte buffer, REG_SZ only.
    pas::WideString ReadRegistryText(std::uint32_t Root, pas::WideString KeyPath, pas::WideString ValueName, pas::WideString DefaultValue);

    void ApplyProcessAffinity();

    // Native entry exits before the retained module/process checks; the entire dormant body is preserved.
    void CheckPlatformModules();

    void InitializePlatformRuntimeAndMainWindow();

    // Falls back to Russian when the selected language is unavailable; raises on package-open failure.
    void LoadLanguageAndPackages();

    void LoadSelectedModInstallBlocks();

    void ResetInstalledPackageState();

    void FinalizePlatformRuntime();

    // Requires a WOW64 process and the filesystem-redirection and extended registry APIs.
    std::uint8_t HasWow64Support();

    void ApplyMainWindowGeometry();

    void ShowAndFocusMainWindow();

    void LoadDatConfigAndModOverrides();

    void FreeDatConfigRoots();

    std::uint8_t IsInstallFeatureEnabled(const pas::WideString& Path);

    void LoadInformationColorTags();

    void InitializeRuntimeAndSettings();

    void FinalizeRuntimeAndSettings();

    // Keeps the highest refresh rate for each size. A zero-width entry means automatic resolution; a custom size may be appended.
    void EnumerateAndSelectDisplayModes();

    void ConfigureDefaultRenderState();

    void PreparePresentationParameters();

    void FreeScreenRenderBuffers();

    void LogPresentationParameters();

    pas::AnsiString Direct3DErrorText(std::int32_t Code);

    void GR_DXInit();

    // Linear RGB ramp with brightness/contrast endpoints; returns when no device is present.
    void ApplyGammaRamp(float Brightness, float Contrast);

    void GR_DXReset();

    // Increments the nesting count and always returns true.
    std::uint8_t BeginFramePresentation();

    // Presents at the outermost level, subject to the frame-rate limit.
    void EndFramePresentation();

    // Hardware mode ends/presents/restarts the scene; software mode draws the buffer texture unless OffscreenTexture is assigned.
    void PresentScreenBuffer();

    // Method callback receives Context/EAX, Message/EDX, WParam/ECX and LParam on stack. Returns zero when exiting.
    std::int32_t GR_WinMessage(TWindowMessageCallbackGR Callback);

    // Presents OffscreenTexture, fitting or cropping it to the viewport.
    void DrawOffscreenTexture();

    void CaptureScreenBackground(std::uint8_t ApplyEffects, std::uint8_t UnusedOption);

    void CopyBgraToRgb24(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height);

    // Copies a due software frame into the recording ring and flushes it when full.
    void CaptureRecordingFrame();

    // Writes pending RGB565 frames as sequential Film\NNNNNN.bmp files.
    void FlushRecordingFrames();

    // Tests bit 15 of GetAsyncKeyState.
    std::uint8_t IsVirtualKeyDown(std::int32_t Key);

    // Returns one raw value, or a marker containing Path on lookup failure.
    pas::WideString LookupLocalizedTextByKey(const pas::WideString& Path);

    // Returns one raw value; missing paths return empty and may create intermediate blocks.
    pas::WideString LookupLocalizedTextOrEmpty(const pas::WideString& Path);

    pas::WideString FormatUnixDateTime(std::uint32_t Value);

    // Always returns 2 in this binary.
    pas::WideString GiResourceSuffix();

    // Always returns 2; variant 1 retains the legacy quest-picture downscaling branch.
    std::int32_t GiResourceVariant();

    // Appends a line to the session log and closes the file. The lock is not released if a write raises.
    void AppendLogLineThreadSafe(const pas::AnsiString& Text);

    // Creates #####add.log when absent; native unchecked TextFile I/O.
    void AppendDebugLogLine(const pas::AnsiString& Text);

    // Appends only when #####add.log already exists; shares SessionLogLock and native unchecked TextFile I/O.
    void AppendOptionalDebugLogLine(const pas::AnsiString& Text);

    // Appends without a newline, flushes and closes the file.
    void AppendLogTextThreadSafe(const pas::AnsiString& Text);

    void LogMemoryUsage();

    // Delphi wrappers translate DLL exceptions into Exception objects.
    // Borrows Source until ReadImagePixels consumes the context. Returns nil for unsupported input. Detection requires at least 34 bytes and accepts BMP, JFIF JPEG, PNG and supported PSD modes.
    EC_OKGF::POkgfReadContext BeginImageRead(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height);

    // Consumes Context on success; returns nonzero on success.
    std::int32_t ReadImagePixels(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t BytesPerPixel);

    // Borrows Source; accepts indexed PNG and indexed or grayscale PSD. Returns nil on failure. BytesPerPixel is one or two.
    EC_OKGF::POkgfReadContext BeginIndexedImageRead(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height, std::int32_t& PaletteCount, std::int32_t& BytesPerPixel);

    // Consumes Context on success. Palette requires the count returned by BeginIndexedImageRead.
    std::int32_t ReadIndexedImagePixels(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, GR_GraphBuf::PColorRGBA Palette);

    std::int32_t WritePngFile(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::int32_t HasAlpha, std::int32_t SwapRedBlue);

    std::int32_t WriteBmpFile(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t BitsPerPixel, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t Width, std::int32_t Height);

    // Delphi exception wrappers around the named OKGF/OKGR DLL exports.
    // Ex_ distinguishes wrappers from DLL import symbols; full parameter types remain unresolved.
    void* Ex_OKGF_MulTable256x256();

    std::uint32_t Ex_OKGF_DXVersion();

    void Ex_OKGR_AlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_TransAlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_AlphaIndexed_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_AlphaIndexed_AlphaDraw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_TransBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_TransBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_TransBuf_HADrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    std::int32_t Ex_OKGR_TransBuf_Build_WORD(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest, std::uint16_t TransparentColor);

    std::int32_t Ex_OKGR_TransBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    void Ex_OKGR_TransAlphaBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_AlphaBuf_DrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    std::int32_t Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    std::int32_t Ex_OKGR_AlphaBuf_BuildFromRGBA(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    void Ex_OKGR_AlphaSimpleBuf_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_AlphaSimpleBufPalAlpha_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, GR_GraphBuf::PColorRGBA Palette);

    void Ex_OKGR_MaskBuf_DrawClip_DWORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint32_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_MaskBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_TransBuf_FillAlphaClip_RGBA(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint32_t Color);

    void Ex_OKGR_TransBuf_FillAlphaClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint16_t Color);

    void Ex_OKGR_AlphaIndexed_CopyDrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha);

    void Ex_OKGR_AlphaIndexed_AlphaDrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha);

    void* Ex_OKGR_RotateBuf_Build(std::int32_t Width, std::int32_t Height, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t CenterX, std::int32_t CenterY);

    void Ex_OKGR_RotateBuf_Free(void* Buffer);

    void Ex_OKGR_RotateBuf_Size(std::int32_t X, std::int32_t Y, std::uint8_t Angle, void* RotationMap, WindowsSdk::TRect& Bounds);

    void Ex_OKGR_RotateBuf_Draw_DWORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap);

    void Ex_OKGR_RotateBuf_Draw_BYTE(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, std::uint8_t Angle, void* RotationMap);

    void Ex_OKGR_RotateBuf_DrawTransClip_WORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap, const WindowsSdk::TRect& Clip);

    void* Ex_OKGR_LightBuf_Create(std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_LightBuf_Destroy(void* Buffer);

    void Ex_OKGR_LightBuf_SetSme(void* Buffer, std::int32_t X, std::int32_t Y);

    void Ex_OKGR_LightBuf_Init(void* Buffer, std::uint8_t Value);

    void Ex_OKGR_LightBuf_LoadFromPalBuf(void* Buffer, void* Source, std::int32_t Width, std::int32_t Height, std::int32_t Pitch, GR_GraphBuf::PColorRGBA Palette);

    void Ex_OKGR_LightBuf_Rotate(void* Dest, void* Source, void* RotationMap, std::uint8_t Angle);

    void* Ex_OKGR_Planet2_TemplBuild(void* Source, std::int32_t Pitch, std::int32_t Height, std::int32_t TextureWidth, std::int32_t TextureHeight, std::int32_t& ByteCount);

    std::int32_t Ex_OKGR_Planet2_TemplDel(void* TemplateData);

    void Ex_OKGR_Planet2_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void Ex_OKGR_Planet2_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Planet3_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void Ex_OKGR_Planet3_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Planet4_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void Ex_OKGR_Planet4_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Copy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_PalCopy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, void* Palette, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_CopyTrans_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint16_t TransparentColor);

    void Ex_OKGR_CopySingleBuf_XY_XY_WORD(void* Pixels, std::int32_t Pitch, std::int32_t DestX, std::int32_t DestY, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_HACopy_XY_XY_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_StretchGdi_WORD(void* Dest, std::uint32_t Width, std::uint32_t Height, void* Source, std::uint32_t SourceWidth, std::uint32_t SourceHeight);

    void Ex_OKGR_Fill_WORD(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint16_t Color);

    void Ex_OKGF_ConvertRGBto565(void* Source, void* Dest, std::int32_t Pitch, std::int32_t Width, std::int32_t Height);

    void Ex_OKGF_Convert565toRGB(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void Ex_OKGF_Convert565toBGR(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void Ex_OKGF_Convert565toBGRA(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void Ex_OKGF_Convert_8888to565(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_ShrLight_16(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::int32_t Shift);

    void Ex_OKGR_ShrLightMask_16(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height);

    void Ex_OKGR_Light_BYTE(void* Pixels, std::int32_t PixelStride, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint8_t Alpha);

    void Ex_OKGR_Circle_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Circle_DrawClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Circle_DrawFillClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Circle_DrawFillClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip);

    std::int32_t Ex_OKGR_Line_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, const WindowsSdk::TRect& Clip);

    // Native implementation ignores Pixels/Pitch and draws into ScreenRenderBuffer.
    void DrawGradientLine16Clipped(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, WindowsSdk::TRect Clip);

    std::int32_t Ex_OKGR_LineColor_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::uint32_t> Color1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, pas::Var<std::uint32_t> Color2, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Line_Draw_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color);

    void Ex_OKGR_Line_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    std::int32_t Ex_OKGR_Line_CopyToBuf_WORD(void* Dest, void* Source, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2);

    // Restores one saved 16-bit pixel per rasterized line point; returns the pixel count.
    std::int32_t Ex_OKGR_Line_CopyFromBuf_WORD(void* Source, void* Dest, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2);

    void Ex_OKGR_Line_DrawClip_Alpha_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_AnimLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_AnimShadowLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip, void* ShadowPixels, std::int32_t ShadowPitch);

    void Ex_OKGR_Alpha64Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_Alpha128Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGR_FillTrapezium_DWORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t X2, std::int32_t Y1, std::int32_t X3, std::int32_t X4, std::int32_t Y2, std::uint32_t Color, const WindowsSdk::TRect& Clip);

    void Ex_OKGF_Rescale(void* Dest, std::int32_t Width, std::int32_t Height, std::int32_t DestPitch, void* Source, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t SourcePitch, std::int32_t BytesPerPixel, std::int32_t Filter);

    void Ex_OKGR_F5_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source);

    void Ex_OKGR_F6_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source);

    // Decoded: 'libogg-0', 'libvorbis-0', 'libvorbisfile', 'matrixgame',
    // 'okgf', 'steam_ach', 'steam_api', 'xvidcore', 'zlib'.
    // Differences from the 1024x768 UI baseline; may be negative.
    // Identity function in this binary.
    std::int32_t GiScalePixels(std::int32_t Value);

    // Returns Value; AlternateValue is unused in this binary.
    std::int32_t GiScalePixelsEx(std::int32_t Value, std::int32_t AlternateValue);

    // Converts the borrowed UTF-16 message to AnsiString and raises Exception.
    void RaiseWideMessage(const pas::WideString& Message);

    void DrawTransparentBuffer16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, WindowsSdk::TRect Clip, std::uint8_t HalfAlpha);

    void CopyPalettedBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, void* Palette, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip);

    void CopyBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip, std::uint8_t UnusedOption);

    void DrawAlphaBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip);

    void ExpandPaletteToBgra(void* Dest, std::int32_t DestPitch, std::uint32_t Width, std::uint32_t Height, void* Source, std::int32_t SourcePitch, void* Palette);

    // Uses Data.StyleColor from Main.dat and the current pixel format. Missing entries use the defaults; malformed configured RGB text may raise.
    std::uint32_t GetStyleColorGI(pas::WideString StyleName, std::int32_t DefaultRed, std::int32_t DefaultGreen, std::int32_t DefaultBlue);

    // Returns a complete opening <color=...> tag. Configured Data.StyleColor text is inserted verbatim; missing entries use the default RGB values.
    pas::WideString GetStyleColorTagGI(pas::WideString StyleName, std::int32_t DefaultRed, std::int32_t DefaultGreen, std::int32_t DefaultBlue);

    void CopyGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip, std::uint8_t HalfAlpha, std::uint8_t UnusedOption);

    void DrawAlphaGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip);

    void CopyTransparentGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip, std::uint16_t TransparentColor);

    void DrawPaletteAlphaBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBufPal::TGraphBufPalGR* Source, WindowsSdk::TRect Clip);

    void BlendPaletteBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBufPal::TGraphBufPalGR* Source, WindowsSdk::TRect Clip);

    pas::WideString GetClipboardWideText();

    void SetClipboardWideText(pas::WideString Text);

    void WriteTextFileThreadSafe(pas::AnsiString FileName, pas::AnsiString Text);

    // CRC32 of the C: volume serial and ANSI processor name. Native code ignores volume-query failure.
    std::uint32_t ComputeMachineFingerprintCRC();

    std::int32_t PAS_STDCALL OKGF_ZLib_Compress(void* Dest, void* Source, std::int32_t SourceSize, std::int32_t Mode);

    std::int32_t PAS_STDCALL OKGF_ZLib_UnCompress(void* Dest, std::int32_t DestCapacity, void* Source, std::int32_t SourceSize);

    std::int32_t PAS_STDCALL GlobalMemoryStatusEx(TMemoryStatusEx& Status);

    void* OKGF_MulTable256x256();

    std::uint32_t OKGF_DXVersion();

    EC_OKGF::POkgfReadContext OKGF_ReadStart_Buf(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height);

    std::int32_t OKGF_Read(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t BytesPerPixel);

    EC_OKGF::POkgfReadContext OKGF_ReadStartPal_Buf(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height, std::int32_t& PaletteCount, std::int32_t& BytesPerPixel);

    std::int32_t OKGF_ReadPal(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, GR_GraphBuf::PColorRGBA Palette);

    std::int32_t OKGF_Write_PNG_File(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::int32_t HasAlpha, std::int32_t SwapRedBlue);

    std::int32_t OKGF_Write_BMP_File(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t BitsPerPixel, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t Width, std::int32_t Height);

    void OKGR_AlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_TransAlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_AlphaIndexed_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_AlphaIndexed_AlphaDraw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_TransBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_TransBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void OKGR_TransBuf_HADrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    std::int32_t OKGR_TransBuf_Build_WORD(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest, std::uint16_t TransparentColor);

    std::int32_t OKGR_TransBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    void OKGR_TransAlphaBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void OKGR_AlphaBuf_DrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    std::int32_t OKGR_TransAlphaBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    std::int32_t OKGR_AlphaBuf_BuildFromRGBA(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest);

    void OKGR_AlphaSimpleBuf_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void OKGR_AlphaSimpleBufPalAlpha_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, GR_GraphBuf::PColorRGBA Palette);

    void OKGR_MaskBuf_DrawClip_DWORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint32_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_MaskBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_TransBuf_FillAlphaClip_RGBA(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint32_t Color);

    void OKGR_TransBuf_FillAlphaClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint16_t Color);

    void OKGR_AlphaIndexed_CopyDrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha);

    void OKGR_AlphaIndexed_AlphaDrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip);

    void OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha);

    void* OKGR_RotateBuf_Build(std::int32_t Width, std::int32_t Height, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t CenterX, std::int32_t CenterY);

    void OKGR_RotateBuf_Free(void* Buffer);

    void OKGR_RotateBuf_Size(std::int32_t X, std::int32_t Y, std::uint8_t Angle, void* RotationMap, WindowsSdk::TRect& Bounds);

    void OKGR_RotateBuf_Draw_DWORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap);

    void OKGR_RotateBuf_Draw_BYTE(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, std::uint8_t Angle, void* RotationMap);

    void OKGR_RotateBuf_DrawTransClip_WORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap, const WindowsSdk::TRect& Clip);

    void* OKGR_LightBuf_Create(std::int32_t Width, std::int32_t Height);

    void OKGR_LightBuf_Destroy(void* Buffer);

    void OKGR_LightBuf_SetSme(void* Buffer, std::int32_t X, std::int32_t Y);

    void OKGR_LightBuf_Init(void* Buffer, std::uint8_t Value);

    void OKGR_LightBuf_LoadFromPalBuf(void* Buffer, void* Source, std::int32_t Width, std::int32_t Height, std::int32_t Pitch, GR_GraphBuf::PColorRGBA Palette);

    void OKGR_LightBuf_Rotate(void* Dest, void* Source, void* RotationMap, std::uint8_t Angle);

    void* OKGR_Planet2_TemplBuild(void* Source, std::int32_t Pitch, std::int32_t Height, std::int32_t TextureWidth, std::int32_t TextureHeight, std::int32_t& ByteCount);

    // The native Delphi binding preserves EAX, but the DLL defines no result contract.
    std::int32_t OKGR_Planet2_TemplDel(void* TemplateData);

    void OKGR_Planet2_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void OKGR_Planet2_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void OKGR_Planet3_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void OKGR_Planet3_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void OKGR_Planet4_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y);

    void OKGR_Planet4_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip);

    void OKGR_Copy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void OKGR_PalCopy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, void* Palette, std::int32_t Width, std::int32_t Height);

    void OKGR_CopyTrans_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint16_t TransparentColor);

    void OKGR_CopySingleBuf_XY_XY_WORD(void* Pixels, std::int32_t Pitch, std::int32_t DestX, std::int32_t DestY, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void OKGR_HACopy_XY_XY_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void OKGR_StretchGdi_WORD(void* Dest, std::uint32_t Width, std::uint32_t Height, void* Source, std::uint32_t SourceWidth, std::uint32_t SourceHeight);

    void OKGR_Fill_WORD(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint16_t Color);

    void OKGF_ConvertRGBto565(void* Source, void* Dest, std::int32_t Pitch, std::int32_t Width, std::int32_t Height);

    void OKGF_Convert565toRGB(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void OKGF_Convert565toBGR(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void OKGF_Convert565toBGRA(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height);

    void OKGF_Convert_8888to565(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height);

    void OKGR_ShrLight_16(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::int32_t Shift);

    void OKGR_ShrLightMask_16(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height);

    void OKGR_Light_BYTE(void* Pixels, std::int32_t PixelStride, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint8_t Alpha);

    void OKGR_Circle_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_Circle_DrawClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_Circle_DrawFillClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_Circle_DrawFillClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_PixelAlpha_16(void* Pixel, std::uint16_t Color, std::uint8_t Alpha);

    std::int32_t OKGR_Line_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, const WindowsSdk::TRect& Clip);

    std::int32_t OKGR_LineColor_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::uint32_t> Color1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, pas::Var<std::uint32_t> Color2, const WindowsSdk::TRect& Clip);

    void OKGR_Line_Draw_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color);

    void OKGR_Line_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    std::int32_t OKGR_Line_CopyToBuf_WORD(void* Dest, void* Source, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2);

    std::int32_t OKGR_Line_CopyFromBuf_WORD(void* Source, void* Dest, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2);

    void OKGR_Line_DrawClip_Alpha_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, const WindowsSdk::TRect& Clip);

    void OKGR_AnimLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip);

    void OKGR_AnimShadowLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip, void* ShadowPixels, std::int32_t ShadowPitch);

    void OKGR_Alpha64Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_Alpha128Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip);

    void OKGR_FillTrapezium_DWORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t X2, std::int32_t Y1, std::int32_t X3, std::int32_t X4, std::int32_t Y2, std::uint32_t Color, const WindowsSdk::TRect& Clip);

    void OKGF_Rescale(void* Dest, std::int32_t Width, std::int32_t Height, std::int32_t DestPitch, void* Source, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t SourcePitch, std::int32_t BytesPerPixel, std::int32_t Filter);

    void OKGR_F5_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source);

    void OKGR_F6_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source);

    // Cdecl triangle ABI verified at native caller.
    void OKGF_Triangle_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, std::int32_t X3, std::int32_t Y3, std::uint32_t Color3, WindowsSdk::PRect Clip);

    // Cdecl gradient-line ABI verified at native caller.
    void OKGF_LineIp_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2);

    void TCCInterface_Create(TCCInterface* Self);

    void TCCInterface_Destroy(TCCInterface* Self);

} // namespace GR_Main
