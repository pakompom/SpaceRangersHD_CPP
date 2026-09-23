#include "layout/GR_Main.hpp"
#include "types/Classes.hpp"
#include "types/EC_File.hpp"
#include "types/EC_OKGF.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBufPal.hpp"
#include "types/MessagesSdk.hpp"
#include "types/Types.hpp"
#include "units/ActiveXSdk.hpp"
#include "units/BitmapPorts.hpp"
#include "units/ClassesImports.hpp"
#include "units/ClockPorts.hpp"
#include "units/DateUtils.hpp"
#include "units/Direct3D9.hpp"
#include "units/DirectSound.hpp"
#include "units/DirectXRenderException.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_Data.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/Forms.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Music.hpp"
#include "units/GR_Sound.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/MessageText.hpp"
#include "units/Registry.hpp"
#include "units/Robot.hpp"
#include "units/ShlObj.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPacket.hpp"

namespace GR_Main {
    using TGetNativeSystemInfo = pas::StdcallProc<void(WindowsSdk::TSystemInfo&)>;

    using TIsWow64Process = pas::StdcallProc<std::int32_t(WindowsImports::THandle, std::int32_t&)>;

    // Shared by version display, configuration migration and data compatibility checks.
    const pas::WideString GameVersionText = u"2.1.2500"_w;

    const pas::WideString ModSelectionConfigPath = u"Mods\\ModCFG.txt"_w;

    // Message pump returns without sleeping when active; cleared during device loss.
    std::uint8_t RuntimeActive{};

    std::uint8_t VSyncEnabled{};

    // User setting PathGrow; permits extending the shared path-node pool.
    std::uint8_t PathGrowEnabled{};

    // User setting ShowSystemMouse; uses Windows cursor handles instead of drawing the image child.
    std::uint8_t ShowSystemMouse{};

    GR_GraphBuf::TGraphBufGR* ScreenRenderBuffer{};

    GR_GraphBuf::TGraphBufGR* RenderScratchBuffer{};

    // Second shared scratch buffer, also used for captured screen backgrounds.
    GR_GraphBuf::TGraphBufGR* AuxRenderBuffer{};

    pas::WideString SelectedLanguage{};

    pas::WideString RequestedLanguage{};

    pas::WideString AvailableLanguageCodes{};

    pas::WideString OverrideGameUserDirectory{};

    GR_GraphBuf::TPixelFormatGR* CurrentPixelFormat{};

    std::int32_t GameScreenWidth{};

    std::int32_t GameScreenHeight{};

    std::int32_t PresentationWidth{};

    std::int32_t PresentationHeight{};

    WindowsSdk::TPoint ViewportOffset{};

    // Enables scaled or panned software presentation and mouse-coordinate conversion.
    std::uint8_t AlternateViewportEnabled{};

    WindowsSdk::TRect GameScreenRect{};

    WindowsSdk::TRect PresentationRect{};

    WindowsSdk::TRect ScrollInteriorRect{};

    std::uint32_t MainWindowHandle{};

    pas::DynArray<EC_Str::TWideCasePair> WideCaseTable{};

    EC_BlockPar::TBlockParEC* InstallConfig{};

    EC_BlockPar::TBlockParEC* LanguageInstallConfig{};

    EC_BlockPar::TBlockParEC* UserSettingsConfig{};

    EC_BlockPar::TBlockParEC* MainDataConfig{};

    EC_BlockPar::TBlockParEC* LanguageDataConfig{};

    EC_BlockPar::TBlockParEC* UiStyleConfig{};

    pas::WideString SelectedMods{};

    pas::WideString SelectedModsDisplaySuffix{};

    pas::WideString LoadedSaveModSet{};

    // Startup failure guard; other writers remain to be recovered.
    std::uint8_t SuppressModRetryPrompt = false;

    std::uint8_t SkipModsOnReload = false;

    std::uint8_t WindowedModeRequested = false;

    std::uint8_t ExitScreenLoop = false;

    std::uint8_t FullFrameRedrawRequested = true;

    float DisplayBrightness = 0.0f;

    float DisplayContrast = 0.0f;

    float RobotBrightness = 0.0f;

    float RobotContrast = 0.0f;

    std::uint8_t OffscreenFrameUpdated = false;

    std::uint32_t OffscreenLastPresentationTick = 0u;

    // 256 RGB565 colors blended between (8,32,255) and (200,128,128).
    void* InterfaceBlendPalette = nullptr;

    std::int32_t RequestedRefreshRate = 0;

    std::uint8_t PresentWithoutLimit = false;

    std::uint8_t DisableHardwareVertexProcessing = false;

    std::uint8_t DisableMultithreadFlag = false;

    std::uint8_t DisableTripleBuffer = false;

    // Owns TBlockParEC entries loaded from selected mods' Install.txt files.
    pas::List* ModInstallConfigs = nullptr;

    // Owns the selected mods' language-specific install blocks.
    pas::List* ModLanguageInstallConfigs = nullptr;

    std::uint8_t ApplyEditableSaveOnLoad = false;

    pas::WideString EditableSaveFileName = pas::WideString();

    EC_BlockPar::TBlockParEC* ModShipNameConfig = nullptr;

    EC_BlockPar::TBlockParEC* ModRuinNameConfig = nullptr;

    // User-supplied galaxy seed, edited by CheatSeed.
    pas::WideString NewGameSeedText = pas::WideString();

    std::uint8_t CacheLoadLoggingEnabled = false;

    GR_Sound::TSoundControl* SoundManager = nullptr;

    GR_Music::TMusicControl* MusicManager = nullptr;

    std::uint8_t ShowFrameRate = false;

    std::uint8_t RecordingFrames = false;

    pas::List* RecordingFrameBuffers = nullptr;

    GR_Main::TCursorUnit* FirstRegisteredCursor = nullptr;

    GR_Main::TCursorUnit* LastRegisteredCursor = nullptr;

    std::uint8_t BuildVersionMismatch = false;

    pas::CriticalSection* SessionLogLock = nullptr;

    // Receives Ctrl+Shift keys when Alt is not held.
    GR_Main::TDebugKeyCallbackGR DebugKeyCallback = nullptr;

    // Gates message-loop cursor selection and restoration.
    std::uint8_t CustomCursorEnabled = true;

    std::uint32_t DirectXVersion = 0u;

    std::int32_t RecordingFrameCount = 0;

    // 1000 div FilmFPS; native does not check for zero.
    std::int32_t RecordingFrameInterval = 50;

    std::uint32_t LastRecordingFrameTick = 0u;

    // Native VerifyStartupModuleChecksum subtracts a local byte offset of 8 from
    // StartupChecksumAnchor's address for each marker access ().
    // The subtraction is emitted at runtime, not inferred from adjacent addresses.
    // Keep these initialized globals consecutive and in this order; direct access
    // to StartupIntegrityMarker would remove those native subtraction instructions.
    // Other routines access StartupIntegrityMarker and LastMouseMessageTick directly.
    // Signed integrity marker: positive after a failed startup module checksum, negative after a clean check; reset by TMessageLoopGI.Present.
    std::int32_t StartupIntegrityMarker = 0;

    std::uint32_t LastMouseMessageTick = 0u;

    // Checksum helper accesses StartupIntegrityMarker at byte offset -8; original anchor meaning unresolved.
    std::int32_t StartupChecksumAnchor = 0;

    // Set across MatrixGame Run, including its exception handler.
    std::uint8_t RobotBattleActive = false;

    // Counts CentralProcessor registry subkeys, with a minimum of one.
    std::int32_t ProcessorCoreCount = 1;

    Direct3D9::IDirect3D9 Direct3D = nullptr;

    Direct3D9::IDirect3DDevice9 Direct3DDevice = nullptr;

    Direct3D9::IDirect3DTexture9 OffscreenTexture = nullptr;

    // Fill/crop instead of fitting the entire video frame.
    std::uint8_t OffscreenFillViewport = false;

    std::uint8_t UseDesktopDisplayMode = false;

    std::int32_t GameDisplayModeCount = 0;

    std::int32_t SelectedGameDisplayMode = -1;

    std::int32_t SmallestGameDisplayMode = -1;

    std::uint8_t UseAutomaticRobotDisplayMode = false;

    std::int32_t RobotDisplayModeCount = 0;

    std::int32_t SelectedRobotDisplayMode = -1;

    std::uint8_t AltResolutionSwitch = false;

    std::uint32_t LastPresentationTick = 0u;

    std::uint32_t PresentationFrameRate = 0u;

    EC_Thread::TThreadEC* RuntimeWatchdog = nullptr;

    std::int32_t PresentationDepth = 0;

    EC_BlockPar::TBlockParEC* EditableSaveBlock{};

    // Borrowed MainDataConfig.Data block; contains StyleColor.
    EC_BlockPar::TBlockParEC* GameDataConfig{};

    // Optional user-directory newgame.txt.
    EC_BlockPar::TBlockParEC* NewGameSettingsConfig{};

    // Borrowed MainDataConfig.ZPos depth-name table.
    EC_BlockPar::TBlockParEC* UiDepthConfig{};

    EC_Data::TDataEC* CacheDataRoot{};

    EC_Cache::TCacheEC* GlobalCache{};

    pas::TextFile SessionLog{};

    GR_GraphBuf::TGraphBufGR* SavePreviewGraph{};

    GR_GraphBuf::TGraphBufGR* SecondarySavePreviewGraph{};

    std::int64_t PerformanceCounterFrequency{};

    std::uint32_t DebugCommandMessage{};

    // Consumed by to skip the separate exception-log copy.
    std::uint8_t SuppressExceptionLogCopy{};

    GR_Main::TBlendPixel16 BlendPixel16{};

    // OKGF_Triangle_16 callback; cdecl pixel, pitch, vertex/color arguments.
    GR_Main::TTriangleRasterizer16 TriangleRasterizer16{};

    // OKGF_LineIp_16 callback; cdecl pixel, pitch, vertex/color arguments.
    GR_Main::TLineRasterizer16 LineRasterizer16{};

    // Assigned by Rangers.start; no native reads indexed.
    pas::Proc<void()> RuntimeExitCheckCallback1{};

    // Assigned by Rangers.start; no native reads indexed.
    pas::Proc<void()> RuntimeExitCheckCallback2{};

    GR_Main::TRuntimeCallbackGR OnMessageIdle{};

    GR_Main::TRuntimeCallbackGR OnMessageResume{};

    // Owned here: direct startup/helper accesses; other units use reference cell.
    GR_Main::TCCInterface* CCInterface{};

    std::uint32_t RuntimeStartupTick{};

    std::uint32_t MainRuntimeThreadId{};

    GR_Main::TDisplayModeGR DesktopDisplayMode{};

    Direct3D9::TD3DPresentParameters Direct3DPresentParameters{};

    Direct3D9::TD3DPresentParameters PreviousPresentParameters{};

    pas::DynArray<GR_Main::TDisplayModeGR> GameDisplayModes{};

    pas::DynArray<GR_Main::TDisplayModeGR> RobotDisplayModes{};

    std::int32_t ExtraScreenWidth{};

    std::int32_t ExtraScreenHeight{};

    pas::WideString CachedGameUserDirectory = pas::WideString();

    // Cleared by settings initialization; no retained reader found, original meaning unresolved.
    std::uint32_t StartupState{};

    std::uint32_t ScreenCenterX{};

    std::uint32_t ScreenCenterY{};

    std::uint32_t LastWindowMessageTick{};

    std::uint8_t MessageIdle{};

    TCursorUnit* AddCursorUnit() {
        TCursorUnit* Cursor = pas::make_object<TCursorUnit>();
        if (LastRegisteredCursor != nullptr) {
            LastRegisteredCursor->Next = Cursor;
        }
        Cursor->Prev = LastRegisteredCursor;
        Cursor->Next = nullptr;
        LastRegisteredCursor = Cursor;
        if (FirstRegisteredCursor == nullptr) {
            FirstRegisteredCursor = Cursor;
        }
        return Cursor;
    }

    void RemoveCursorUnit(TCursorUnit* Cursor) {
        if (Cursor->Prev != nullptr) {
            Cursor->Prev->Next = Cursor->Next;
        }
        if (Cursor->Next != nullptr) {
            Cursor->Next->Prev = Cursor->Prev;
        }
        if (LastRegisteredCursor == Cursor) {
            LastRegisteredCursor = Cursor->Prev;
        }
        if (FirstRegisteredCursor == Cursor) {
            FirstRegisteredCursor = Cursor->Next;
        }
        pas::free(Cursor);
    }

    // Case-sensitive lookup; raises when absent.
    TCursorUnit* FindCursorByName(const std::u16string_view& Name) {
        TCursorUnit* Cursor = FirstRegisteredCursor;
        while (Cursor != nullptr) {
            if (pas::view(Cursor->Name) == Name) {
                return Cursor;
            }
            Cursor = Cursor->Next;
        }
        pas::raise(pas::make_exception<pas::Exception>("GR_CursorFind"_a));
    }

    // Uses a zero key/button state.
    void PostMouseMoveMessage() {
        WindowsSdk::TPoint Point{};
        WindowsSdk::GetCursorPos(Point);
        WindowsSdk::ScreenToClient(MainWindowHandle, Point);
        WindowsSdk::PostMessage(MainWindowHandle, MessagesSdk::WM_MOUSEMOVE, 0, static_cast<std::uint16_t>(Point.X) | pas::shl(static_cast<std::int32_t>(static_cast<std::uint16_t>(Point.Y)), 16));
    }

    // Native uses an explicit indirect jump into a generated fault sequence if the watchdog stops.
    void CheckRuntimeWatchdog() {
        if (RuntimeWatchdog != nullptr) {
            if (!RuntimeWatchdog->IsRunning()) {
                // Manual port: the original generated x86 deliberately writes to address zero.
                pas::raise(pas::make_exception<SysUtilsImports::EAccessViolation>("Runtime watchdog stopped"_a));
            }
        }
    }

    std::int32_t PAS_STDCALL MainWindowProc(std::uint32_t Window, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        WindowsSdk::TPoint Origin{};
        WindowsSdk::TRect Bounds{};
        if (Message == MessagesSdk::WM_ACTIVATEAPP) {
            if (WParam != 0) {
                if (Direct3DDevice != nullptr) {
                    if (Direct3D9::IDirect3DDevice9_TestCooperativeLevel(Direct3DDevice) == Direct3D9::D3DERR_DEVICENOTRESET) {
                        GR_Main::GR_DXReset();
                    }
                }
                if (!Direct3DPresentParameters.Windowed) {
                    Origin = ClassesImports::Point(GameScreenRect.Left, GameScreenRect.Top);
                    WindowsSdk::ClientToScreen(MainWindowHandle, Origin);
                    Bounds = ClassesImports::Rect(Origin.X + GameScreenRect.Left, Origin.Y + GameScreenRect.Top, Origin.X + GameScreenRect.Right, Origin.Y + GameScreenRect.Bottom);
                    WindowsSdk::ClipCursor(reinterpret_cast<WindowsSdk::PRect>(&Bounds));
                }
                Forms::Application->OnActivate(nullptr);
            } else {
                if (Direct3DDevice != nullptr) {
                    Direct3D9::IDirect3DDevice9_TestCooperativeLevel(Direct3DDevice);
                }
                if (!Direct3DPresentParameters.Windowed) {
                    WindowsSdk::ClipCursor(nullptr);
                }
                Forms::Application->OnDeactivate(nullptr);
            }
            if (WParam != 0) {
                if (GlobalsV::CurrentScreenId == GlobalsV::screenPlanetNO) {
                    reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->UpdateActionCursor(false);
                } else if (GlobalsV::CurrentScreenId == GlobalsV::screenShip || GlobalsV::CurrentScreenId == GlobalsV::screenStarMap) {
                    reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->UpdateActionCursor(true);
                }
                if (reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::screenShip]) != nullptr && reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::screenShip])->GetActionParentLoop() != nullptr && reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::screenShip])->GetActionParentLoop() == reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])) {
                    reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::screenShip])->UpdateActionCursor(true);
                }
            }
        } else if (Message == MessagesSdk::WM_DESTROY) {
            if (ExitScreenLoop) {
                WindowsSdk::PostQuitMessage(0);
            }
        } else if (Message == MessagesSdk::WM_ERASEBKGND) {
            return 1;
        } else if (Message == MessagesSdk::WM_PAINT) {
            if (Direct3DPresentParameters.Windowed) {
                GR_Main::PresentScreenBuffer();
            }
        } else if (!(Message == MessagesSdk::WM_MOUSEMOVE)) {
            if (!(Message == MessagesSdk::WM_LBUTTONDOWN)) {
                if (!(Message == MessagesSdk::WM_LBUTTONUP)) {
                    if (!(Message == MessagesSdk::WM_RBUTTONDOWN)) {
                        if (!(Message == MessagesSdk::WM_RBUTTONUP)) {
                            if (Message == MessagesSdk::WM_SYSKEYDOWN && pas::in_set<WindowsSdk::VK_MENU, WindowsSdk::VK_MENU, WindowsSdk::VK_LEFT, WindowsSdk::VK_DOWN>(WParam)) {
                                return 1;
                            } else if (Message == MessagesSdk::WM_SYSKEYUP && pas::in_set<WindowsSdk::VK_MENU, WindowsSdk::VK_MENU, WindowsSdk::VK_LEFT, WindowsSdk::VK_DOWN>(WParam)) {
                                return 1;
                            } else if (Message == MessagesSdk::WM_KEYDOWN) {
                                static_cast<void>(WParam == 'R' && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) && GlobalsV::CurrentScreenId != GlobalsV::screenNone);
                            } else if (Message == MessagesSdk::WM_CLOSE) {
                                ExitScreenLoop = true;
                            } else if (Message == MessagesSdk::WM_CANCELMODE) {
                                RuntimeActive = false;
                            } else if (Message == MessagesSdk::WM_SETCURSOR) {
                                return 1;
                            } else if (Message == MessagesSdk::WM_TIMER) {
                                GR_Main::CheckRuntimeWatchdog();
                                return 1;
                            }
                        }
                    }
                }
            }
        }
        return WindowsSdk::DefWindowProcW(Window, Message, WParam, LParam);
    }

    // Creates a 300x225 RGB preview and equally sized scratch buffer.
    void CaptureSavePreview() {
        GR_Main::FreeSavePreviewBuffers();
        SavePreviewGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        SavePreviewGraph->LoadFromScreen(0);
        if (GlobalsV::HardwareRenderingEnabled) {
            SavePreviewGraph->RescaleWithAspect(300u, 225u, true, 1, 1, 5);
            SavePreviewGraph->ConvertBgraToRgb24();
        } else {
            SavePreviewGraph->Convert565ToRgb();
            SavePreviewGraph->RescaleWithAspect(300u, 225u, true, 1, 1, 5);
        }
        SecondarySavePreviewGraph = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        SecondarySavePreviewGraph->AllocateNativePitch(300, 225, 900);
    }

    void FreeSavePreviewBuffers() {
        if (SavePreviewGraph != nullptr) {
            pas::free(SavePreviewGraph);
            SavePreviewGraph = nullptr;
        }
        if (SecondarySavePreviewGraph != nullptr) {
            pas::free(SecondarySavePreviewGraph);
            SecondarySavePreviewGraph = nullptr;
        }
    }

    // Returns a trailing directory separator.
    pas::WideString GetGameUserDirectory() {
        std::uint8_t* PathBuffer{};
        ShlObj::PItemIDList ItemIdList{};
        pas::WideString DocumentsPath{};
        if (CachedGameUserDirectory != u"") {
            return CachedGameUserDirectory;
        } else if (OverrideGameUserDirectory != u"") {
            SysUtilsImports::CreateDir(static_cast<pas::AnsiString>(OverrideGameUserDirectory));
            return pas::concat_wide({OverrideGameUserDirectory, u"\\"});
        } else {
            ShlObj::SHGetSpecialFolderLocation(0u, ShlObj::CSIDL_PERSONAL, ItemIdList);
            PathBuffer = SysUtilsImports::StrAlloc(WindowsImports::MAX_PATH);
            ShlObj::SHGetPathFromIDListA(ItemIdList, PathBuffer);
            ActiveXSdk::CoTaskMemFree(ItemIdList);
            DocumentsPath = static_cast<pas::WideString>(pas::concat_ansi({SysUtilsImports::StrPas(PathBuffer), "\\"}));
            SysUtilsImports::StrDispose(PathBuffer);
            SysUtilsImports::CreateDir(static_cast<pas::AnsiString>(pas::concat_wide({DocumentsPath, u"SpaceRangersHD"})));
            CachedGameUserDirectory = pas::concat_wide({DocumentsPath, u"SpaceRangersHD\\"});
            return CachedGameUserDirectory;
        }
    }

    void CreateStartupLogFile() {
        pas::WideString FileName{};
        FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"########.log"});
        pas::text_assign(SessionLog, static_cast<pas::AnsiString>(FileName), false);
        pas::text_open(SessionLog, 3, false);
        pas::text_writeln(SessionLog, "Start"_a, false);
        pas::text_close(SessionLog, false);
    }

    // Samples the low 32 bits of RDTSC over 200ms; returns 1500 on an exception.
    double MeasureCpuClockMHz() {
        double Result{};
        std::uint32_t TickLow{};
        std::uint32_t ProcessPriority = WindowsSdk::GetPriorityClass(WindowsSdk::GetCurrentProcess());
        std::int32_t ThreadPriority = WindowsSdk::GetThreadPriority(WindowsSdk::GetCurrentThread());
        WindowsSdk::SetPriorityClass(WindowsSdk::GetCurrentProcess(), WindowsSdk::REALTIME_PRIORITY_CLASS);
        WindowsSdk::SetThreadPriority(WindowsSdk::GetCurrentThread(), WindowsSdk::THREAD_PRIORITY_TIME_CRITICAL);
        try {
            SysUtilsImports::Sleep(10u);
            // The native timestamp reads and 64-bit subtraction are handwritten asm.
            TickLow = ClockPorts::ReadCycleCounter();
            SysUtilsImports::Sleep(200u);
            TickLow = ClockPorts::ReadCycleCounter() - TickLow;
            Result = pas::real_divide(TickLow, 2.0E+5L);
        } catch (...) {
            Result = 1.5E+3;
        }
        WindowsSdk::SetThreadPriority(WindowsSdk::GetCurrentThread(), ThreadPriority);
        WindowsSdk::SetPriorityClass(WindowsSdk::GetCurrentProcess(), ProcessPriority);
        return Result;
    }

    // ANSI registry API, fixed 2048-byte buffer, REG_SZ only.
    pas::WideString ReadRegistryText(std::uint32_t Root, pas::WideString KeyPath, pas::WideString ValueName, pas::WideString DefaultValue) {
        pas::AnsiString cpp_text{};
        pas::AnsiString cpp_text_2{};
        pas::WideString Result{};
        WindowsSdk::HKEY Key{};
        std::uint32_t ValueType{};
        std::uint32_t ByteCount{};
        if (WindowsSdk::RegOpenKeyExA(Root, (cpp_text = static_cast<pas::AnsiString>(KeyPath), cpp_text.pchar()), 0u, WindowsSdk::KEY_READ, Key) != WindowsSdk::ERROR_SUCCESS) {
            return DefaultValue;
        }
        ByteCount = 2048u;
        void* Data = EC_Mem::AllocEC(ByteCount);
        if (([&] {
            std::uint8_t* cpp_arg = (cpp_text_2 = static_cast<pas::AnsiString>(ValueName), cpp_text_2.pchar());
            WindowsSdk::HKEY key = Key;
            return WindowsSdk::RegQueryValueExA(key, cpp_arg, nullptr, &ValueType, static_cast<std::uint8_t*>(Data), &ByteCount);
        }()) != WindowsSdk::ERROR_SUCCESS) {
            Result = std::move(DefaultValue);
            WindowsSdk::RegCloseKey(Key);
            EC_Mem::FreeEC(Data);
            return Result;
        }
        if (ValueType != WindowsSdk::REG_SZ) {
            Result = std::move(DefaultValue);
        } else {
            Result = static_cast<pas::WideString>(static_cast<std::uint8_t*>(Data));
        }
        EC_Mem::FreeEC(Data);
        WindowsSdk::RegCloseKey(Key);
        return Result;
    }

    void ApplyProcessAffinity() {
        std::uint32_t Mask = 1u;
        if (GlobalsV::MultiThreadEnabled) {
            while (WindowsSdk::SetProcessAffinityMask(WindowsSdk::GetCurrentProcess(), Mask)) {
                Mask = Mask << 1 | 1;
            }
            return;
        }
        WindowsSdk::SetProcessAffinityMask(WindowsSdk::GetCurrentProcess(), Mask);
    }

    // Native entry exits before the retained module/process checks; the entire dormant body is preserved.
    void CheckPlatformModules() {
        pas::AnsiString GameDirectory{};
        pas::AnsiString ModulePath{};
        pas::WideString DllSuffix{};
        pas::AnsiString SteamClientPath{};
        // This build disables the checks, but Delphi O- retained their native bytes.
        return;
    }

    void InitializePlatformRuntimeAndMainWindow() {
        pas::WideString cpp_text{};
        pas::AnsiString cpp_text_2{};
        pas::WideString cpp_text_3{};
        pas::AnsiString cpp_text_4{};
        pas::AnsiString cpp_text_5{};
        pas::WideString SystemDirectory{};
        WindowsSdk::TWndClassW WindowClass{};
        SystemDirectory.set_length(256);
        std::uint32_t DirectoryLength = WindowsSdk::GetSystemDirectoryW(SystemDirectory.pchar(), 256u);
        SystemDirectory.set_length(static_cast<std::int32_t>(DirectoryLength));
        WindowsSdk::HMODULE Module = WindowsSdk::LoadLibraryW((cpp_text = pas::concat_wide({SystemDirectory, EC_Str::DecodeTextW(u"\\/di34da9..idalal"_w)}), cpp_text.pchar()));
        Direct3D9::Direct3DCreate9 = pas::callback_from_address<Direct3D9::TDirect3DCreate9>(WindowsImports::GetProcAddress(Module, (cpp_text_2 = static_cast<pas::AnsiString>(EC_Str::DecodeTextW(u"Drinroekcata33DICAroevaltaen9"_w)), cpp_text_2.pchar())));
        Module = WindowsSdk::LoadLibraryW((cpp_text_3 = pas::concat_wide({SystemDirectory, EC_Str::DecodeTextW(u"\\/dosdosusnuds.idalal"_w)}), cpp_text_3.pchar()));
        DirectSound::DirectSoundCreate = pas::callback_from_address<DirectSound::TDirectSoundCreate>(WindowsImports::GetProcAddress(Module, (cpp_text_4 = static_cast<pas::AnsiString>(EC_Str::DecodeTextW(u"DrinroekcataSnowusnud.Carvenaltie"_w)), cpp_text_4.pchar())));
        DirectSound::DirectSoundEnumerate = pas::callback_from_address<DirectSound::TDirectSoundEnumerate>(WindowsImports::GetProcAddress(Module, (cpp_text_5 = static_cast<pas::AnsiString>(EC_Str::DecodeTextW(u"DrinroekcataSnowusnud.ElnourmieArtastaenAi"_w)), cpp_text_5.pchar())));
        GR_Main::CheckPlatformModules();
        ActiveXSdk::CoInitialize(nullptr);
        DirectXVersion = GR_Main::Ex_OKGF_DXVersion();
        DebugCommandMessage = WindowsSdk::RegisterWindowMessage(pas::literal_pointer("DebugMsgCommand"));
        WindowsSdk::CopyFile(pas::literal_pointer("#ship_c.dbf"), pas::literal_pointer("#ship.dbf"), 0);
        GR_Main::AppendLogLineThreadSafe("Build=2.1.2500 (11 August 2026)"_a);
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"DXVersion=", SysUtils::Int64ToStr(DirectXVersion >> 16), ".", SysUtils::Int64ToStr(DirectXVersion >> 8 & 0x000000ff), ".", SysUtils::Int64ToStr(DirectXVersion & 0x000000ff)}));
        PerformanceCounterFrequency = 0;
        WindowsSdk::QueryPerformanceFrequency(PerformanceCounterFrequency);
        WindowClass.style = 0x0000002bu;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hInstance = System::HInstance;
        WindowClass.hIcon = WindowsSdk::LoadIcon(System::HInstance, pas::literal_pointer("MAINICON"));
        WindowClass.hCursor = WindowsSdk::LoadCursor(0u, WindowsSdk::IDC_ARROW);
        WindowClass.hbrBackground = WindowsSdk::GetStockObject(WindowsSdk::BLACK_BRUSH);
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = pas::literal_pointer(u"Rangers MainClassName");
        WindowClass.lpfnWndProc = reinterpret_cast<WindowsSdk::TFNWndProc>(pas::callback_address(pas::StdcallProc<std::int32_t(std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t)>(MainWindowProc)));
        if (WindowsSdk::RegisterClassW(WindowClass) == 0) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"RegisterClass GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
        }
        if (!aPacket::InitializePackageCollection()) {
            pas::raise(pas::make_exception<pas::Exception>("Error while initializing package files"_a));
        }
        MainWindowHandle = WindowsImports::CreateWindowExW(0u, pas::literal_pointer(u"Rangers MainClassName"), pas::literal_pointer(u"Rangers"), 0u, 0, 0, 4096, 2048, 0u, 0u, System::HInstance, nullptr);
        if (MainWindowHandle == 0) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"CreateWindowEx GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
        }
        WindowsSdk::SetTimer(MainWindowHandle, 1u, 100u, nullptr);
        Forms::Application->Handle = MainWindowHandle;
        InstallConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        InstallConfig->LoadFromTextFileWithEncodingProbe(pas::literal_pointer(u"install.txt"), false);
        MessageText::QuestMessages = pas::construct_call<MessageText::TQuestMessages>(MessageText::TQuestMessages_Create);
    }

    // Falls back to Russian when the selected language is unavailable; raises on package-open failure.
    void LoadLanguageAndPackages() {
        pas::WideString cpp_text{};
        if (RequestedLanguage != u"") {
            if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", RequestedLanguage, u".txt"})))) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Not installed - ", RequestedLanguage})));
                RequestedLanguage = pas::WideString();
            } else {
                SelectedLanguage = RequestedLanguage;
            }
        }
        if (SelectedLanguage != u"") {
            if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", SelectedLanguage, u".txt"})))) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Not installed - ", SelectedLanguage, u", try to switch to russian"})));
                SelectedLanguage = u"russian"_w;
            }
        }
        if (SelectedLanguage == u"") {
            SelectedLanguage = u"russian"_w;
        }
        // Keep the else: DCC32 emits the native jump after the raise.
        if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"install_", SelectedLanguage, u".txt"})))) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Not installed language: ", SelectedLanguage}))));
        } else {
            LanguageInstallConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            {
                char16_t* cpp_arg = (cpp_text = pas::concat_wide({u"install_", SelectedLanguage, u".txt"}), cpp_text.pchar());
                EC_BlockPar::TBlockParEC* languageInstallConfig = LanguageInstallConfig;
                languageInstallConfig->LoadFromTextFileWithEncodingProbe(cpp_arg, false);
            }
        }
        ModInstallConfigs = pas::make_object<pas::List>();
        ModLanguageInstallConfigs = pas::make_object<pas::List>();
        GR_Main::LoadSelectedModInstallBlocks();
        if (!aPacket::LoadConfiguredPackages()) {
            pas::raise(pas::make_exception<pas::Exception>("Error while openning package files"_a));
        }
    }

    void LoadSelectedModInstallBlocks() {
        pas::WideString cpp_text{};
        pas::WideString cpp_text_2{};
        std::int32_t Index{};
        pas::WideString ModNames{};
        pas::WideString ModPath{};
        EC_BlockPar::TBlockParEC* Block{};
        ModNames = pas::WideString();
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(ModSelectionConfigPath))) {
            Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            Block->LoadFromTextFileWithEncodingProbe(ModSelectionConfigPath.pchar(), false);
            if (Block->CountParams(u"CurrentMod"_wref.get()) > 0) {
                ModNames = EC_Str::TrimWideString(Block->GetParam(u"CurrentMod"sv));
            }
            SelectedMods = ModNames;
            SelectedModsDisplaySuffix = pas::concat_wide({u", ", SelectedMods, u","});
            if (ModNames != u"") {
                if (SkipModsOnReload) {
                    GR_Main::AppendLogLineThreadSafe("Trying to reload without mods"_a);
                } else {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"CurrentMod=", ModNames})));
                }
            }
            pas::free(Block);
        } else {
            SelectedMods = pas::WideString();
            SelectedModsDisplaySuffix = pas::WideString();
        }
        if (!SkipModsOnReload) {
            Index = 0;
            do {
                ModPath = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(ModNames), Index, u","sv));
                if (ModPath != u"") {
                    ModPath = pas::concat_wide({ModPath, u"\\"});
                }
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"Mods\\", ModPath, u"Install.txt"})))) {
                    Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    pas::list_add(ModInstallConfigs, reinterpret_cast<void*>(Block));
                    {
                        char16_t* cpp_arg = (cpp_text = pas::concat_wide({u"Mods\\", ModPath, u"Install.txt"}), cpp_text.pchar());
                        EC_BlockPar::TBlockParEC* block = Block;
                        block->LoadFromTextFileWithEncodingProbe(cpp_arg, false);
                    }
                }
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"Mods\\", ModPath, u"Install_", SelectedLanguage, u".txt"})))) {
                    Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    pas::list_add(ModLanguageInstallConfigs, reinterpret_cast<void*>(Block));
                    {
                        char16_t* cpp_arg_2 = (cpp_text_2 = pas::concat_wide({u"Mods\\", ModPath, u"Install_", SelectedLanguage, u".txt"}), cpp_text_2.pchar());
                        EC_BlockPar::TBlockParEC* block_2 = Block;
                        block_2->LoadFromTextFileWithEncodingProbe(cpp_arg_2, false);
                    }
                }
                ++Index;
            } while (!(Index >= EC_Str::CountDelimitedPartsW(pas::view(ModNames), u","sv)));
        }
    }

    void ResetInstalledPackageState() {
        std::int32_t I{};
        aPacket::FinalizePackageCollection();
        aPacket::InitializePackageCollection();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ModInstallConfigs) - 1); cpp_range.next(I); ) {
            pas::free(pas::list_at<pas::Object>(ModInstallConfigs, I));
        }
        pas::list_clear(ModInstallConfigs);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ModLanguageInstallConfigs) - 1); cpp_range_2.next(I); ) {
            pas::free(pas::list_at<pas::Object>(ModLanguageInstallConfigs, I));
        }
        pas::list_clear(ModLanguageInstallConfigs);
    }

    void FinalizePlatformRuntime() {
        GR_Main::FreeSavePreviewBuffers();
        if (InstallConfig != nullptr) {
            pas::free(InstallConfig);
            InstallConfig = nullptr;
        }
        if (LanguageInstallConfig != nullptr) {
            pas::free(LanguageInstallConfig);
            LanguageInstallConfig = nullptr;
        }
        aPacket::FinalizePackageCollection();
        WindowsSdk::DestroyWindow(MainWindowHandle);
        MainWindowHandle = 0u;
        ActiveXSdk::CoUninitialize();
    }

    // Requires a WOW64 process and the filesystem-redirection and extended registry APIs.
    std::uint8_t HasWow64Support() {
        TGetNativeSystemInfo NativeSystemInfo{};
        TIsWow64Process IsWow64Process{};
        std::int32_t Wow64{};
        WindowsSdk::TSystemInfo Info{};
        std::uint8_t Supported = false;
        WindowsSdk::HMODULE Module = WindowsSdk::GetModuleHandle(pas::literal_pointer("kernel32.dll"));
        NativeSystemInfo = pas::callback_from_address<TGetNativeSystemInfo>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("GetNativeSystemInfo")));
        if (NativeSystemInfo != nullptr) {
            NativeSystemInfo(Info);
            IsWow64Process = pas::callback_from_address<TIsWow64Process>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("IsWow64Process")));
            if (IsWow64Process != nullptr) {
                if (IsWow64Process(WindowsSdk::GetCurrentProcess(), Wow64)) {
                    if (Wow64) {
                        if (WindowsImports::GetProcAddress(WindowsSdk::GetModuleHandle(pas::literal_pointer("kernel32.dll")), pas::literal_pointer("Wow64DisableWow64FsRedirection")) != nullptr) {
                            if (WindowsImports::GetProcAddress(Module, pas::literal_pointer("GetSystemWow64DirectoryA")) != nullptr) {
                                if (WindowsImports::GetProcAddress(WindowsSdk::GetModuleHandle(pas::literal_pointer("advapi32.dll")), pas::literal_pointer("RegDeleteKeyExA")) != nullptr) {
                                    Supported = true;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            WindowsSdk::GetSystemInfo(Info);
        }
        return Supported;
    }

    void ApplyMainWindowGeometry() {
        std::uint32_t Style{};
        std::int32_t Width{};
        std::int32_t Height{};
        WindowsSdk::TRect Bounds{};
        if (AlternateViewportEnabled) {
            Width = PresentationWidth;
            Height = PresentationHeight;
        } else {
            Width = GameScreenWidth;
            Height = GameScreenHeight;
        }
        if (Direct3DPresentParameters.Windowed) {
            if (UserSettingsConfig->CountParams(u"ShowCaption"_wref.get()) > 0 && static_cast<std::uint8_t>(GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"ShowCaption"_wref.get())))) ^ 1)) {
                Style = 0x10000000u;
            } else {
                Style = 0x10ca0000u;
            }
            if (UserSettingsConfig->CountParams(u"OverrideWindowPosition"_wref.get()) > 0) {
                Bounds.Left = EC_Str::ExtractSignedDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"OverrideWindowPosition"_wref.get()))), 0, u","sv)));
                Bounds.Top = EC_Str::ExtractSignedDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"OverrideWindowPosition"_wref.get()))), 1, u","sv)));
            } else {
                Bounds.Left = (DesktopDisplayMode.Width - static_cast<std::uint32_t>(Width)) / 2;
                Bounds.Top = (DesktopDisplayMode.Height - static_cast<std::uint32_t>(Height)) / 2;
            }
            Bounds.Right = Bounds.Left + Width;
            Bounds.Bottom = Bounds.Top + Height;
        } else {
            Style = 0x90080000u;
            Bounds.Left = 0;
            Bounds.Top = 0;
            Bounds.Right = Width;
            Bounds.Bottom = Height;
        }
        WindowsSdk::AdjustWindowRect(Bounds, Style, 0);
        WindowsSdk::SetWindowLong(MainWindowHandle, WindowsSdk::GWL_STYLE, Style);
        WindowsSdk::SetWindowPos(MainWindowHandle, WindowsSdk::HWND_NOTOPMOST, Bounds.Left, Bounds.Top, Bounds.Right - Bounds.Left, Bounds.Bottom - Bounds.Top, WindowsSdk::SWP_SHOWWINDOW);
    }

    void ShowAndFocusMainWindow() {
        if (!Direct3DPresentParameters.Windowed) {
            WindowsSdk::ShowWindow(MainWindowHandle, WindowsSdk::SW_SHOWMAXIMIZED);
        } else {
            WindowsSdk::ShowWindow(MainWindowHandle, WindowsSdk::SW_SHOWNORMAL);
        }
        WindowsSdk::UpdateWindow(MainWindowHandle);
        // Request foreground activation as well as thread-local keyboard focus.
        WindowsSdk::SetForegroundWindow(MainWindowHandle);
        WindowsSdk::SetFocus(MainWindowHandle);
        WindowsSdk::RedrawWindow(0u, nullptr, 0u, 0x00000787u);
    }

    void LoadDatConfigAndModOverrides() {
        pas::WideString ModNames{};
        pas::WideString ModPath{};
        EC_BlockPar::TBlockParEC* Block{};
        EC_Data::TDataEC* Data{};
        std::int32_t Index{};
        // An empty tree produces a log warning, not an exception from this wrapper.
        auto LoadBlockDatConfig = [&](EC_BlockPar::TBlockParEC* Root, pas::WideString FileName) -> void {
            Root->LoadFromEncryptedDatFile(FileName);
            if (Root->GetBlockCount() <= 0 && Root->GetParamCount() <= 0) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! <", FileName, u"> is empty!"})));
            }
        };
        auto LoadCacheDatConfig = [&](EC_Data::TDataEC* Root, pas::WideString FileName) -> void {
            Root->LoadFromEncryptedDatFile(FileName);
            if (Root->IsEmpty()) {
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Warning! <", FileName, u"> is empty!"})));
            }
        };
        MainDataConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        ModNames = pas::WideString();
        if (static_cast<std::uint8_t>(SkipModsOnReload ^ 1) && SysUtilsImports::FileExists(static_cast<pas::AnsiString>(ModSelectionConfigPath))) {
            Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            Block->LoadFromTextFileWithEncodingProbe(ModSelectionConfigPath.pchar(), false);
            if (Block->CountParams(u"CurrentMod"_wref.get()) > 0) {
                ModNames = EC_Str::TrimWideString(Block->GetParamByPath(u"CurrentMod"_wref.get()));
            }
            pas::free(Block);
        }
        std::uint8_t HasOverrides = false;
        LoadBlockDatConfig(MainDataConfig, u"CFG\\Main.dat"_w);
        Index = 0;
        if (!SkipModsOnReload) {
            do {
                ModPath = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(ModNames), Index, u","sv));
                if (ModPath != u"") {
                    ModPath = pas::concat_wide({ModPath, u"\\"});
                }
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"Mods\\", ModPath, u"CFG\\Main.dat"})))) {
                    HasOverrides = true;
                    Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    LoadBlockDatConfig(Block, pas::concat_wide({u"Mods\\", ModPath, u"CFG\\Main.dat"}));
                    MainDataConfig->MergeFrom(Block);
                    Block->Clear();
                    pas::free(Block);
                }
                ++Index;
            } while (!(Index >= EC_Str::CountDelimitedPartsW(pas::view(ModNames), u","sv)));
        }
        if (GlobalsV::DumpLoadedConfig) {
            MainDataConfig->SaveTextFile(pas::literal_pointer(u"Main.txt"), false, true);
        }
        LanguageDataConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        LoadBlockDatConfig(LanguageDataConfig, pas::concat_wide({u"CFG\\", LanguageInstallConfig->GetParam(u"Lang"sv), u"\\Lang.dat"}));
        Index = 0;
        if (!SkipModsOnReload) {
            do {
                ModPath = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(ModNames), Index, u","sv));
                if (ModPath != u"") {
                    ModPath = pas::concat_wide({ModPath, u"\\"});
                }
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"Mods\\", ModPath, u"CFG\\", LanguageInstallConfig->GetParam(u"Lang"sv), u"\\Lang.dat"})))) {
                    HasOverrides = true;
                    Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
                    LoadBlockDatConfig(Block, pas::concat_wide({u"Mods\\", ModPath, u"CFG\\", LanguageInstallConfig->GetParam(u"Lang"sv), u"\\Lang.dat"}));
                    LanguageDataConfig->MergeFrom(Block);
                    Block->Clear();
                    pas::free(Block);
                }
                ++Index;
            } while (!(Index >= EC_Str::CountDelimitedPartsW(pas::view(ModNames), u","sv)));
        }
        if (GlobalsV::DumpLoadedConfig) {
            LanguageDataConfig->SaveTextFile(pas::literal_pointer(u"Lang.txt"), false, true);
        }
        CacheDataRoot = pas::construct_call<EC_Data::TDataEC>(EC_Data::TDataEC_Create);
        LoadCacheDatConfig(CacheDataRoot, u"CFG\\CacheData.dat"_w);
        Index = 0;
        if (!SkipModsOnReload) {
            do {
                ModPath = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(ModNames), Index, u","sv));
                if (ModPath != u"") {
                    ModPath = pas::concat_wide({ModPath, u"\\"});
                }
                if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({u"Mods\\", ModPath, u"CFG\\CacheData.dat"})))) {
                    HasOverrides = true;
                    Data = pas::construct_call<EC_Data::TDataEC>(EC_Data::TDataEC_Create);
                    LoadCacheDatConfig(Data, pas::concat_wide({u"Mods\\", ModPath, u"CFG\\CacheData.dat"}));
                    CacheDataRoot->MergeFrom(Data);
                    pas::free(Data);
                }
                ++Index;
            } while (!(Index >= EC_Str::CountDelimitedPartsW(pas::view(ModNames), u","sv)));
        }
        if (GlobalsV::DumpLoadedConfig) {
            Block = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            CacheDataRoot->WriteToBlock(Block);
            Block->SaveTextFile(pas::literal_pointer(u"CacheData.txt"), false, true);
            pas::free(Block);
        }
        if (HasOverrides && ModNames == u"") {
            SelectedMods = u"Custom mod"_w;
            SelectedModsDisplaySuffix = pas::WideString();
            GR_Main::AppendLogLineThreadSafe("Custom mod"_a);
        }
    }

    void FreeDatConfigRoots() {
        if (CacheDataRoot != nullptr) {
            pas::free(CacheDataRoot);
            CacheDataRoot = nullptr;
        }
        if (LanguageDataConfig != nullptr) {
            pas::free(LanguageDataConfig);
            LanguageDataConfig = nullptr;
        }
        GameDataConfig = nullptr;
        if (MainDataConfig != nullptr) {
            pas::free(MainDataConfig);
            MainDataConfig = nullptr;
        }
    }

    std::uint8_t IsInstallFeatureEnabled(const pas::WideString& Path) {
        if (InstallConfig->CountParamsByPath(Path) < 1) {
            return false;
        }
        return GI_Main::ParseEnabledNameGI(pas::view(InstallConfig->GetParamByPathOrMarker(Path)));
    }

    void LoadInformationColorTags() {
        EC_BlockPar::TBlockParEC* Block{};
        aMyFunction::InfoNameColorTag = aMyFunction::DefaultInfoNameColorTag;
        aMyFunction::InfoHullSeriesColorTag = aMyFunction::DefaultInfoHullSeriesColorTag;
        if (GameDataConfig->CountBlocks(u"StyleColor"_wref.get()) > 0) {
            Block = GameDataConfig->GetBlock(u"StyleColor"sv);
            if (Block->CountParamsByPath(u"InfoNameColor"_wref.get()) > 0) {
                aMyFunction::InfoNameColorTag = pas::concat_wide({u"<color=", Block->GetParamByPath(u"InfoNameColor"_wref.get()), u">"});
            }
            if (Block->CountParamsByPath(u"InfoHullSeriesColor"_wref.get()) > 0) {
                aMyFunction::InfoHullSeriesColorTag = pas::concat_wide({u"<color=", Block->GetParamByPath(u"InfoHullSeriesColor"_wref.get()), u">"});
            }
        }
    }

    void InitializeRuntimeAndSettings() {
        pas::WideString ModuleName{};
        pas::WideString Text{};
        pas::WideString ExtraText{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t BufferSize{};
        void* Frame{};
        TCursorUnit* Cursor{};
        Registry::TRegistry* Reg{};
        TMemoryStatusEx MemoryStatus{};
        // Nested startup helper; checks the module path at parent-frame -4 and writes the signed integrity marker.
        auto VerifyStartupModuleChecksum = [&]() -> void {
            CCInterface->SetResourceChecksumFailed(false);
            EC_Data::VerifyResourceFileChecksum(ModuleName);
            std::int32_t MarkerOffset = 8;
            if (CCInterface->GetResourceChecksumFailed()) {
                pas::store_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(reinterpret_cast<std::uint8_t*>(&StartupChecksumAnchor) - MarkerOffset), aMyFunction::RandomIntRange(1000000000, 2000000000));
            } else if (pas::load_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(reinterpret_cast<std::uint8_t*>(&StartupChecksumAnchor) - MarkerOffset)) <= 0) {
                pas::store_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(reinterpret_cast<std::uint8_t*>(&StartupChecksumAnchor) - MarkerOffset), aMyFunction::RandomIntRange(-2000000000, -1000000000));
            }
            CCInterface->SetResourceChecksumFailed(false);
        };
        StartupState = 0u;
        GR_Main::FinalizeRuntimeAndSettings();
        Text = EC_Str::TrimWideString(GR_Main::ReadRegistryText(WindowsImports::HKEY_LOCAL_MACHINE, u"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"_w, u"ProductName"_w, pas::WideString()));
        if (GR_Main::HasWow64Support()) {
            ExtraText = u" [x64] build "_w;
        } else {
            ExtraText = u" [x86] build "_w;
        }
        ModuleName = EC_Str::TrimWideString(GR_Main::ReadRegistryText(WindowsImports::HKEY_LOCAL_MACHINE, u"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"_w, u"CurrentBuild"_w, pas::WideString()));
        if (EC_Str::ExtractDigitsToIntW(pas::view(ModuleName)) >= 22000) {
            Text = EC_Str::ReplaceAllWideString(Text, u"Windows 10"_wref.get(), u"Windows 11"sv);
        }
        if (GlobalsV::RunningUnderWine) {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Wine compatibility mode is set to '", Text, ExtraText, ModuleName, u"'"})));
        } else {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Operating System=", Text, ExtraText, ModuleName})));
        }
        Index = 0;
        Count = 0;
        while (true) {
            Reg = pas::construct_call<Registry::TRegistry>(Registry::TRegistry_Create);
            {
                pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
                std::exception_ptr cpp_error{};
                try {
                    Reg->SetRootKey(WindowsImports::HKEY_LOCAL_MACHINE);
                    if (Reg->KeyExists(pas::concat_ansi({"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\", SysUtils::IntToStr(Index)}))) {
                        ++Count;
                    } else {
                        cpp_flow = pas::FinallyFlow::Break;
                        goto cpp_cleanup;
                    }
                    ++Index;
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                cpp_cleanup:;
                pas::free(Reg);
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
                if (cpp_flow == pas::FinallyFlow::Break) {
                    break;
                }
            }
        }
        Index = 0;
        Text = EC_Str::TrimWideString(GR_Main::ReadRegistryText(WindowsImports::HKEY_LOCAL_MACHINE, static_cast<pas::WideString>(pas::concat_ansi({"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\", SysUtils::IntToStr(Index)})), u"Identifier"_w, pas::WideString()));
        ExtraText = EC_Str::TrimWideString(GR_Main::ReadRegistryText(WindowsImports::HKEY_LOCAL_MACHINE, static_cast<pas::WideString>(pas::concat_ansi({"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\", SysUtils::IntToStr(Index)})), u"ProcessorNameString"_w, pas::WideString()));
        if (Count <= 1) {
            ModuleName = u" (1 core)"_w;
        } else {
            ModuleName = static_cast<pas::WideString>(pas::concat_ansi({" (", SysUtils::IntToStr(Count), " cores)"}));
        }
        ProcessorCoreCount = std::max<std::int32_t>(Count, 1);
        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Processor=", ExtraText, ModuleName})));
        {
            double measureCpuClockMHz = GR_Main::MeasureCpuClockMHz();
            double measureCpuClockMHz_2 = GR_Main::MeasureCpuClockMHz();
            double real_min = pas::real_min<double>(measureCpuClockMHz, measureCpuClockMHz_2);
            double real_min_2 = pas::real_min<double>(real_min, GR_Main::MeasureCpuClockMHz());
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"CPU Clock=", SysUtils::Int64ToStr(System::Round(real_min_2)), " MHz"}));
        }
        MemoryStatus.Length = static_cast<std::int32_t>(sizeof(TMemoryStatusEx));
        GR_Main::GlobalMemoryStatusEx(MemoryStatus);
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Physical Memory Total=", SysUtils::IntToStr(MemoryStatus.TotalPhys / 0x00100000), " MB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Physical Memory Available=", SysUtils::IntToStr(MemoryStatus.AvailPhys / 0x00100000), " MB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Page File Total=", SysUtils::IntToStr(MemoryStatus.TotalPageFile / 0x00100000), " MB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Page File Available=", SysUtils::IntToStr(MemoryStatus.AvailPageFile / 0x00100000), " MB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Virtual Memory Total=", SysUtils::IntToStr(MemoryStatus.TotalVirtual / 0x00100000), " MB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Virtual Memory Available=", SysUtils::IntToStr(MemoryStatus.AvailVirtual / 0x00100000), " MB"}));
        UserSettingsConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        Text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"CFG.TXT"});
        if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(Text))) {
            GR_Main::AppendLogTextThreadSafe("Creating cfg.txt ... "_a);
            WindowsSdk::CopyFileW(pas::literal_pointer(u"cfg.txt"), Text.pchar(), 0);
            UserSettingsConfig->LoadFromTextFileWithEncodingProbe(Text.pchar(), true);
            UserSettingsConfig->AddParam(u"CurrentVersion"_wref.get(), GameVersionText);
            UserSettingsConfig->AddParam(u"VideoMemSizeLimit"_wref.get(), u"256"_wref.get());
            if (GlobalsV::RunningUnderWine) {
                UserSettingsConfig->AddParam(u"RunOnWineWithoutWarning"_wref.get(), u"True"_wref.get());
                GlobalsV::ShowWineWarning = true;
            }
            UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
            GR_Main::AppendLogLineThreadSafe("ok!"_a);
        } else {
            UserSettingsConfig->LoadFromTextFileWithEncodingProbe(Text.pchar(), true);
            if (UserSettingsConfig->CountParamsByPath(u"CurrentVersion"_wref.get()) == 0) {
                GR_Main::AppendLogTextThreadSafe("Updating cfg.txt content ... "_a);
                UserSettingsConfig->AddParam(u"CurrentVersion"_wref.get(), GameVersionText);
                UserSettingsConfig->SetOrAddParam(u"HardwareRender"_wref.get(), u"True"_wref.get());
                UserSettingsConfig->SetOrAddParam(u"MultiThread"_wref.get(), u"False"_wref.get());
                UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
                GR_Main::AppendLogLineThreadSafe("ok!"_a);
            } else if (([&] {
                pas::WideString cpp_string = UserSettingsConfig->GetParamByPathOrMarker(u"CurrentVersion"_wref.get());
                return cpp_string != GameVersionText;
            }())) {
                GR_Main::AppendLogTextThreadSafe("Updating cfg.txt version ... "_a);
                if (UserSettingsConfig->GetParam(u"CurrentVersion"sv) == u"2.1.1800" && UserSettingsConfig->CountParamsByPath(u"CountFilmSave"_wref.get()) > 0 && UserSettingsConfig->GetParamByPathOrMarker(u"CountFilmSave"_wref.get()) == u"30") {
                    UserSettingsConfig->SetOrAddParam(u"CountFilmSave"_wref.get(), u"7"_wref.get());
                }
                UserSettingsConfig->SetOrAddParam(u"CurrentVersion"_wref.get(), GameVersionText);
                UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
                GR_Main::AppendLogLineThreadSafe("ok!"_a);
            }
            if (UserSettingsConfig->CountParamsByPath(u"VideoMemSizeLimit"_wref.get()) == 0) {
                GR_Main::AppendLogTextThreadSafe("Updating cfg.txt content ... "_a);
                UserSettingsConfig->AddParam(u"VideoMemSizeLimit"_wref.get(), u"256"_wref.get());
                UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
                GR_Main::AppendLogLineThreadSafe("ok!"_a);
            }
            if (GlobalsV::RunningUnderWine) {
                if (UserSettingsConfig->CountParams(u"RunOnWineWithoutWarning"_wref.get()) == 0 || static_cast<std::uint8_t>(GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"RunOnWineWithoutWarning"_wref.get())))) ^ 1)) {
                    if (UserSettingsConfig->CountParams(u"RunOnWineWithoutWarning"_wref.get()) == 0) {
                        UserSettingsConfig->AddParam(u"RunOnWineWithoutWarning"_wref.get(), u"True"_wref.get());
                    } else {
                        UserSettingsConfig->SetOrAddParam(u"RunOnWineWithoutWarning"_wref.get(), u"True"_wref.get());
                    }
                    UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
                    GlobalsV::ShowWineWarning = true;
                }
            }
        }
        EditableSaveBlock = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        NewGameSettingsConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        Text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"newgame.txt"});
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(Text.pchar()))) {
            NewGameSettingsConfig->LoadFromTextFileWithEncodingProbe(Text.pchar(), true);
        }
        if (UserSettingsConfig->CountParamsByPath(u"MultiThread"_wref.get()) > 0) {
            GlobalsV::MultiThreadEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"MultiThread"_wref.get())));
        }
        GR_Main::ApplyProcessAffinity();
        PathGrowEnabled = true;
        if (UserSettingsConfig->CountParams(u"PathGrow"_wref.get()) > 0) {
            PathGrowEnabled = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"PathGrow"_wref.get()))));
        }
        ShowSystemMouse = false;
        if (UserSettingsConfig->CountParams(u"ShowSystemMouse"_wref.get()) > 0) {
            ShowSystemMouse = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"ShowSystemMouse"_wref.get()))));
        }
        if (SysUtilsImports::FileExists("Mods\\ShipName.txt"_a)) {
            ModShipNameConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            ModShipNameConfig->LoadFromTextFileWithEncodingProbe(pas::literal_pointer(u"Mods\\ShipName.txt"), false);
        }
        if (SysUtilsImports::FileExists("Mods\\RuinName.txt"_a)) {
            ModRuinNameConfig = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
            ModRuinNameConfig->LoadFromTextFileWithEncodingProbe(pas::literal_pointer(u"Mods\\RuinName.txt"), false);
        }
        if (SysUtilsImports::FileExists("MusicChange.txt"_a)) {
            MainDataConfig->GetBlock(u"Music"sv)->Clear();
            MainDataConfig->GetBlock(u"Music"sv)->LoadFromTextFileWithEncodingProbe(pas::literal_pointer(u"MusicChange.txt"), false);
        }
        {
            EC_BlockPar::TBlockParEC* blockByPath = LanguageDataConfig->GetBlockByPath(u"PlanetQuest"_wref.get());
            EC_Data::TDataEC* cacheDataRoot = CacheDataRoot;
            cacheDataRoot->AddMissingFromBlock(blockByPath);
        }
        GlobalCache = pas::construct_call<EC_Cache::TCacheEC>(EC_Cache::TCacheEC_Create);
        GlobalCache->SetDataRoot(CacheDataRoot);
        GlobalCache->ResidentByteLimit = 0;
        if (UserSettingsConfig->CountParams(u"CacheSize"_wref.get()) > 0) {
            GlobalCache->ResidentByteLimit = pas::shl(pas::shl(EC_Str::ExtractDigitsToIntW(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"CacheSize"_wref.get()))), 10), 10);
        }
        if (GlobalCache->ResidentByteLimit <= 0x01000000) {
            if (MemoryStatus.AvailVirtual > 0x48000000) {
                GlobalCache->ResidentByteLimit = 0x18000000;
            } else {
                GlobalCache->ResidentByteLimit = std::min<std::int64_t>(static_cast<std::int64_t>(static_cast<std::uint32_t>(MemoryStatus.AvailVirtual) / 3), static_cast<std::int64_t>(0x18000000));
            }
        }
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Cache Size=", SysUtils::IntToStr(GlobalCache->ResidentByteLimit / 1024), " KB"}));
        if (UserSettingsConfig->CountParams(u"Brightness"_wref.get()) > 0) {
            DisplayBrightness = EC_Str::ParseDecimalToSingleW(UserSettingsConfig->GetParamByPathOrMarker(u"Brightness"_wref.get()));
        }
        if (UserSettingsConfig->CountParams(u"Contrast"_wref.get()) > 0) {
            DisplayContrast = EC_Str::ParseDecimalToSingleW(UserSettingsConfig->GetParamByPathOrMarker(u"Contrast"_wref.get()));
        }
        if (UserSettingsConfig->CountParams(u"RobotBrightness"_wref.get()) > 0) {
            RobotBrightness = EC_Str::ParseDecimalToSingleW(UserSettingsConfig->GetParamByPathOrMarker(u"RobotBrightness"_wref.get()));
        }
        if (UserSettingsConfig->CountParams(u"RobotContrast"_wref.get()) > 0) {
            RobotContrast = EC_Str::ParseDecimalToSingleW(UserSettingsConfig->GetParamByPathOrMarker(u"RobotContrast"_wref.get()));
        }
        if (UserSettingsConfig->CountParams(u"3D"_wref.get()) > 0) {
            GlobalsV::ThreeDimensionalModeEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"3D"_wref.get())));
        }
        UiStyleConfig = MainDataConfig->GetBlockByPath(u"ML"_wref.get());
        GameDataConfig = MainDataConfig->GetBlockByPath(u"Data"_wref.get());
        GR_Main::LoadInformationColorTags();
        UiDepthConfig = MainDataConfig->GetBlockByPath(u"ZPos"_wref.get());
        GlobalsV::PlanetDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"Planet"sv));
        GlobalsV::ShipPathDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"UnitPathShip"sv));
        GlobalsV::ShipPathEndDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"UnitPathEndShip"sv));
        GlobalsV::UnitPathDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"UnitPath"sv));
        GlobalsV::UnitPathEndDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"UnitPathEnd"sv));
        GlobalsV::ActionButtonDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"ButtonAction"sv));
        GlobalsV::GalaxyStarDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"GalaxyStar"sv));
        GlobalsV::GalaxyStarNameDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"GalaxyStarName"sv));
        GlobalsV::GalaxyWarDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"GalaxyWar"sv));
        GlobalsV::ConstellationLineDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"ConstellationLine"sv));
        GlobalsV::ConstellationColorDepth = EC_Str::ExtractDecimalToSingleW(UiDepthConfig->GetParam(u"ConstellationColor"sv));
        if (UserSettingsConfig->CountParamsByPath(u"Sound"_wref.get()) > 0) {
            GlobalsV::SoundEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"Sound"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"SoundInSpace"_wref.get()) > 0) {
            GlobalsV::SoundInSpaceEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"SoundInSpace"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"SoundVolume"_wref.get()) > 0) {
            GlobalsV::SoundVolume = pas::real_divide(SysUtils::StrToInt(static_cast<pas::AnsiString>(UserSettingsConfig->GetParamByPathOrMarker(u"SoundVolume"_wref.get()))), 1.0E+2L);
        }
        if (GlobalsV::SoundVolume > 1.0L) {
            GlobalsV::SoundVolume = 1.0f;
        }
        if (GlobalsV::SoundVolume < 0.0L) {
            GlobalsV::SoundVolume = 0.0f;
        }
        if (UserSettingsConfig->CountParamsByPath(u"RobotSoundVolume"_wref.get()) > 0) {
            GlobalsV::RobotSoundVolume = pas::real_divide(SysUtils::StrToInt(static_cast<pas::AnsiString>(UserSettingsConfig->GetParamByPathOrMarker(u"RobotSoundVolume"_wref.get()))), 1.0E+2L);
        }
        if (GlobalsV::RobotSoundVolume > 1.0L) {
            GlobalsV::RobotSoundVolume = 1.0f;
        }
        if (GlobalsV::RobotSoundVolume < 0.0L) {
            GlobalsV::RobotSoundVolume = 0.0f;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"Sound"_wref.get())) {
            GlobalsV::SoundEnabled = false;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"SoundInSpace"_wref.get())) {
            GlobalsV::SoundInSpaceEnabled = false;
        }
        if (UserSettingsConfig->CountParamsByPath(u"Music"_wref.get()) > 0) {
            GlobalsV::MusicEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"Music"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"MusicInSpace"_wref.get()) > 0) {
            GlobalsV::MusicInSpaceEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"MusicInSpace"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"MusicInHyper"_wref.get()) > 0) {
            GlobalsV::MusicInHyperEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"MusicInHyper"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"MusicInPlanet"_wref.get()) > 0) {
            GlobalsV::MusicInPlanetEnabled = GI_Main::ParseEnabledNameGI(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"MusicInPlanet"_wref.get())));
        }
        if (UserSettingsConfig->CountParamsByPath(u"MusicVolume"_wref.get()) > 0) {
            GlobalsV::MusicVolume = pas::real_divide(SysUtils::StrToInt(static_cast<pas::AnsiString>(UserSettingsConfig->GetParamByPathOrMarker(u"MusicVolume"_wref.get()))), 1.0E+2L);
        }
        if (GlobalsV::MusicVolume > 1.0L) {
            GlobalsV::MusicVolume = 1.0f;
        }
        if (GlobalsV::MusicVolume < 0.0L) {
            GlobalsV::MusicVolume = 0.0f;
        }
        if (UserSettingsConfig->CountParamsByPath(u"RobotMusicVolume"_wref.get()) > 0) {
            GlobalsV::RobotMusicVolume = pas::real_divide(SysUtils::StrToInt(static_cast<pas::AnsiString>(UserSettingsConfig->GetParamByPathOrMarker(u"RobotMusicVolume"_wref.get()))), 1.0E+2L);
        }
        if (GlobalsV::RobotMusicVolume > 1.0L) {
            GlobalsV::RobotMusicVolume = 1.0f;
        }
        if (GlobalsV::RobotMusicVolume < 0.0L) {
            GlobalsV::RobotMusicVolume = 0.0f;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"Music"_wref.get())) {
            GlobalsV::MusicEnabled = false;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"MusicInSpace"_wref.get())) {
            GlobalsV::MusicInSpaceEnabled = false;
        }
        EC_BlockPar::TBlockParEC* Block = LanguageDataConfig->GetBlock(u"CaseConv"sv);
        Count = Block->GetParamCount();
        WideCaseTable.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            WideCaseTable[Index].LowerChar = Block->GetParamName(Index).read(1);
            WideCaseTable[Index].UpperChar = Block->GetParamValue(Index).read(1);
        }
        GR_Main::AppendLogLineThreadSafe("Loading configuration files.... ok!"_a);
        GR_Main::AppendLogLineThreadSafe("Creating window.... ok!"_a);
        if (GlobalsV::SoundEnabled) {
            GR_Main::AppendLogLineThreadSafe("Sound interfaces are:"_a);
        } else {
            GR_Main::AppendLogLineThreadSafe("Sound is disabled..."_a);
        }
        SoundManager = pas::construct_call<GR_Sound::TSoundControl>(GR_Sound::TSoundControl_Create);
        MusicManager = pas::construct_call<GR_Music::TMusicControl>(GR_Music::TMusicControl_Create);
        if (GlobalsV::XonarSoundDevice) {
            if (UserSettingsConfig->CountParams(u"RunWithXonarWithoutWarning"_wref.get()) == 0 || static_cast<std::uint8_t>(GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"RunWithXonarWithoutWarning"_wref.get())))) ^ 1)) {
                if (UserSettingsConfig->CountParams(u"RunWithXonarWithoutWarning"_wref.get()) == 0) {
                    UserSettingsConfig->AddParam(u"RunWithXonarWithoutWarning"_wref.get(), u"True"_wref.get());
                } else {
                    UserSettingsConfig->SetOrAddParam(u"RunWithXonarWithoutWarning"_wref.get(), u"True"_wref.get());
                }
                Text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"CFG.TXT"});
                UserSettingsConfig->SaveTextFile(Text.pchar(), true, false);
                GlobalsV::ShowXonarWarning = true;
            }
        }
        GR_Main::GR_DXInit();
        if (GlobalsV::HardwareRenderingEnabled) {
            GR_Main::AppendLogLineThreadSafe("Renderer=hardware"_a);
        } else {
            GR_Main::AppendLogLineThreadSafe("Renderer=software"_a);
        }
        ScreenCenterX = static_cast<std::uint32_t>(GameScreenWidth) >> 1;
        ScreenCenterY = static_cast<std::uint32_t>(GameScreenHeight) >> 1;
        GR_Main::ShowAndFocusMainWindow();
        if (!ShowSystemMouse) {
            WindowsSdk::ShowCursor(0);
        }
        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Sound=", EC_Str::BoolToWideString(GlobalsV::SoundEnabled)})));
        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Music=", EC_Str::BoolToWideString(GlobalsV::MusicEnabled)})));
        InterfaceBlendPalette = EC_Mem::AllocEC(512);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 255); cpp_range_2.next(Index); ) {
            EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(InterfaceBlendPalette, 2 * Index), CurrentPixelFormat->PackRgbBytes(8, 32, 255));
            GR_Main::BlendPixel16(EC_Mem::AddPointerOffset(InterfaceBlendPalette, 2 * Index), CurrentPixelFormat->PackRgbBytes(200, 128, 128), Index);
        }
        if (RecordingFrameBuffers != nullptr) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(RecordingFrameBuffers) - 1); cpp_range_3.next(Index); ) {
                EC_Mem::FreeEC(pas::list_get(RecordingFrameBuffers, Index));
            }
            pas::list_clear(RecordingFrameBuffers);
            pas::free(RecordingFrameBuffers);
            RecordingFrameBuffers = nullptr;
        }
        if (UserSettingsConfig->CountParamsByPath(u"FilmBufSize"_wref.get()) > 0) {
            BufferSize = EC_Str::ExtractDigitsToIntW(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"FilmBufSize"_wref.get())));
            if (BufferSize > 0) {
                RecordingFrameBuffers = pas::make_object<pas::List>();
                Count = pas::idiv(pas::shl(pas::shl(BufferSize, 10), 10), GameScreenWidth * GameScreenHeight * 2) + 1;
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"FilmFrame=", SysUtils::IntToStr(Count)}));
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(Index); ) {
                    Frame = EC_Mem::AllocEC(GameScreenWidth * GameScreenHeight * 2);
                    pas::list_add(RecordingFrameBuffers, Frame);
                }
            }
        }
        if (UserSettingsConfig->CountParamsByPath(u"FilmFPS"_wref.get()) > 0) {
            RecordingFrameInterval = pas::idiv(1000, EC_Str::ExtractDigitsToIntW(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"FilmFPS"_wref.get()))));
        }
        Block = MainDataConfig->GetBlockByPath(u"Graph.Cursor"_wref.get());
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Block->GetBlockCount() - 1); cpp_range_5.next(Index); ) {
            Cursor = GR_Main::AddCursorUnit();
            Cursor->Name = Block->GetBlockNameByIndex(Index);
            Cursor->ImagePath = Block->GetBlockByIndex(Index)->GetParam(u"Image"sv);
            Cursor->HotSpot = GI_Main::GetPointGI(pas::view(Block->GetBlockByIndex(Index)->GetParam(u"Sme"sv)));
        }
        if (([&] {
            pas::WideString cpp_string_2 = LanguageDataConfig->GetParamByPathOrMarker(u"BV.BV"_wref.get());
            return cpp_string_2 != GameVersionText;
        }())) {
            GR_Main::AppendLogLineThreadSafe("Build version mismatch with Lang.dat!"_a);
            BuildVersionMismatch = true;
        }
        if (([&] {
            pas::WideString cpp_string_3 = MainDataConfig->GetParamByPathOrMarker(u"BV.BV"_wref.get());
            return cpp_string_3 != GameVersionText;
        }())) {
            GR_Main::AppendLogLineThreadSafe("Build version mismatch with Main.dat!"_a);
            BuildVersionMismatch = true;
        }
        if (([&] {
            const pas::WideString& cpp_string_ref = CacheDataRoot->FindEntry(u"BV"_wref.get())->ChildData->FindEntry(u"BV"_wref.get())->SharedFileRef->FileRef->FileName;
            return cpp_string_ref != GameVersionText;
        }())) {
            GR_Main::AppendLogLineThreadSafe("Build version mismatch with CacheData.dat!"_a);
            BuildVersionMismatch = true;
        }
        std::uint8_t SavedChecksumFailed = CCInterface->GetResourceChecksumFailed();
        Text = u"ll"_w;
        Text = pas::concat_wide({u".d", Text});
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aucah"_w), Text});
        if (WindowsSdk::GetModuleHandleW(ModuleName.pchar()) != 0) {
            VerifyStartupModuleChecksum();
        }
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"sotoenalm^_^aupki"_w), Text});
        if (WindowsSdk::GetModuleHandleW(ModuleName.pchar()) != 0) {
            VerifyStartupModuleChecksum();
        }
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"zoloimba"_w), Text});
        VerifyStartupModuleChecksum();
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"MhastorhinxaGrakmae"_w), Text});
        VerifyStartupModuleChecksum();
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"ookogifa"_w), Text});
        VerifyStartupModuleChecksum();
        ModuleName = pas::concat_wide({EC_Str::DecodeTextW(u"xavriadeccomrie"_w), Text});
        VerifyStartupModuleChecksum();
        ExtraText = u"ib"_w;
        ExtraText = pas::concat_wide({u"l", ExtraText});
        ModuleName = pas::concat_wide({ExtraText, EC_Str::DecodeTextW(u"osgaga-10a"_w), Text});
        VerifyStartupModuleChecksum();
        ModuleName = pas::concat_wide({ExtraText, EC_Str::DecodeTextW(u"vrokrablius-->0"_w), Text});
        VerifyStartupModuleChecksum();
        ModuleName = pas::concat_wide({ExtraText, EC_Str::DecodeTextW(u"veohrablissufainlae"_w), Text});
        VerifyStartupModuleChecksum();
        CCInterface->SetResourceChecksumFailed(SavedChecksumFailed);
    }

    void FinalizeRuntimeAndSettings() {
        while (!(FirstRegisteredCursor == nullptr)) {
            GR_Main::RemoveCursorUnit(LastRegisteredCursor);
        }
        if (InterfaceBlendPalette != nullptr) {
            EC_Mem::FreeEC(InterfaceBlendPalette);
            InterfaceBlendPalette = nullptr;
        }
        if (SoundManager != nullptr) {
            SoundManager->SignalStop();
        }
        if (MusicManager != nullptr) {
            pas::free(MusicManager);
            MusicManager = nullptr;
        }
        if (SoundManager != nullptr) {
            pas::free(SoundManager);
            SoundManager = nullptr;
        }
        if (GlobalCache != nullptr) {
            pas::free(GlobalCache);
            GlobalCache = nullptr;
        }
        if (ModShipNameConfig != nullptr) {
            pas::free(ModShipNameConfig);
            ModShipNameConfig = nullptr;
        }
        if (ModRuinNameConfig != nullptr) {
            pas::free(ModRuinNameConfig);
            ModRuinNameConfig = nullptr;
        }
        if (UserSettingsConfig != nullptr) {
            pas::free(UserSettingsConfig);
            UserSettingsConfig = nullptr;
        }
        GR_Main::FreeScreenRenderBuffers();
        WideCaseTable = nullptr;
    }

    // Keeps the highest refresh rate for each size. A zero-width entry means automatic resolution; a custom size may be appended.
    void EnumerateAndSelectDisplayModes() {
        std::int32_t Index{};
        pas::WideString Resolution{};
        pas::WideString ModeKey{};
        EC_BlockPar::TBlockParEC* Modes{};
        TDisplayModeGR Mode{};
        if (GameDisplayModeCount == 0) {
            Direct3D9::IDirect3D9_GetAdapterDisplayMode(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, &DesktopDisplayMode);
        }
        pas::fill_memory(&Mode, static_cast<std::int32_t>(sizeof(TDisplayModeGR)), static_cast<std::uint8_t>(0));
        GameDisplayModeCount = 0;
        SelectedGameDisplayMode = -1;
        SmallestGameDisplayMode = -1;
        std::int32_t SmallestArea = -1;
        RobotDisplayModeCount = 0;
        SelectedRobotDisplayMode = -1;
        Resolution = pas::WideString();
        if (UserSettingsConfig->CountParams(u"VideoMode"_wref.get()) > 0) {
            Resolution = EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"VideoMode"_wref.get()));
        }
        if (EC_Str::CountDelimitedPartsW(pas::view(Resolution), u","sv) > 1) {
            GameScreenWidth = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Resolution), 0, u","sv)));
            GameScreenHeight = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Resolution), 1, u","sv)));
            RequestedRefreshRate = 0;
            if (EC_Str::CountDelimitedPartsW(pas::view(Resolution), u","sv) > 2) {
                RequestedRefreshRate = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Resolution), 2, u","sv)));
            }
        } else {
            GameScreenWidth = 0;
            GameScreenHeight = 0;
            RequestedRefreshRate = 0;
        }
        // Native keeps the VideoMode text when RobotResolution is absent.
        if (UserSettingsConfig->CountParams(u"RobotResolution"_wref.get()) > 0) {
            Resolution = EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"RobotResolution"_wref.get()));
        }
        if (EC_Str::CountDelimitedPartsW(pas::view(Resolution), u","sv) > 1) {
            Robot::RobotSettings.ScreenWidth = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Resolution), 0, u","sv)));
            Robot::RobotSettings.ScreenHeight = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Resolution), 1, u","sv)));
        } else {
            Robot::RobotSettings.ScreenWidth = 0;
            Robot::RobotSettings.ScreenHeight = 0;
        }
        UseDesktopDisplayMode = false;
        if (static_cast<std::uint32_t>(GameScreenWidth) < 1024 || static_cast<std::uint32_t>(GameScreenHeight) < 720) {
            GameScreenWidth = DesktopDisplayMode.Width;
            GameScreenHeight = DesktopDisplayMode.Height;
            UseDesktopDisplayMode = true;
        }
        UseAutomaticRobotDisplayMode = false;
        if (Robot::RobotSettings.ScreenWidth < 1024 || Robot::RobotSettings.ScreenHeight < 720) {
            UseAutomaticRobotDisplayMode = true;
        }
        std::int32_t ModeCount = Direct3D9::IDirect3D9_GetAdapterModeCount(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, DesktopDisplayMode.Format);
        if (DesktopDisplayMode.Format != Direct3D9::D3DFMT_X8R8G8B8 && DesktopDisplayMode.Format != Direct3D9::D3DFMT_A8R8G8B8) {
            DesktopDisplayMode.Format = Direct3D9::D3DFMT_X8R8G8B8;
        }
        Modes = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ModeCount - 1); cpp_range.next(Index); ) {
            Direct3D9::IDirect3D9_EnumAdapterModes(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, DesktopDisplayMode.Format, Index, &Mode);
            if (Mode.Height >= 720) {
                ModeKey = static_cast<pas::WideString>(pas::concat_ansi_reverse({pas::int_to_hex(static_cast<std::int64_t>(Mode.Height), 6), pas::int_to_hex(static_cast<std::int64_t>(Mode.Width), 6)}));
                if (Modes->CountParams(ModeKey) <= 0 || SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Modes->GetParam(pas::view(ModeKey))), 0, u","sv))) < static_cast<std::int32_t>(Mode.RefreshRate)) {
                    Modes->SetOrAddParam(ModeKey, static_cast<pas::WideString>(pas::concat_ansi({SysUtils::Int64ToStr(Mode.RefreshRate), ",", SysUtils::IntToStr(Index)})));
                }
            }
        }
        GameDisplayModeCount = Modes->GetParamCount();
        RobotDisplayModeCount = Modes->GetParamCount();
        GameDisplayModes.set_length(GameDisplayModeCount + 2);
        RobotDisplayModes.set_length(RobotDisplayModeCount + 2);
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Desktop Resolution=", SysUtils::Int64ToStr(DesktopDisplayMode.Width), "x", SysUtils::Int64ToStr(DesktopDisplayMode.Height)}));
        GR_Main::AppendLogTextThreadSafe("Available Resolutions="_a);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Modes->GetParamCount() - 1); cpp_range_2.next(Index); ) {
            ModeKey = Modes->GetParamValue(Index);
            Direct3D9::IDirect3D9_EnumAdapterModes(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, DesktopDisplayMode.Format, SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(ModeKey), 1, u","sv))), &Mode);
            GameDisplayModes[Index] = Mode;
            RobotDisplayModes[Index] = Mode;
            if (SmallestArea < 0 || static_cast<std::int32_t>(Mode.Width * Mode.Height) < SmallestArea) {
                SmallestArea = Mode.Width * Mode.Height;
                SmallestGameDisplayMode = Index;
            }
            if (Mode.Width == static_cast<std::uint32_t>(GameScreenWidth) && Mode.Height == static_cast<std::uint32_t>(GameScreenHeight)) {
                SelectedGameDisplayMode = Index;
            }
            if (static_cast<std::uint32_t>(Robot::RobotSettings.ScreenWidth) == Mode.Width && static_cast<std::uint32_t>(Robot::RobotSettings.ScreenHeight) == Mode.Height) {
                SelectedRobotDisplayMode = Index;
            }
            if (Index > 0) {
                GR_Main::AppendLogTextThreadSafe(", "_a);
            }
            GR_Main::AppendLogTextThreadSafe(pas::concat_ansi({SysUtils::Int64ToStr(Mode.Width), "x", SysUtils::Int64ToStr(Mode.Height)}));
        }
        pas::free(Modes);
        if (GameDisplayModeCount == 0) {
            GR_Main::AppendLogTextThreadSafe("No supported resolutions found!"_a);
            if (static_cast<std::uint32_t>(GameScreenWidth) < 1024 || static_cast<std::uint32_t>(GameScreenHeight) < 720) {
                AlternateViewportEnabled = true;
                ViewportOffset = ClassesImports::Point(0, 0);
                PresentationWidth = GameScreenWidth;
                PresentationHeight = GameScreenHeight;
                Robot::RobotSettings.ScreenWidth = GameScreenWidth;
                Robot::RobotSettings.ScreenHeight = GameScreenHeight;
                if (GlobalsV::ScaleViewportToWindow) {
                    if (static_cast<std::uint32_t>(GameScreenWidth) > static_cast<std::uint32_t>(GameScreenHeight)) {
                        GameScreenHeight = 720;
                        GameScreenWidth = pas::idiv(static_cast<std::uint32_t>(GameScreenHeight * PresentationWidth), static_cast<std::uint32_t>(PresentationHeight));
                    } else {
                        GameScreenWidth = 1024;
                        GameScreenHeight = pas::idiv(static_cast<std::uint32_t>(GameScreenWidth * PresentationHeight), static_cast<std::uint32_t>(PresentationWidth));
                    }
                } else {
                    GameScreenWidth = std::max<std::int64_t>(static_cast<std::int64_t>(1024), static_cast<std::int64_t>(static_cast<std::uint32_t>(GameScreenWidth)));
                    GameScreenHeight = std::max<std::int64_t>(static_cast<std::int64_t>(720), static_cast<std::int64_t>(static_cast<std::uint32_t>(GameScreenHeight)));
                }
                GlobalsV::HardwareRenderingEnabled = false;
            }
        }
        GameDisplayModes[GameDisplayModeCount].Width = 0u;
        GameDisplayModes[GameDisplayModeCount].Height = 0u;
        GameDisplayModes[GameDisplayModeCount].RefreshRate = DesktopDisplayMode.RefreshRate;
        RobotDisplayModes[RobotDisplayModeCount].Width = 0u;
        RobotDisplayModes[RobotDisplayModeCount].Height = 0u;
        if (UseDesktopDisplayMode) {
            SelectedGameDisplayMode = GameDisplayModeCount;
        }
        if (UseAutomaticRobotDisplayMode) {
            SelectedRobotDisplayMode = RobotDisplayModeCount;
        }
        ++GameDisplayModeCount;
        ++RobotDisplayModeCount;
        if (SelectedGameDisplayMode == -1) {
            SelectedGameDisplayMode = GameDisplayModeCount;
            GameDisplayModes[SelectedGameDisplayMode].Width = GameScreenWidth;
            GameDisplayModes[SelectedGameDisplayMode].Height = GameScreenHeight;
            ++GameDisplayModeCount;
        }
        if (SelectedRobotDisplayMode == -1) {
            SelectedRobotDisplayMode = RobotDisplayModeCount;
            RobotDisplayModes[SelectedRobotDisplayMode].Width = Robot::RobotSettings.ScreenWidth;
            RobotDisplayModes[SelectedRobotDisplayMode].Height = Robot::RobotSettings.ScreenHeight;
            ++RobotDisplayModeCount;
        }
        GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
        ExtraScreenWidth = GameScreenWidth - 1024;
        ExtraScreenHeight = GameScreenHeight - 768;
        GameScreenRect = ClassesImports::Rect(0, 0, GameScreenWidth, GameScreenHeight);
        PresentationRect = ClassesImports::Rect(0, 0, PresentationWidth, PresentationHeight);
    }

    void ConfigureDefaultRenderState() {
        Direct3D9::IDirect3DDevice9_SetRenderState(Direct3DDevice, Direct3D9::D3DRS_ALPHABLENDENABLE, 1u);
        Direct3D9::IDirect3DDevice9_SetRenderState(Direct3DDevice, Direct3D9::D3DRS_SRCBLEND, Direct3D9::D3DBLEND_SRCALPHA);
        Direct3D9::IDirect3DDevice9_SetRenderState(Direct3DDevice, Direct3D9::D3DRS_DESTBLEND, Direct3D9::D3DBLEND_INVSRCALPHA);
        Direct3D9::IDirect3DDevice9_SetTextureStageState(Direct3DDevice, 0u, Direct3D9::D3DTSS_ALPHAOP, Direct3D9::D3DTOP_MODULATE);
        Direct3D9::IDirect3DDevice9_SetSamplerState(Direct3DDevice, 0u, Direct3D9::D3DSAMP_MIPFILTER, Direct3D9::D3DTEXF_LINEAR);
        Direct3D9::IDirect3DDevice9_SetSamplerState(Direct3DDevice, 0u, Direct3D9::D3DSAMP_MINFILTER, Direct3D9::D3DTEXF_LINEAR);
        Direct3D9::IDirect3DDevice9_SetSamplerState(Direct3DDevice, 0u, Direct3D9::D3DSAMP_MAGFILTER, Direct3D9::D3DTEXF_LINEAR);
        Direct3D9::IDirect3DDevice9_SetRenderState(Direct3DDevice, Direct3D9::D3DRS_SCISSORTESTENABLE, 1u);
        Direct3D9::IDirect3DDevice9_SetRenderState(Direct3DDevice, Direct3D9::D3DRS_CULLMODE, Direct3D9::D3DCULL_NONE);
    }

    void PreparePresentationParameters() {
        PreviousPresentParameters = Direct3DPresentParameters;
        WindowsImports::ZeroMemory(&Direct3DPresentParameters, static_cast<std::int32_t>(sizeof(Direct3D9::TD3DPresentParameters)));
        Direct3DPresentParameters.Windowed = -(WindowedModeRequested && static_cast<std::uint32_t>(GameScreenHeight) < DesktopDisplayMode.Height);
        Direct3DPresentParameters.DeviceWindow = MainWindowHandle;
        if (DisableTripleBuffer) {
            Direct3DPresentParameters.BackBufferCount = 1u;
        } else {
            Direct3DPresentParameters.BackBufferCount = 2u;
        }
        Direct3DPresentParameters.BackBufferFormat = DesktopDisplayMode.Format;
        Direct3DPresentParameters.PresentationInterval = Direct3D9::D3DPRESENT_INTERVAL_IMMEDIATE;
        if (Direct3DPresentParameters.Windowed) {
            if (VSyncEnabled) {
                Direct3DPresentParameters.PresentationInterval = Direct3D9::D3DPRESENT_INTERVAL_DEFAULT;
            }
            Direct3DPresentParameters.SwapEffect = Direct3D9::D3DSWAPEFFECT_DISCARD;
        } else {
            if (VSyncEnabled) {
                Direct3DPresentParameters.PresentationInterval = Direct3D9::D3DPRESENT_INTERVAL_ONE;
            }
            Direct3DPresentParameters.SwapEffect = Direct3D9::D3DSWAPEFFECT_FLIP;
            if (UseDesktopDisplayMode) {
                Direct3DPresentParameters.BackBufferWidth = DesktopDisplayMode.Width;
                Direct3DPresentParameters.BackBufferHeight = DesktopDisplayMode.Height;
                Direct3DPresentParameters.FullScreenRefreshRateInHz = DesktopDisplayMode.RefreshRate;
            } else {
                Direct3DPresentParameters.BackBufferWidth = GameDisplayModes[SelectedGameDisplayMode].Width;
                Direct3DPresentParameters.BackBufferHeight = GameDisplayModes[SelectedGameDisplayMode].Height;
                if (RequestedRefreshRate > 0) {
                    Direct3DPresentParameters.FullScreenRefreshRateInHz = RequestedRefreshRate;
                } else {
                    Direct3DPresentParameters.FullScreenRefreshRateInHz = GameDisplayModes[SelectedGameDisplayMode].RefreshRate;
                }
            }
        }
        PresentationFrameRate = Direct3DPresentParameters.FullScreenRefreshRateInHz;
        if (PresentationFrameRate == 0) {
            PresentationFrameRate = DesktopDisplayMode.RefreshRate;
        }
        if (PresentationFrameRate == 0) {
            PresentationFrameRate = 50u;
        }
    }

    void FreeScreenRenderBuffers() {
        if (RenderScratchBuffer != nullptr) {
            pas::free(RenderScratchBuffer);
            RenderScratchBuffer = nullptr;
        }
        if (AuxRenderBuffer != nullptr) {
            pas::free(AuxRenderBuffer);
            AuxRenderBuffer = nullptr;
        }
        if (ScreenRenderBuffer != nullptr) {
            pas::free(ScreenRenderBuffer);
            ScreenRenderBuffer = nullptr;
        }
        PresentationDepth = 0;
    }

    void LogPresentationParameters() {
        std::uint32_t CurrentValue{};
        std::uint32_t PreviousValue{};
        pas::WideString Text{};
        auto LogPresentationField = [&](pas::WideString Name) -> void {
            if (CurrentValue == PreviousValue) {
                Text = pas::concat_wide({Name, u" = ", pas::wide_int64_to_str(static_cast<std::int64_t>(CurrentValue))});
            } else {
                Text = pas::concat_wide({Name, u" = ", pas::wide_int64_to_str(static_cast<std::int64_t>(CurrentValue)), u", previous value = ", pas::wide_int64_to_str(static_cast<std::int64_t>(PreviousValue))});
            }
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Text));
        };
        GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
        GR_Main::AppendLogLineThreadSafe("D3DPresent structure:"_a);
        CurrentValue = Direct3DPresentParameters.BackBufferWidth;
        PreviousValue = PreviousPresentParameters.BackBufferWidth;
        LogPresentationField(u"BackBufferWidth"_w);
        CurrentValue = Direct3DPresentParameters.BackBufferHeight;
        PreviousValue = PreviousPresentParameters.BackBufferHeight;
        LogPresentationField(u"BackBufferHeight"_w);
        CurrentValue = Direct3DPresentParameters.BackBufferCount;
        PreviousValue = PreviousPresentParameters.BackBufferCount;
        LogPresentationField(u"BackBufferCount"_w);
        CurrentValue = Direct3DPresentParameters.BackBufferFormat;
        PreviousValue = PreviousPresentParameters.BackBufferFormat;
        LogPresentationField(u"BackBufferFormat"_w);
        CurrentValue = Direct3DPresentParameters.MultiSampleQuality;
        PreviousValue = PreviousPresentParameters.MultiSampleQuality;
        LogPresentationField(u"MultiSampleQuality"_w);
        CurrentValue = Direct3DPresentParameters.MultiSampleType;
        PreviousValue = PreviousPresentParameters.MultiSampleType;
        LogPresentationField(u"MultiSampleType"_w);
        CurrentValue = Direct3DPresentParameters.SwapEffect;
        PreviousValue = PreviousPresentParameters.SwapEffect;
        LogPresentationField(u"SwapEffect"_w);
        CurrentValue = Direct3DPresentParameters.DeviceWindow;
        PreviousValue = PreviousPresentParameters.DeviceWindow;
        LogPresentationField(u"hDeviceWindow"_w);
        CurrentValue = Direct3DPresentParameters.Windowed != 0;
        PreviousValue = PreviousPresentParameters.Windowed != 0;
        LogPresentationField(u"Windowed"_w);
        CurrentValue = Direct3DPresentParameters.EnableAutoDepthStencil != 0;
        PreviousValue = PreviousPresentParameters.EnableAutoDepthStencil != 0;
        LogPresentationField(u"EnableAutoDepthStencil"_w);
        CurrentValue = Direct3DPresentParameters.AutoDepthStencilFormat;
        PreviousValue = PreviousPresentParameters.AutoDepthStencilFormat;
        LogPresentationField(u"AutoDepthStencilFormat"_w);
        CurrentValue = Direct3DPresentParameters.Flags;
        PreviousValue = PreviousPresentParameters.Flags;
        LogPresentationField(u"Flags"_w);
        CurrentValue = Direct3DPresentParameters.FullScreenRefreshRateInHz;
        PreviousValue = PreviousPresentParameters.FullScreenRefreshRateInHz;
        LogPresentationField(u"FullScreen_RefreshRateInHz"_w);
        CurrentValue = Direct3DPresentParameters.PresentationInterval;
        PreviousValue = PreviousPresentParameters.PresentationInterval;
        LogPresentationField(u"PresentationInterval"_w);
        GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
    }

    pas::AnsiString Direct3DErrorText(std::int32_t Code) {
        pas::AnsiString Result{};
        switch (Code) {
            case 0: Result = "D3D_OK"_a; break;
            case -2005530600: Result = "D3DERR_WRONGTEXTUREFORMAT"_a; break;
            case -2005530599: Result = "D3DERR_UNSUPPORTEDCOLOROPERATION"_a; break;
            case -2005530598: Result = "D3DERR_UNSUPPORTEDCOLORARG"_a; break;
            case -2005530597: Result = "D3DERR_UNSUPPORTEDALPHAOPERATION"_a; break;
            case -2005530596: Result = "D3DERR_UNSUPPORTEDALPHAARG"_a; break;
            case -2005530595: Result = "D3DERR_TOOMANYOPERATIONS"_a; break;
            case -2005530594: Result = "D3DERR_CONFLICTINGTEXTUREFILTER"_a; break;
            case -2005530593: Result = "D3DERR_UNSUPPORTEDFACTORVALUE"_a; break;
            case -2005530591: Result = "D3DERR_CONFLICTINGRENDERSTATE"_a; break;
            case -2005530590: Result = "D3DERR_UNSUPPORTEDTEXTUREFILTER"_a; break;
            case -2005530586: Result = "D3DERR_CONFLICTINGTEXTUREPALETTE"_a; break;
            case -2005530585: Result = "D3DERR_DRIVERINTERNALERROR"_a; break;
            case -2005530522: Result = "D3DERR_NOTFOUND"_a; break;
            case -2005530521: Result = "D3DERR_MOREDATA"_a; break;
            case -2005530520: Result = "D3DERR_DEVICELOST"_a; break;
            case -2005530519: Result = "D3DERR_DEVICENOTRESET"_a; break;
            case -2005530518: Result = "D3DERR_NOTAVAILABLE"_a; break;
            case -2005532292: Result = "D3DERR_OUTOFVIDEOMEMORY"_a; break;
            case -2005530517: Result = "D3DERR_INVALIDDEVICE"_a; break;
            case -2005530516: Result = "D3DERR_INVALIDCALL"_a; break;
            case -2005530515: Result = "D3DERR_DRIVERINVALIDCALL"_a; break;
            default: return SysUtils::IntToStr(Code);
        }
        return pas::concat_ansi({Result, " (", SysUtils::IntToStr(Code), ")"});
    }

    void GR_DXInit() {
        Direct3D9::IDirect3D9 cpp_result{};
        std::int32_t Code{};
        Direct3D9::IDirect3DSurface9 Surface{};
        std::int32_t Index{};
        std::int32_t MiniMapSize{};
        float Angle{};
        std::uint32_t DeviceFlags{};
        Direct3D9::TD3DAdapterIdentifier9 Identifier{};
        Direct3D9::TD3DCaps9 Caps{};
        GR_Main::FreeScreenRenderBuffers();
        WindowedModeRequested = false;
        if (UserSettingsConfig->CountParams(u"Window"_wref.get()) > 0) {
            WindowedModeRequested = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"Window"_wref.get()))));
        }
        VSyncEnabled = false;
        if (UserSettingsConfig->CountParams(u"VSync"_wref.get()) > 0) {
            VSyncEnabled = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"VSync"_wref.get()))));
        }
        GlobalsV::HardwareRenderingRequested = false;
        if (UserSettingsConfig->CountParams(u"HardwareRender"_wref.get()) > 0) {
            GlobalsV::HardwareRenderingRequested = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"HardwareRender"_wref.get()))));
        }
        GlobalsV::HardwareRenderingEnabled = GlobalsV::HardwareRenderingRequested && (static_cast<std::uint8_t>(GlobalsV::RunningUnderWine ^ 1) || UserSettingsConfig->CountParams(u"AllowHardwareRenderUnderWine"_wref.get()) != 0 && GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"AllowHardwareRenderUnderWine"_wref.get())))));
        GlobalsV::ScaleViewportToWindow = true;
        if (UserSettingsConfig->CountParams(u"RenderModeScale"_wref.get()) > 0) {
            GlobalsV::ScaleViewportToWindow = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"RenderModeScale"_wref.get()))));
        }
        GR_DX::TextureManagerDisabled = false;
        if (UserSettingsConfig->CountParams(u"DisableTextureManager"_wref.get()) > 0) {
            GR_DX::TextureManagerDisabled = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"DisableTextureManager"_wref.get()))));
        }
        PresentWithoutLimit = false;
        if (UserSettingsConfig->CountParams(u"DisableFrameLimit"_wref.get()) > 0) {
            PresentWithoutLimit = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"DisableFrameLimit"_wref.get()))));
        }
        DisableHardwareVertexProcessing = false;
        if (UserSettingsConfig->CountParams(u"DisableHWVertexProcessing"_wref.get()) > 0) {
            DisableHardwareVertexProcessing = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"DisableHWVertexProcessing"_wref.get()))));
        }
        DisableMultithreadFlag = false;
        if (UserSettingsConfig->CountParams(u"DisableMultithreadFlag"_wref.get()) > 0) {
            DisableMultithreadFlag = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"DisableMultithreadFlag"_wref.get()))));
        }
        DisableTripleBuffer = false;
        if (UserSettingsConfig->CountParams(u"DisableTripleBuffer"_wref.get()) > 0) {
            DisableTripleBuffer = GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(UserSettingsConfig->GetParamByPathOrMarker(u"DisableTripleBuffer"_wref.get()))));
        }
        ScreenRenderBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
        RenderScratchBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
        AuxRenderBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
        std::uint16_t ControlWord = 0x0000133f;
        // The native code directly clears x87 exceptions and loads the local control
        // word; this small handwritten sequence has no Pascal intrinsic equivalent.
        ControlWord = ControlWord & 0x0000fcff;
        System::Set8087CW(ControlWord);
        try {
            if (Direct3D == nullptr) {
                Direct3D = (Direct3D9::CreateDirect3D9(0x80000020u, cpp_result), cpp_result);
                // Native constructs this exception without raising it.
                if (Direct3D == nullptr) {
                    pas::construct_call<DirectXRenderException::EDirectXRender>(DirectXRenderException::EDirectXRender_Create, "GR_DXInit()::Direct3DCreate9(...)"_a);
                }
                GR_Main::AppendLogLineThreadSafe("Initializing DX9.... ok!"_a);
                Direct3D9::IDirect3D9_GetAdapterIdentifier(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, 0u, Identifier);
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Videocard=", pas::array_text<pas::AnsiString>(Identifier.Description.elements, 512)}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Driver=", pas::array_text<pas::AnsiString>(Identifier.Driver.elements, 512)}));
            }
            Robot::SupportedMultiSamples = nullptr;
            Robot::SupportedMultiSampleCount = 0;
            for (Index = 0; Index <= 16; ++Index) {
                if (Direct3D9::IDirect3D9_CheckDeviceMultiSampleType(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, Direct3D9::D3DFMT_A8R8G8B8, 0, Index, nullptr) == 0) {
                    Robot::SupportedMultiSamples.set_length(Robot::SupportedMultiSampleCount + 1);
                    Robot::SupportedMultiSamples[Robot::SupportedMultiSampleCount] = Index;
                    ++Robot::SupportedMultiSampleCount;
                }
            }
            if (Direct3D9::IDirect3D9_GetDeviceCaps(Direct3D, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, Caps) == 0) {
                Robot::MaximumAnisotropy = Caps.MaxAnisotropy;
            }
            GR_DX::MaxTextureSize.X = Caps.MaxTextureWidth;
            GR_DX::MaxTextureSize.Y = Caps.MaxTextureHeight;
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Max Texture Size=", SysUtils::Int64ToStr(Caps.MaxTextureWidth), "x", SysUtils::Int64ToStr(Caps.MaxTextureHeight)}));
            GR_Main::EnumerateAndSelectDisplayModes();
            GR_Main::PreparePresentationParameters();
            GR_Main::ApplyMainWindowGeometry();
            if (Direct3DDevice == nullptr) {
                DeviceFlags = 0u;
                if (!DisableMultithreadFlag) {
                    DeviceFlags = Direct3D9::D3DCREATE_MULTITHREADED;
                }
                if (!DisableHardwareVertexProcessing) {
                    Code = ([&] {
                        Direct3D9::IDirect3DDevice9& direct3DDevice = pas::out_interface(Direct3DDevice);
                        std::uint32_t mainWindowHandle = MainWindowHandle;
                        pas::ComView<Direct3D9::IDirect3D9_Tag> direct3D = Direct3D;
                        return Direct3D9::IDirect3D9_CreateDevice(direct3D, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, mainWindowHandle, DeviceFlags | Direct3D9::D3DCREATE_HARDWARE_VERTEXPROCESSING, Direct3DPresentParameters, direct3DDevice);
                    }());
                } else {
                    Code = -1;
                }
                if (Code != 0) {
                    Code = ([&] {
                        Direct3D9::IDirect3DDevice9& direct3DDevice_2 = pas::out_interface(Direct3DDevice);
                        std::uint32_t mainWindowHandle_2 = MainWindowHandle;
                        pas::ComView<Direct3D9::IDirect3D9_Tag> direct3D_2 = Direct3D;
                        return Direct3D9::IDirect3D9_CreateDevice(direct3D_2, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, mainWindowHandle_2, DeviceFlags | Direct3D9::D3DCREATE_SOFTWARE_VERTEXPROCESSING, Direct3DPresentParameters, direct3DDevice_2);
                    }());
                }
                if (Code != 0 || Direct3DDevice == nullptr) {
                    GR_Main::AppendLogLineThreadSafe("Error: GR_DXInit()::GR_Direct3D.CreateDevice, failed to create device, trying to switch to minimal resolution..."_a);
                    SelectedGameDisplayMode = SmallestGameDisplayMode;
                    GameScreenWidth = GameDisplayModes[SelectedGameDisplayMode].Width;
                    GameScreenHeight = GameDisplayModes[SelectedGameDisplayMode].Height;
                    PresentationWidth = GameScreenWidth;
                    PresentationHeight = GameScreenHeight;
                    ExtraScreenWidth = GameScreenWidth - 1024;
                    ExtraScreenHeight = GameScreenHeight - 768;
                    GameScreenRect = ClassesImports::Rect(0, 0, GameScreenWidth, GameScreenHeight);
                    PresentationRect = ClassesImports::Rect(0, 0, PresentationWidth, PresentationHeight);
                    WindowedModeRequested = true;
                    AlternateViewportEnabled = false;
                    GR_Main::PreparePresentationParameters();
                    GR_Main::ApplyMainWindowGeometry();
                    Code = ([&] {
                        Direct3D9::IDirect3DDevice9& direct3DDevice_3 = pas::out_interface(Direct3DDevice);
                        std::uint32_t mainWindowHandle_3 = MainWindowHandle;
                        pas::ComView<Direct3D9::IDirect3D9_Tag> direct3D_3 = Direct3D;
                        return Direct3D9::IDirect3D9_CreateDevice(direct3D_3, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, mainWindowHandle_3, DeviceFlags | Direct3D9::D3DCREATE_SOFTWARE_VERTEXPROCESSING, Direct3DPresentParameters, direct3DDevice_3);
                    }());
                    if (Code != 0 || Direct3DDevice == nullptr) {
                        GR_Main::LogPresentationParameters();
                        pas::raise(pas::construct_call<DirectXRenderException::EDirectXRender>(DirectXRenderException::EDirectXRender_CreateCode, "GR_DXInit()::GR_Direct3D.CreateDevice"_a, Code));
                    }
                }
                {
                    Direct3D9::IDirect3DSurface9& surface = pas::out_interface(Surface);
                    pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_4 = Direct3DDevice;
                    Direct3D9::IDirect3DDevice9_GetRenderTarget(direct3DDevice_4, 0u, surface);
                }
                Direct3D9::IDirect3DDevice9_ColorFill(Direct3DDevice, Surface, nullptr, 0u);
                {
                    Direct3D9::IDirect3DSurface9& surface_2 = pas::out_interface(Surface);
                    pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_5 = Direct3DDevice;
                    Direct3D9::IDirect3DDevice9_GetBackBuffer(direct3DDevice_5, 0u, 0u, Direct3D9::D3DBACKBUFFER_TYPE_MONO, surface_2);
                }
                Direct3D9::IDirect3DDevice9_ColorFill(Direct3DDevice, Surface, nullptr, 0u);
                {
                    Direct3D9::IDirect3DSurface9& surface_3 = pas::out_interface(Surface);
                    pas::ComView<Direct3D9::IDirect3DDevice9_Tag> direct3DDevice_6 = Direct3DDevice;
                    Direct3D9::IDirect3DDevice9_GetBackBuffer(direct3DDevice_6, 0u, 1u, Direct3D9::D3DBACKBUFFER_TYPE_MONO, surface_3);
                }
                Direct3D9::IDirect3DDevice9_ColorFill(Direct3DDevice, Surface, nullptr, 0u);
                GR_Main::AppendLogLineThreadSafe("Initializing Direct3D device... ok!"_a);
            } else {
                Code = Direct3D9::IDirect3DDevice9_Reset(Direct3DDevice, &Direct3DPresentParameters);
                if (Code != 0) {
                    GR_Main::LogPresentationParameters();
                    pas::raise(pas::construct_call<DirectXRenderException::EDirectXRender>(DirectXRenderException::EDirectXRender_CreateCode, "GR_DXInit()::GR_D3DDevice.Reset"_a, Code));
                }
                GR_Main::AppendLogLineThreadSafe("Re-initializing Direct3D device... ok!"_a);
            }
            GR_DX::AvailableTextureBytes = Direct3D9::IDirect3DDevice9_GetAvailableTextureMem(Direct3DDevice);
            GR_DX::ReservedTextureBytes = EC_Str::ExtractDigitsToIntW(pas::view(UserSettingsConfig->GetParamByPathOrMarker(u"VideoMemSizeLimit"_wref.get())));
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Available Video Memory=", SysUtils::Int64ToStr(GR_DX::AvailableTextureBytes >> 10), " KB"}));
            if (static_cast<std::int32_t>(GR_DX::ReservedTextureBytes) > 0) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Available Video Memory Override=", SysUtils::IntToStr(GR_DX::ReservedTextureBytes << 10), " KB"}));
                GR_DX::ReservedTextureBytes = GR_DX::AvailableTextureBytes - (GR_DX::ReservedTextureBytes << 10 << 10);
            }
            UserSettingsConfig->GetParam(u"VideoMemSizeLimit"sv);
            for (Index = 0; Index <= 15; ++Index) {
                GR_DX::DrawVertices[Index].Z = 1.0f;
                GR_DX::DrawVertices[Index].RHW = 1.0f;
            }
            Direct3D9::IDirect3DDevice9_SetVertexShader(Direct3DDevice, nullptr);
            Direct3D9::IDirect3DDevice9_SetFVF(Direct3DDevice, Direct3D9::D3DFVF_XYZRHW | Direct3D9::D3DFVF_DIFFUSE | Direct3D9::D3DFVF_TEX1);
            GR_Main::ConfigureDefaultRenderState();
            GR_Main::AppendLogTextThreadSafe("Display Mode="_a);
            if (!Direct3DPresentParameters.Windowed) {
                GR_Main::AppendLogTextThreadSafe(pas::concat_ansi({SysUtils::Int64ToStr(Direct3DPresentParameters.BackBufferWidth), "x", SysUtils::Int64ToStr(Direct3DPresentParameters.BackBufferHeight), " ", SysUtils::Int64ToStr(Direct3DPresentParameters.FullScreenRefreshRateInHz), "Hz"}));
                if (AlternateViewportEnabled) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({" [", SysUtils::Int64ToStr(static_cast<std::uint32_t>(GameScreenWidth)), "x", SysUtils::Int64ToStr(static_cast<std::uint32_t>(GameScreenHeight)), "]"}));
                }
            } else {
                if (AlternateViewportEnabled) {
                    GR_Main::AppendLogTextThreadSafe(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(PresentationWidth)), "x", SysUtils::Int64ToStr(static_cast<std::uint32_t>(PresentationHeight)), " ["}));
                }
                GR_Main::AppendLogTextThreadSafe(pas::concat_ansi({SysUtils::Int64ToStr(static_cast<std::uint32_t>(GameScreenWidth)), "x", SysUtils::Int64ToStr(static_cast<std::uint32_t>(GameScreenHeight))}));
                if (AlternateViewportEnabled) {
                    GR_Main::AppendLogTextThreadSafe("]"_a);
                }
            }
            GR_Main::AppendLogLineThreadSafe(pas::AnsiString());
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (DirectXRenderException::EDirectXRender* E = pas::class_cast_if<DirectXRenderException::EDirectXRender*>(cpp_exception)) {
                GlobalsV::ThreeDimensionalModeEnabled = false;
                Direct3DDevice = nullptr;
                Direct3D = nullptr;
            } else {
                throw;
            }
        }
        CurrentPixelFormat = pas::make_object<GR_GraphBuf::TPixelFormatGR>();
        CurrentPixelFormat->RedMask = 0x0000f800u;
        CurrentPixelFormat->GreenMask = 0x000007e0u;
        CurrentPixelFormat->BlueMask = 0x0000001fu;
        CurrentPixelFormat->AlphaMask = 0u;
        CurrentPixelFormat->BytesPerPixel = 2;
        CurrentPixelFormat->RebuildChannelMetrics();
        BlendPixel16 = TBlendPixel16(OKGR_PixelAlpha_16);
        TriangleRasterizer16 = TTriangleRasterizer16(OKGF_Triangle_16);
        LineRasterizer16 = TLineRasterizer16(OKGF_LineIp_16);
        ScreenRenderBuffer->AllocateNativePitch(GameScreenWidth, GameScreenHeight, 2 * GameScreenWidth);
        if (GameDataConfig->CountParams(u"MiniMapBufSize"_wref.get()) > 0) {
            MiniMapSize = EC_Str::ExtractDigitsToIntW(pas::view(GameDataConfig->GetParam(u"MiniMapBufSize"sv)));
        } else {
            MiniMapSize = 156;
        }
        RenderScratchBuffer->AllocateNative(MiniMapSize, MiniMapSize);
        GR_Main::ApplyGammaRamp(DisplayBrightness, DisplayContrast);
        // Native uses two different approximations of pi for these tables.
        for (Index = 0; Index <= 360; ++Index) {
            Angle = Index * pas::constant(aMyFunction::GamePi / 1.8E+2L);
            GR_DX::CircleCos[Index] = System::Cos(Angle);
            GR_DX::CircleSin[Index] = System::Sin(Angle);
        }
        for (Index = 0; Index <= 359; ++Index) {
            GR_DX::LineAlphaTable[Index] = System::Trunc(System::Cos(pas::real_divide(Index, 1.8E+2L) * 3.14159265354L) * 127.0L + 128.0L);
        }
        GR_DX::PendingPointCapacity = 1024;
        GR_DX::PendingPoints.set_length(GR_DX::PendingPointCapacity);
    }

    // Linear RGB ramp with brightness/contrast endpoints; returns when no device is present.
    void ApplyGammaRamp(float Brightness, float Contrast) {
        std::int32_t Index{};
        float LowInput{};
        float LowOutput{};
        float HighInput{};
        float HighOutput{};
        Direct3D9::TD3DGammaRamp Ramp{};
        if (Direct3DDevice == nullptr) {
            return;
        }
        if (Brightness >= 0.0L) {
            LowInput = 0.0f;
            LowOutput = Brightness * 0.5L;
            HighInput = 1.0L - Brightness * 0.5L;
            HighOutput = 1.0f;
        } else {
            LowInput = -Brightness * 0.5L;
            LowOutput = 0.0f;
            HighInput = 1.0f;
            HighOutput = 1.0L - -Brightness * 0.5L;
        }
        float Step = pas::real_divide(static_cast<long double>(HighOutput) - LowOutput, static_cast<long double>(HighInput) - LowInput);
        float Level = (0.5L - LowInput) * Step + LowOutput;
        LowInput = LowInput + 0.4L * Contrast * Level;
        HighInput = HighInput - (1.0L - Level) * (0.4L * Contrast);
        std::int32_t LowIndex = System::Round(LowInput * 255.0L);
        std::int32_t HighIndex = System::Round(HighInput * 255.0L);
        std::int32_t Value = System::Round(pas::real_max<float>(0.0f, LowOutput) * 65535.0L);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LowIndex - 1); cpp_range.next(Index); ) {
            pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Red, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
            pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Green, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
            pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Blue, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
        }
        Level = LowOutput;
        Step = pas::real_divide(static_cast<long double>(HighOutput) - LowOutput, HighIndex - LowIndex);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(LowIndex, HighIndex - 1); cpp_range_2.next(Index); ) {
            if (Index >= 0 && Index <= 255) {
                Value = System::Round(pas::real_max<float>(0.0f, Level) * 65535.0L);
                if (Value < 0) {
                    Value = 0;
                } else if (Value > 65535) {
                    Value = 65535;
                }
                pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Red, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
                pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Green, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
                pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Blue, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
            }
            Level = static_cast<long double>(Level) + Step;
        }
        Value = System::Round(pas::real_min<float>(1.0f, HighOutput) * 65535.0L);
        {
            const std::int32_t cpp_first = HighIndex;
            if (cpp_first <= 255) {
                for (Index = cpp_first; Index <= 255; ++Index) {
                    pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Red, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
                    pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Green, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
                    pas::store_unaligned<std::uint16_t>(pas::byte_offset(&Ramp.Blue, Index * sizeof(std::uint16_t)), static_cast<std::uint16_t>(Value));
                }
            }
        }
        Direct3D9::IDirect3DDevice9_SetGammaRamp(Direct3DDevice, 0u, 0u, &Ramp);
    }

    void GR_DXReset() {
        std::int32_t ErrorCode{};
        GR_Main::PreparePresentationParameters();
        if (!Direct3DPresentParameters.Windowed) {
            GR_Main::ApplyMainWindowGeometry();
        }
        if (Direct3DDevice == nullptr) {
            ErrorCode = ([&] {
                Direct3D9::IDirect3DDevice9& direct3DDevice = pas::out_interface(Direct3DDevice);
                std::uint32_t mainWindowHandle = MainWindowHandle;
                pas::ComView<Direct3D9::IDirect3D9_Tag> direct3D = Direct3D;
                return Direct3D9::IDirect3D9_CreateDevice(direct3D, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, mainWindowHandle, Direct3D9::D3DCREATE_MULTITHREADED | Direct3D9::D3DCREATE_HARDWARE_VERTEXPROCESSING, Direct3DPresentParameters, direct3DDevice);
            }());
            if (ErrorCode != 0) {
                ErrorCode = ([&] {
                    Direct3D9::IDirect3DDevice9& direct3DDevice_2 = pas::out_interface(Direct3DDevice);
                    std::uint32_t mainWindowHandle_2 = MainWindowHandle;
                    pas::ComView<Direct3D9::IDirect3D9_Tag> direct3D_2 = Direct3D;
                    return Direct3D9::IDirect3D9_CreateDevice(direct3D_2, Direct3D9::D3DADAPTER_DEFAULT, Direct3D9::D3DDEVTYPE_HAL, mainWindowHandle_2, Direct3D9::D3DCREATE_MULTITHREADED | Direct3D9::D3DCREATE_SOFTWARE_VERTEXPROCESSING, Direct3DPresentParameters, direct3DDevice_2);
                }());
            }
            if (ErrorCode != 0 || Direct3DDevice == nullptr) {
                GR_Main::LogPresentationParameters();
                pas::raise(pas::construct_call<DirectXRenderException::EDirectXRender>(DirectXRenderException::EDirectXRender_CreateCode, "GR_DXReset()::GR_Direct3D.CreateDevice(...)"_a, ErrorCode));
            }
        } else {
            ErrorCode = Direct3D9::IDirect3DDevice9_Reset(Direct3DDevice, &Direct3DPresentParameters);
            if (ErrorCode != 0) {
                GR_Main::LogPresentationParameters();
                pas::raise(pas::construct_call<DirectXRenderException::EDirectXRender>(DirectXRenderException::EDirectXRender_CreateCode, "GR_DXReset()::GR_D3DDevice.Reset(...)"_a, ErrorCode));
            }
        }
        Direct3D9::IDirect3DDevice9_SetVertexShader(Direct3DDevice, nullptr);
        Direct3D9::IDirect3DDevice9_SetFVF(Direct3DDevice, Direct3D9::D3DFVF_XYZRHW | Direct3D9::D3DFVF_DIFFUSE | Direct3D9::D3DFVF_TEX1);
        GR_Main::ConfigureDefaultRenderState();
        if (Direct3DPresentParameters.Windowed) {
            GR_Main::ApplyMainWindowGeometry();
        }
        GR_Main::ShowAndFocusMainWindow();
    }

    // Increments the nesting count and always returns true.
    std::uint8_t BeginFramePresentation() {
        ++PresentationDepth;
        return true;
    }

    // Presents at the outermost level, subject to the frame-rate limit.
    void EndFramePresentation() {
        std::uint32_t Tick{};
        if (PresentationDepth > 0) {
            --PresentationDepth;
            if (PresentationDepth == 0) {
                if (PresentWithoutLimit) {
                    GR_Main::PresentScreenBuffer();
                } else {
                    Tick = MMSystem::timeGetTime();
                    if (pas::idiv(1000, PresentationFrameRate) < Tick - LastPresentationTick) {
                        LastPresentationTick = Tick;
                        GR_Main::PresentScreenBuffer();
                    }
                }
            }
        }
    }

    // Hardware mode ends/presents/restarts the scene; software mode draws the buffer texture unless OffscreenTexture is assigned.
    void PresentScreenBuffer() {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        if (GlobalsV::HardwareRenderingEnabled) {
            Direct3D9::IDirect3DDevice9_EndScene(Direct3DDevice);
            Direct3D9::IDirect3DDevice9_Present(Direct3DDevice, nullptr, nullptr, 0u, nullptr);
            Direct3D9::IDirect3DDevice9_BeginScene(Direct3DDevice);
        } else if (OffscreenTexture == nullptr) {
            Direct3D9::IDirect3DDevice9_BeginScene(Direct3DDevice);
            if (!AlternateViewportEnabled) {
                GR_DX::DrawTexture((ScreenRenderBuffer->GetTexture(cpp_result), cpp_result), 0, 0, 255, GR_DX::RgbWhite, nullptr, false, false);
            } else if (GlobalsV::ScaleViewportToWindow) {
                std::int32_t presentationWidth = PresentationWidth;
                std::int32_t presentationHeight = PresentationHeight;
                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture = (ScreenRenderBuffer->GetTexture(cpp_result_2), cpp_result_2);
                GR_DX::DrawTextureSized(texture, 0, 0, presentationWidth, presentationHeight, 255, GR_DX::RgbWhite, nullptr, false, false);
            } else {
                pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture_2 = (ScreenRenderBuffer->GetTexture(cpp_result_3), cpp_result_3);
                std::int32_t y = ViewportOffset.Y;
                std::int32_t x = ViewportOffset.X;
                GR_DX::DrawTexture(texture_2, x, y, 255, GR_DX::RgbWhite, nullptr, false, false);
            }
            Direct3D9::IDirect3DDevice9_EndScene(Direct3DDevice);
            Direct3D9::IDirect3DDevice9_Present(Direct3DDevice, nullptr, nullptr, 0u, nullptr);
        }
    }

    // Method callback receives Context/EAX, Message/EDX, WParam/ECX and LParam on stack. Returns zero when exiting.
    std::int32_t GR_WinMessage(TWindowMessageCallbackGR Callback) {
        std::int32_t Result{};
        std::int32_t ContinueLoop{};
        Windows::TMsg Msg{};
        Windows::TTrackMouseEvent EventTrack{};
        std::int32_t Stage = 0;
        try {
            if (SoundManager != nullptr) {
                SoundManager->UpdateFades();
            }
            Stage = 1;
            ContinueLoop = 1;
            while (true) {
                if (ExitScreenLoop) {
                    return 0;
                }
                Stage = 2;
                if (([&] {
                    std::uint32_t cpp_left = MMSystem::timeGetTime();
                    return cpp_left - LastWindowMessageTick;
                }()) > 5000 && static_cast<std::uint8_t>(MessageIdle ^ 1)) {
                    MessageIdle = true;
                    if (OnMessageIdle != nullptr) {
                        GR_Main::OnMessageIdle();
                    }
                }
                Stage = 3;
                if (LastMouseMessageTick != 0) {
                    if (([&] {
                        std::uint32_t cpp_left_2 = MMSystem::timeGetTime();
                        return cpp_left_2 - LastMouseMessageTick;
                    }()) > 100 && RuntimeActive) {
                        LastMouseMessageTick = MMSystem::timeGetTime();
                        GR_Main::PostMouseMoveMessage();
                    }
                }
                Stage = 4;
                while (WindowsSdk::PeekMessageW(Msg, 0u, 0u, 0u, WindowsSdk::PM_REMOVE) != 0) {
                    Stage = 5;
                    if (MessageIdle && OnMessageResume != nullptr) {
                        GR_Main::OnMessageResume();
                    }
                    MessageIdle = false;
                    LastWindowMessageTick = MMSystem::timeGetTime();
                    Stage = 6;
                    WindowsSdk::TranslateMessage(Msg);
                    Stage = 7;
                    if (Msg.message != DebugCommandMessage) {
                        if (Msg.message == MessagesSdk::WM_QUIT) {
                            ContinueLoop = 0;
                        }
                    }
                    Stage = 8;
                    if (Msg.message == MessagesSdk::WM_MOUSEMOVE) {
                        pas::fill_memory(&EventTrack, static_cast<std::int32_t>(sizeof(Windows::TTrackMouseEvent)), static_cast<std::uint8_t>(0));
                        EventTrack.cbSize = static_cast<std::int32_t>(sizeof(Windows::TTrackMouseEvent));
                        EventTrack.dwFlags = WindowsSdk::TME_LEAVE;
                        EventTrack.hwndTrack = MainWindowHandle;
                        WindowsSdk::TrackMouseEvent(EventTrack);
                        LastMouseMessageTick = 0u;
                    }
                    Stage = 9;
                    WindowsSdk::DispatchMessageW(Msg);
                    Stage = 10;
                    if (Msg.hwnd == MainWindowHandle && pas::assigned(Callback) && Forms::Application->GetActive()) {
                        Callback(Msg.message, Msg.wParam, Msg.lParam);
                    }
                }
                Stage = 11;
                if (ContinueLoop == 0 || RuntimeActive) {
                    break;
                }
                SysUtilsImports::Sleep(1u);
            }
            Result = ContinueLoop;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure GR_WinMessage, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
        return Result;
    }

    // Presents OffscreenTexture, fitting or cropping it to the viewport.
    void DrawOffscreenTexture() {
        std::int32_t X{};
        std::int32_t Y{};
        std::uint32_t Width{};
        std::uint32_t Height{};
        std::uint32_t ViewWidth{};
        std::uint32_t ViewHeight{};
        Direct3D9::TD3DSurfaceDesc Desc{};
        if (OffscreenTexture != nullptr) {
            if (!OffscreenFrameUpdated) {
                std::uint32_t cpp_left = MMSystem::timeGetTime();
                if (cpp_left - OffscreenLastPresentationTick < 100) {
                    return;
                }
            }
            OffscreenLastPresentationTick = MMSystem::timeGetTime();
            Direct3D9::IDirect3DTexture9_GetLevelDesc(OffscreenTexture, 0u, Desc);
            if (AlternateViewportEnabled) {
                ViewWidth = PresentationWidth;
                ViewHeight = PresentationHeight;
            } else {
                ViewWidth = GameScreenWidth;
                ViewHeight = GameScreenHeight;
            }
            Width = ViewWidth;
            Height = System::Round(pas::real_divide(Desc.Height, Desc.Width) * ViewWidth);
            if (Height > ViewHeight && static_cast<std::uint8_t>(OffscreenFillViewport ^ 1) || Height < ViewHeight && OffscreenFillViewport != false) {
                Width = System::Round(pas::real_divide(Desc.Width, Desc.Height) * ViewHeight);
                Height = ViewHeight;
            }
            X = static_cast<std::int32_t>(ViewWidth - Width) / 2;
            Y = static_cast<std::int32_t>(ViewHeight - Height) / 2;
            Direct3D9::IDirect3DDevice9_Clear(Direct3DDevice, 0u, nullptr, Direct3D9::D3DCLEAR_TARGET, 0u, 1.0f, 0u);
            if (GlobalsV::HardwareRenderingEnabled) {
                GR_DX::DrawTextureSized(OffscreenTexture, X, Y, Width, Height, 255, GR_DX::RgbWhite, reinterpret_cast<WindowsSdk::PRect>(&GameScreenRect), false, false);
            } else {
                Direct3D9::IDirect3DDevice9_BeginScene(Direct3DDevice);
                GR_DX::DrawTextureSized(OffscreenTexture, X, Y, Width, Height, 255, GR_DX::RgbWhite, reinterpret_cast<WindowsSdk::PRect>(&GameScreenRect), false, false);
                Direct3D9::IDirect3DDevice9_EndScene(Direct3DDevice);
                Direct3D9::IDirect3DDevice9_Present(Direct3DDevice, nullptr, nullptr, 0u, nullptr);
            }
        }
    }

    void CaptureScreenBackground(std::uint8_t ApplyEffects, std::uint8_t UnusedOption) {
        AuxRenderBuffer->LoadFromScreen(UnusedOption);
        if (ApplyEffects) {
            if (GlobalsV::BackgroundShade) {
                AuxRenderBuffer->AdjustBrightness(-50);
            }
            if (GlobalsV::BackgroundGrayscale) {
                AuxRenderBuffer->ConvertToGrayscale();
            }
        }
    }

    void CopyBgraToRgb24(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height) {
        std::int32_t X{};
        std::int32_t Y = 0;
        while (Y < Height) {
            X = 0;
            while (X < Width) {
                static_cast<GR_GraphBuf::PColorRGB>(EC_Mem::AddPointerOffset(Dest, X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGB))))->R = *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))))->R))));
                *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_GraphBuf::PColorRGB>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGB)))))->G)))) = *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))))->G))));
                *static_cast<std::uint8_t*>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_GraphBuf::PColorRGB>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGB)))))->B)))) = static_cast<GR_GraphBuf::PColorBGRA>(EC_Mem::AddPointerOffset(Source, X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA))))->B;
                ++X;
            }
            Dest = EC_Mem::AddPointerOffset(Dest, DestPitch);
            Source = EC_Mem::AddPointerOffset(Source, SourcePitch);
            ++Y;
        }
    }

    // Copies a due software frame into the recording ring and flushes it when full.
    void CaptureRecordingFrame() {
        std::uint32_t cpp_left_2 = MMSystem::timeGetTime();
        std::uint32_t cpp_left = cpp_left_2 - LastRecordingFrameTick;
        if (cpp_left >= static_cast<std::uint32_t>(RecordingFrameInterval)) {
            {
                void* pixels = ScreenRenderBuffer->GetPixels();
                std::int32_t pitchBytes = ScreenRenderBuffer->PitchBytes;
                std::int32_t gameScreenWidth = GameScreenWidth;
                std::int32_t gameScreenHeight = GameScreenHeight;
                void* cpp_arg = pas::list_get(RecordingFrameBuffers, RecordingFrameCount);
                std::int32_t cpp_arg_2 = GameScreenWidth * 2;
                GR_Main::Ex_OKGR_Copy_XY_XY_WORD(cpp_arg, cpp_arg_2, 0, 0, pixels, pitchBytes, 0, 0, gameScreenWidth, gameScreenHeight);
            }
            ++RecordingFrameCount;
            if (RecordingFrameCount >= pas::list_count(RecordingFrameBuffers)) {
                GR_Main::FlushRecordingFrames();
            }
            LastRecordingFrameTick = MMSystem::timeGetTime();
        }
    }

    // Writes pending RGB565 frames as sequential Film\NNNNNN.bmp files.
    void FlushRecordingFrames() {
        WindowsImports::THandle SearchHandle{};
        std::int32_t FirstFrameNumber{};
        std::int32_t Index{};
        pas::AnsiString Directory{};
        GR_GraphBuf::TGraphBufGR* Frame{};
        pas::AnsiString FileName{};
        Windows::TWin32FindDataA FindData{};
        if (RecordingFrameCount >= 1) {
            Directory = SysUtilsImports::GetCurrentDir();
            SysUtilsImports::SetCurrentDir("Film"_a);
            FirstFrameNumber = -1;
            SearchHandle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.*"), FindData);
            // Native code scans without testing for INVALID_HANDLE_VALUE.
            do {
                if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) != WindowsImports::FILE_ATTRIBUTE_DIRECTORY) {
                    FirstFrameNumber = std::max<std::int32_t>(FirstFrameNumber, EC_Str::ExtractDigitsToIntW(pas::view(static_cast<pas::WideString>(pas::array_text<pas::AnsiString>(FindData.cFileName.elements, 260)))));
                }
            } while (WindowsSdk::FindNextFile(SearchHandle, FindData) != 0);
            WindowsImports::FindClose(SearchHandle);
            SysUtilsImports::SetCurrentDir(Directory);
            ++FirstFrameNumber;
            Frame = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Frame->AllocateNativePitch(ScreenRenderBuffer->Width, ScreenRenderBuffer->Height, ScreenRenderBuffer->Width * 3);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, RecordingFrameCount - 1); cpp_range.next(Index); ) {
                {
                    std::int32_t pitchBytes = Frame->PitchBytes;
                    std::int32_t width = Frame->Width;
                    std::int32_t height = Frame->Height;
                    void* cpp_arg = pas::list_get(RecordingFrameBuffers, Index);
                    void* pixels = Frame->GetPixels();
                    std::int32_t cpp_arg_2 = GameScreenWidth * 2;
                    GR_Main::Ex_OKGF_Convert565toBGR(cpp_arg, cpp_arg_2, pixels, pitchBytes, width, height);
                }
                FileName = static_cast<pas::AnsiString>(pas::concat_wide({u"Film\\", EC_Str::IntToFixedWidthWideString(FirstFrameNumber + Index, 6), u".bmp"}));
                {
                    std::int32_t width_2 = Frame->Width;
                    std::int32_t height_2 = Frame->Height;
                    void* pixels_2 = Frame->GetPixels();
                    std::int32_t pitchBytes_2 = Frame->PitchBytes;
                    std::uint8_t* cpp_arg_3 = FileName.pchar();
                    GR_Main::WriteBmpFile(cpp_arg_3, pixels_2, pitchBytes_2, 24, 0x000000ffu, 0x0000ff00u, 0x00ff0000u, 0u, width_2, height_2);
                }
            }
            pas::free(Frame);
            RecordingFrameCount = 0;
        }
    }

    // Tests bit 15 of GetAsyncKeyState.
    std::uint8_t IsVirtualKeyDown(std::int32_t Key) {
        return (WindowsSdk::GetAsyncKeyState(Key) & 0x00008000) == 0x00008000;
    }

    // Returns one raw value, or a marker containing Path on lookup failure.
    pas::WideString LookupLocalizedTextByKey(const pas::WideString& Path) {
        return LanguageDataConfig->GetParamByPathOrMarker(Path);
    }

    // Returns one raw value; missing paths return empty and may create intermediate blocks.
    pas::WideString LookupLocalizedTextOrEmpty(const pas::WideString& Path) {
        if (LanguageDataConfig->CountParamsByPath(Path) > 0) {
            return LanguageDataConfig->GetParamByPathOrMarker(Path);
        }
        return pas::WideString();
    }

    pas::WideString FormatUnixDateTime(std::uint32_t Value) {
        System::TDateTime DateValue = DateUtils::UnixToDateTime(Value);
        return static_cast<pas::WideString>(SysUtilsImports::DateTimeToStr(DateValue));
    }

    // Always returns 2 in this binary.
    pas::WideString GiResourceSuffix() {
        return u"2"_w;
    }

    // Always returns 2; variant 1 retains the legacy quest-picture downscaling branch.
    std::int32_t GiResourceVariant() {
        return 2;
    }

    // Appends a line to the session log and closes the file. The lock is not released if a write raises.
    void AppendLogLineThreadSafe(const pas::AnsiString& Text) {
        if (SessionLogLock == nullptr) {
            SessionLogLock = pas::make_critical_section<pas::CriticalSection>();
        }
        pas::critical_enter(SessionLogLock);
        pas::text_open(SessionLog, 4, false);
        pas::text_writeln(SessionLog, Text, false);
        pas::text_close(SessionLog, false);
        pas::critical_leave(SessionLogLock);
    }

    // Creates #####add.log when absent; native unchecked TextFile I/O.
    void AppendDebugLogLine(const pas::AnsiString& Text) {
        pas::TextFile Log{};
        if (SessionLogLock == nullptr) {
            SessionLogLock = pas::make_critical_section<pas::CriticalSection>();
        }
        pas::critical_enter(SessionLogLock);
        pas::text_assign(Log, "#####add.log"_a, false);
        if (!SysUtilsImports::FileExists("#####add.log"_a)) {
            pas::text_open(Log, 3, false);
        } else {
            pas::text_open(Log, 4, false);
        }
        pas::text_writeln(Log, Text, false);
        pas::text_close(Log, false);
        pas::critical_leave(SessionLogLock);
    }

    // Appends only when #####add.log already exists; shares SessionLogLock and native unchecked TextFile I/O.
    void AppendOptionalDebugLogLine(const pas::AnsiString& Text) {
        pas::TextFile Log{};
        if (SysUtilsImports::FileExists("#####add.log"_a)) {
            if (SessionLogLock == nullptr) {
                SessionLogLock = pas::make_critical_section<pas::CriticalSection>();
            }
            pas::critical_enter(SessionLogLock);
            pas::text_assign(Log, "#####add.log"_a, false);
            pas::text_open(Log, 4, false);
            pas::text_writeln(Log, Text, false);
            pas::text_close(Log, false);
            pas::critical_leave(SessionLogLock);
        }
    }

    // Appends without a newline, flushes and closes the file.
    void AppendLogTextThreadSafe(const pas::AnsiString& Text) {
        if (SessionLogLock == nullptr) {
            SessionLogLock = pas::make_critical_section<pas::CriticalSection>();
        }
        pas::critical_enter(SessionLogLock);
        pas::text_open(SessionLog, 4, false);
        pas::text_write(SessionLog, Text, false);
        pas::text_close(SessionLog, false);
        pas::critical_leave(SessionLogLock);
    }

    void LogMemoryUsage() {
        TMemoryStatusEx Status{};
        Status.Length = static_cast<std::int32_t>(sizeof(TMemoryStatusEx));
        GR_Main::GlobalMemoryStatusEx(Status);
        GR_Main::AppendLogLineThreadSafe("Memory Info"_a);
        GR_Main::AppendLogLineThreadSafe("Physical Memory:"_a);
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Used=", SysUtils::Int64ToStr(Status.TotalPhys - Status.AvailPhys >> 10), " KB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Available=", SysUtils::Int64ToStr(Status.AvailPhys >> 10), " KB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Total=", SysUtils::Int64ToStr(Status.TotalPhys >> 10), " KB"}));
        GR_Main::AppendLogLineThreadSafe("Virtual Memory:"_a);
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Used=", SysUtils::Int64ToStr(Status.TotalVirtual - Status.AvailVirtual >> 10), " KB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Available=", SysUtils::Int64ToStr(Status.AvailVirtual >> 10), " KB"}));
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Total=", SysUtils::Int64ToStr(Status.TotalVirtual >> 10), " KB"}));
        if (GlobalCache != nullptr) {
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Cache Size=", SysUtils::IntToStr(pas::shr(GlobalCache->ResidentBytes, 10)), " KB"}));
        }
        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Textures Cache Size=", SysUtils::Int64ToStr(GR_DX::ResidentTextureBytes >> 10), " KB"}));
    }

    // Delphi wrappers translate DLL exceptions into Exception objects.
    // Borrows Source until ReadImagePixels consumes the context. Returns nil for unsupported input. Detection requires at least 34 bytes and accepts BMP, JFIF JPEG, PNG and supported PSD modes.
    EC_OKGF::POkgfReadContext BeginImageRead(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height) {
        EC_OKGF::POkgfReadContext Result{};
        try {
            Result = GR_Main::OKGF_ReadStart_Buf(Source, SourceSize, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_ReadStart_Buf"_a));
        }
        return Result;
    }

    // Consumes Context on success; returns nonzero on success.
    std::int32_t ReadImagePixels(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t BytesPerPixel) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGF_Read(Context, Pixels, PitchBytes, RedMask, GreenMask, BlueMask, AlphaMask, BytesPerPixel);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Read"_a));
        }
        return Result;
    }

    // Borrows Source; accepts indexed PNG and indexed or grayscale PSD. Returns nil on failure. BytesPerPixel is one or two.
    EC_OKGF::POkgfReadContext BeginIndexedImageRead(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height, std::int32_t& PaletteCount, std::int32_t& BytesPerPixel) {
        EC_OKGF::POkgfReadContext Result{};
        try {
            Result = GR_Main::OKGF_ReadStartPal_Buf(Source, SourceSize, Width, Height, PaletteCount, BytesPerPixel);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_ReadStartPal_Buf"_a));
        }
        return Result;
    }

    // Consumes Context on success. Palette requires the count returned by BeginIndexedImageRead.
    std::int32_t ReadIndexedImagePixels(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, GR_GraphBuf::PColorRGBA Palette) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGF_ReadPal(Context, Pixels, PitchBytes, Palette);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_ReadPal"_a));
        }
        return Result;
    }

    std::int32_t WritePngFile(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::int32_t HasAlpha, std::int32_t SwapRedBlue) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGF_Write_PNG_File(FileName, Pixels, PitchBytes, Width, Height, HasAlpha, SwapRedBlue);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Write_PNG_File"_a));
        }
        return Result;
    }

    std::int32_t WriteBmpFile(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t BitsPerPixel, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t Width, std::int32_t Height) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGF_Write_BMP_File(FileName, Pixels, PitchBytes, BitsPerPixel, RedMask, GreenMask, BlueMask, AlphaMask, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Write_BMP_File"_a));
        }
        return Result;
    }

    // Delphi exception wrappers around the named OKGF/OKGR DLL exports.
    // Ex_ distinguishes wrappers from DLL import symbols; full parameter types remain unresolved.
    void* Ex_OKGF_MulTable256x256() {
        void* Result{};
        try {
            Result = GR_Main::OKGF_MulTable256x256();
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_MulTable256x256"_a));
        }
        return Result;
    }

    std::uint32_t Ex_OKGF_DXVersion() {
        std::uint32_t Result{};
        try {
            Result = GR_Main::OKGF_DXVersion();
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_DXVersion"_a));
        }
        return Result;
    }

    void Ex_OKGR_AlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_AlphaBuf_Draw_RGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaBuf_Draw_RGBA"_a));
        }
    }

    void Ex_OKGR_TransAlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_TransAlphaBuf_Draw_RGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransAlphaBuf_Draw_RGBA"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_AlphaIndexed_Draw_RGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_Draw_RGBA"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_AlphaDraw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_AlphaIndexed_AlphaDraw_RGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_AlphaDraw_RGBA"_a));
        }
    }

    void Ex_OKGR_TransBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_TransBuf_Draw_RGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_Draw_RGBA"_a));
        }
    }

    void Ex_OKGR_TransBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_TransBuf_DrawClip_WORD(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_DrawClip_WORD"_a));
        }
    }

    void Ex_OKGR_TransBuf_HADrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_TransBuf_HADrawClip_16(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_HADrawClip_16"_a));
        }
    }

    std::int32_t Ex_OKGR_TransBuf_Build_WORD(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest, std::uint16_t TransparentColor) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_TransBuf_Build_WORD(Source, Pitch, Width, Height, Dest, TransparentColor);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_Build_WORD"_a));
        }
        return Result;
    }

    std::int32_t Ex_OKGR_TransBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_TransBuf_BuildFromRGBA_16(Source, Pitch, Width, Height, Dest);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_BuildFromRGBA_16"_a));
        }
        return Result;
    }

    void Ex_OKGR_TransAlphaBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_TransAlphaBuf_DrawClip_WORD(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransAlphaBuf_DrawClip_WORD"_a));
        }
    }

    void Ex_OKGR_AlphaBuf_DrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_AlphaBuf_DrawClip_16(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaBuf_DrawClip_16"_a));
        }
    }

    std::int32_t Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_TransAlphaBuf_BuildFromRGBA_16(Source, Pitch, Width, Height, Dest);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransAlphaBuf_BuildFromRGBA_16"_a));
        }
        return Result;
    }

    std::int32_t Ex_OKGR_AlphaBuf_BuildFromRGBA(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_AlphaBuf_BuildFromRGBA(Source, Pitch, Width, Height, Dest);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaBuf_BuildFromRGBA"_a));
        }
        return Result;
    }

    void Ex_OKGR_AlphaSimpleBuf_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_AlphaSimpleBuf_Draw_16(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaSimpleBuf_Draw_16"_a));
        }
    }

    void Ex_OKGR_AlphaSimpleBufPalAlpha_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, GR_GraphBuf::PColorRGBA Palette) {
        try {
            GR_Main::OKGR_AlphaSimpleBufPalAlpha_Draw_16(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height, Palette);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaSimpleBufPalAlpha_Draw_16"_a));
        }
    }

    void Ex_OKGR_MaskBuf_DrawClip_DWORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint32_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_MaskBuf_DrawClip_DWORD(Dest, Pitch, X, Y, Source, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_MaskBuf_DrawClip_DWORD"_a));
        }
    }

    void Ex_OKGR_MaskBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_MaskBuf_DrawClip_WORD(Dest, Pitch, X, Y, Source, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_MaskBuf_DrawClip_WORD"_a));
        }
    }

    void Ex_OKGR_TransBuf_FillAlphaClip_RGBA(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint32_t Color) {
        try {
            GR_Main::OKGR_TransBuf_FillAlphaClip_RGBA(Dest, Pitch, X, Y, Source, Clip, Color);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_FillAlphaClip_RGBA"_a));
        }
    }

    void Ex_OKGR_TransBuf_FillAlphaClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint16_t Color) {
        try {
            GR_Main::OKGR_TransBuf_FillAlphaClip_16(Dest, Pitch, X, Y, Source, Clip, Color);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_TransBuf_FillAlphaClip_16"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_CopyDrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_AlphaIndexed_CopyDrawClip_WORD(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_CopyDrawClip_WORD"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha) {
        try {
            GR_Main::OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(Dest, Pitch, X, Y, Source, Clip, Alpha);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_CopyDrawClip_Alpha_16"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_AlphaDrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_AlphaIndexed_AlphaDrawClip_16(Dest, Pitch, X, Y, Source, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_AlphaDrawClip_16"_a));
        }
    }

    void Ex_OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha) {
        try {
            GR_Main::OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(Dest, Pitch, X, Y, Source, Clip, Alpha);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16"_a));
        }
    }

    void* Ex_OKGR_RotateBuf_Build(std::int32_t Width, std::int32_t Height, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t CenterX, std::int32_t CenterY) {
        void* Result{};
        try {
            Result = GR_Main::OKGR_RotateBuf_Build(Width, Height, SourceWidth, SourceHeight, CenterX, CenterY);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_Build"_a));
        }
        return Result;
    }

    void Ex_OKGR_RotateBuf_Free(void* Buffer) {
        try {
            GR_Main::OKGR_RotateBuf_Free(Buffer);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_Free"_a));
        }
    }

    void Ex_OKGR_RotateBuf_Size(std::int32_t X, std::int32_t Y, std::uint8_t Angle, void* RotationMap, WindowsSdk::TRect& Bounds) {
        try {
            GR_Main::OKGR_RotateBuf_Size(X, Y, Angle, RotationMap, Bounds);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_Size"_a));
        }
    }

    void Ex_OKGR_RotateBuf_Draw_DWORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap) {
        try {
            GR_Main::OKGR_RotateBuf_Draw_DWORD(Dest, DestPitch, Source, SourcePitch, CenterX, CenterY, Angle, RotationMap);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_Draw_DWORD"_a));
        }
    }

    void Ex_OKGR_RotateBuf_Draw_BYTE(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, std::uint8_t Angle, void* RotationMap) {
        try {
            GR_Main::OKGR_RotateBuf_Draw_BYTE(Dest, DestPitch, Source, SourcePitch, Width, Height, Angle, RotationMap);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_Draw_BYTE"_a));
        }
    }

    void Ex_OKGR_RotateBuf_DrawTransClip_WORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_RotateBuf_DrawTransClip_WORD(Dest, DestPitch, Source, SourcePitch, CenterX, CenterY, Angle, RotationMap, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_RotateBuf_DrawTransClip_WORD"_a));
        }
    }

    void* Ex_OKGR_LightBuf_Create(std::int32_t Width, std::int32_t Height) {
        void* Result{};
        try {
            Result = GR_Main::OKGR_LightBuf_Create(Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_Create"_a));
        }
        return Result;
    }

    void Ex_OKGR_LightBuf_Destroy(void* Buffer) {
        try {
            GR_Main::OKGR_LightBuf_Destroy(Buffer);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_Destroy"_a));
        }
    }

    void Ex_OKGR_LightBuf_SetSme(void* Buffer, std::int32_t X, std::int32_t Y) {
        try {
            GR_Main::OKGR_LightBuf_SetSme(Buffer, X, Y);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_SetSme"_a));
        }
    }

    void Ex_OKGR_LightBuf_Init(void* Buffer, std::uint8_t Value) {
        try {
            GR_Main::OKGR_LightBuf_Init(Buffer, Value);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_Init"_a));
        }
    }

    void Ex_OKGR_LightBuf_LoadFromPalBuf(void* Buffer, void* Source, std::int32_t Width, std::int32_t Height, std::int32_t Pitch, GR_GraphBuf::PColorRGBA Palette) {
        try {
            GR_Main::OKGR_LightBuf_LoadFromPalBuf(Buffer, Source, Width, Height, Pitch, Palette);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_LoadFromPalBuf"_a));
        }
    }

    void Ex_OKGR_LightBuf_Rotate(void* Dest, void* Source, void* RotationMap, std::uint8_t Angle) {
        try {
            GR_Main::OKGR_LightBuf_Rotate(Dest, Source, RotationMap, Angle);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_LightBuf_Rotate"_a));
        }
    }

    void* Ex_OKGR_Planet2_TemplBuild(void* Source, std::int32_t Pitch, std::int32_t Height, std::int32_t TextureWidth, std::int32_t TextureHeight, std::int32_t& ByteCount) {
        void* Result{};
        try {
            Result = GR_Main::OKGR_Planet2_TemplBuild(Source, Pitch, Height, TextureWidth, TextureHeight, ByteCount);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet2_TemplBuild"_a));
        }
        return Result;
    }

    std::int32_t Ex_OKGR_Planet2_TemplDel(void* TemplateData) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_Planet2_TemplDel(TemplateData);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet2_TemplDel"_a));
        }
        return Result;
    }

    void Ex_OKGR_Planet2_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        try {
            GR_Main::OKGR_Planet2_DrawAndLight_32(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet2_DrawAndLight_32"_a));
        }
    }

    void Ex_OKGR_Planet2_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Planet2_DrawAndLightClip_16(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet2_DrawAndLightClip_16"_a));
        }
    }

    void Ex_OKGR_Planet3_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        try {
            GR_Main::OKGR_Planet3_DrawAndLight_32(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet3_DrawAndLight_32"_a));
        }
    }

    void Ex_OKGR_Planet3_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Planet3_DrawAndLightClip_16(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet3_DrawAndLightClip_16"_a));
        }
    }

    void Ex_OKGR_Planet4_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        try {
            GR_Main::OKGR_Planet4_DrawAndLight_32(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet4_DrawAndLight_32"_a));
        }
    }

    void Ex_OKGR_Planet4_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Planet4_DrawAndLightClip_16(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Planet4_DrawAndLightClip_16"_a));
        }
    }

    void Ex_OKGR_Copy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_Copy_XY_XY_WORD(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Copy_XY_XY_WORD"_a));
        }
    }

    void Ex_OKGR_PalCopy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, void* Palette, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_PalCopy_XY_XY_WORD(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Palette, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_PalCopy_XY_XY_WORD"_a));
        }
    }

    void Ex_OKGR_CopyTrans_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint16_t TransparentColor) {
        try {
            GR_Main::OKGR_CopyTrans_XY_XY_WORD(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height, TransparentColor);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_CopyTrans_XY_XY_WORD"_a));
        }
    }

    void Ex_OKGR_CopySingleBuf_XY_XY_WORD(void* Pixels, std::int32_t Pitch, std::int32_t DestX, std::int32_t DestY, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_CopySingleBuf_XY_XY_WORD(Pixels, Pitch, DestX, DestY, SourceX, SourceY, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_CopySingleBuf_XY_XY_WORD"_a));
        }
    }

    void Ex_OKGR_HACopy_XY_XY_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_HACopy_XY_XY_16(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_HACopy_XY_XY_16"_a));
        }
    }

    void Ex_OKGR_StretchGdi_WORD(void* Dest, std::uint32_t Width, std::uint32_t Height, void* Source, std::uint32_t SourceWidth, std::uint32_t SourceHeight) {
        try {
            GR_Main::OKGR_StretchGdi_WORD(Dest, Width, Height, Source, SourceWidth, SourceHeight);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_StretchGdi_WORD"_a));
        }
    }

    void Ex_OKGR_Fill_WORD(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint16_t Color) {
        try {
            GR_Main::OKGR_Fill_WORD(Pixels, Pitch, Width, Height, Color);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Fill_WORD"_a));
        }
    }

    void Ex_OKGF_ConvertRGBto565(void* Source, void* Dest, std::int32_t Pitch, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGF_ConvertRGBto565(Source, Dest, Pitch, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_ConvertRGBto565"_a));
        }
    }

    void Ex_OKGF_Convert565toRGB(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGF_Convert565toRGB(Source, SourcePitch, Dest, DestPitch, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Convert565toRGB"_a));
        }
    }

    void Ex_OKGF_Convert565toBGR(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGF_Convert565toBGR(Source, SourcePitch, Dest, DestPitch, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Convert565toBGR"_a));
        }
    }

    void Ex_OKGF_Convert565toBGRA(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGF_Convert565toBGRA(Source, SourcePitch, Dest, DestPitch, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Convert565toBGRA"_a));
        }
    }

    void Ex_OKGF_Convert_8888to565(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGF_Convert_8888to565(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Convert_8888to565"_a));
        }
    }

    void Ex_OKGR_ShrLight_16(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::int32_t Shift) {
        try {
            GR_Main::OKGR_ShrLight_16(Pixels, Pitch, Width, Height, Shift);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_ShrLight_16"_a));
        }
    }

    void Ex_OKGR_ShrLightMask_16(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height) {
        try {
            GR_Main::OKGR_ShrLightMask_16(Dest, DestPitch, Source, SourcePitch, Width, Height);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_ShrLightMask_16"_a));
        }
    }

    void Ex_OKGR_Light_BYTE(void* Pixels, std::int32_t PixelStride, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint8_t Alpha) {
        try {
            GR_Main::OKGR_Light_BYTE(Pixels, PixelStride, Pitch, Width, Height, Alpha);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Light_BYTE"_a));
        }
    }

    void Ex_OKGR_Circle_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Circle_DrawClip_WORD(Pixels, Pitch, X, Y, Radius, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Circle_DrawClip_WORD"_a));
        }
    }

    void Ex_OKGR_Circle_DrawClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Circle_DrawClip_BYTE(Pixels, Pitch, X, Y, Radius, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Circle_DrawClip_BYTE"_a));
        }
    }

    void Ex_OKGR_Circle_DrawFillClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Circle_DrawFillClip_WORD(Pixels, Pitch, X, Y, Radius, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Circle_DrawFillClip_WORD"_a));
        }
    }

    void Ex_OKGR_Circle_DrawFillClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Circle_DrawFillClip_BYTE(Pixels, Pitch, X, Y, Radius, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Circle_DrawFillClip_BYTE"_a));
        }
    }

    std::int32_t Ex_OKGR_Line_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, const WindowsSdk::TRect& Clip) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_Line_Clip(pas::Var<std::int32_t>(X1.address), pas::Var<std::int32_t>(Y1.address), pas::Var<std::int32_t>(X2.address), pas::Var<std::int32_t>(Y2.address), Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_Clip"_a));
        }
        return Result;
    }

    // Native implementation ignores Pixels/Pitch and draws into ScreenRenderBuffer.
    void DrawGradientLine16Clipped(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, WindowsSdk::TRect Clip) {
        TLineRasterizer16 Rasterizer{};
        if (GR_Main::Ex_OKGR_LineColor_Clip(pas::Var<std::int32_t>(&X1), pas::Var<std::int32_t>(&Y1), pas::Var<std::uint32_t>(&Color1), pas::Var<std::int32_t>(&X2), pas::Var<std::int32_t>(&Y2), pas::Var<std::uint32_t>(&Color2), Clip) != 0) {
            Rasterizer = LineRasterizer16;
            {
                std::uint32_t color2 = Color2;
                std::int32_t y2 = Y2;
                std::int32_t x2 = X2;
                std::uint32_t color1 = Color1;
                std::int32_t y1 = Y1;
                std::int32_t x1 = X1;
                std::int32_t pitchBytes = ScreenRenderBuffer->PitchBytes;
                void* pixels = ScreenRenderBuffer->GetPixels();
                Rasterizer(pixels, pitchBytes, x1, y1, color1, x2, y2, color2);
            }
        }
    }

    std::int32_t Ex_OKGR_LineColor_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::uint32_t> Color1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, pas::Var<std::uint32_t> Color2, const WindowsSdk::TRect& Clip) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_LineColor_Clip(pas::Var<std::int32_t>(X1.address), pas::Var<std::int32_t>(Y1.address), pas::Var<std::uint32_t>(Color1.address), pas::Var<std::int32_t>(X2.address), pas::Var<std::int32_t>(Y2.address), pas::Var<std::uint32_t>(Color2.address), Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in Ex_OKGR_LineColor_Clip"_a));
        }
        return Result;
    }

    void Ex_OKGR_Line_Draw_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color) {
        try {
            GR_Main::OKGR_Line_Draw_WORD(Pixels, Pitch, X1, Y1, X2, Y2, Color);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_Draw_WORD"_a));
        }
    }

    void Ex_OKGR_Line_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Line_DrawClip_WORD(Pixels, Pitch, X1, Y1, X2, Y2, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_DrawClip_WORD"_a));
        }
    }

    std::int32_t Ex_OKGR_Line_CopyToBuf_WORD(void* Dest, void* Source, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_Line_CopyToBuf_WORD(Dest, Source, Pitch, X1, Y1, X2, Y2);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_CopyToBuf_WORD"_a));
        }
        return Result;
    }

    // Restores one saved 16-bit pixel per rasterized line point; returns the pixel count.
    std::int32_t Ex_OKGR_Line_CopyFromBuf_WORD(void* Source, void* Dest, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2) {
        std::int32_t Result{};
        try {
            Result = GR_Main::OKGR_Line_CopyFromBuf_WORD(Source, Dest, Pitch, X1, Y1, X2, Y2);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_CopyFromBuf_WORD"_a));
        }
        return Result;
    }

    void Ex_OKGR_Line_DrawClip_Alpha_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Line_DrawClip_Alpha_16(Pixels, Pitch, X1, Y1, X2, Y2, Color, Alpha, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Line_DrawClip_Alpha_16"_a));
        }
    }

    void Ex_OKGR_AnimLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_AnimLine_Draw_16(Pixels, Pitch, X1, Y1, X2, Y2, Color, Phase, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AnimLine_Draw_16"_a));
        }
    }

    void Ex_OKGR_AnimShadowLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip, void* ShadowPixels, std::int32_t ShadowPitch) {
        try {
            GR_Main::OKGR_AnimShadowLine_Draw_16(Pixels, Pitch, X1, Y1, X2, Y2, Color, Phase, Clip, ShadowPixels, ShadowPitch);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_AnimShadowLine_Draw_16"_a));
        }
    }

    void Ex_OKGR_Alpha64Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Alpha64Trapezium_16(Pixels, Pitch, X1, Y1, X2, Y2, X3, X4, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Alpha64Trapezium_16"_a));
        }
    }

    void Ex_OKGR_Alpha128Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_Alpha128Trapezium_16(Pixels, Pitch, X1, Y1, X2, Y2, X3, X4, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_Alpha128Trapezium_16"_a));
        }
    }

    void Ex_OKGR_FillTrapezium_DWORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t X2, std::int32_t Y1, std::int32_t X3, std::int32_t X4, std::int32_t Y2, std::uint32_t Color, const WindowsSdk::TRect& Clip) {
        try {
            GR_Main::OKGR_FillTrapezium_DWORD(Pixels, Pitch, X1, X2, Y1, X3, X4, Y2, Color, Clip);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_FillTrapezium_DWORD"_a));
        }
    }

    void Ex_OKGF_Rescale(void* Dest, std::int32_t Width, std::int32_t Height, std::int32_t DestPitch, void* Source, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t SourcePitch, std::int32_t BytesPerPixel, std::int32_t Filter) {
        try {
            GR_Main::OKGF_Rescale(Dest, Width, Height, DestPitch, Source, SourceWidth, SourceHeight, SourcePitch, BytesPerPixel, Filter);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGF_Rescale"_a));
        }
    }

    void Ex_OKGR_F5_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_F5_DrawRGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_F5_DrawRGBA"_a));
        }
    }

    void Ex_OKGR_F6_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source) {
        try {
            GR_Main::OKGR_F6_DrawRGBA(Dest, Pitch, Source);
        } catch (...) {
            pas::raise(pas::make_exception<pas::Exception>("Error in OKGR_F6_DrawRGBA"_a));
        }
    }

    // 'libogg-0', 'libvorbis-0', 'libvorbisfile', 'matrixgame',
    // 'okgf', 'steam_ach', 'steam_api', 'xvidcore', 'zlib'.
    // Differences from the 1024x768 UI baseline; may be negative.
    // Identity function in this binary.
    std::int32_t GiScalePixels(std::int32_t Value) {
        return Value;
    }

    // Returns Value; AlternateValue is unused in this binary.
    std::int32_t GiScalePixelsEx(std::int32_t Value, std::int32_t AlternateValue) {
        return Value;
    }

    // Converts the borrowed UTF-16 message to AnsiString and raises Exception.
    void RaiseWideMessage(const pas::WideString& Message) {
        pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(Message)));
    }

    void DrawTransparentBuffer16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, WindowsSdk::TRect Clip, std::uint8_t HalfAlpha) {
        WindowsSdk::TRect InclusiveClip{};
        InclusiveClip.Left = Clip.Left;
        InclusiveClip.Top = Clip.Top;
        InclusiveClip.Right = Clip.Right - 1;
        InclusiveClip.Bottom = Clip.Bottom - 1;
        if (HalfAlpha) {
            GR_Main::Ex_OKGR_TransBuf_HADrawClip_16(Dest, Pitch, X, Y, Source, InclusiveClip);
        } else {
            GR_Main::Ex_OKGR_TransBuf_DrawClip_WORD(Dest, Pitch, X, Y, Source, InclusiveClip);
        }
    }

    void CopyPalettedBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, void* Palette, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip) {
        if (X >= Clip.Right || Y >= Clip.Bottom || X + Width - 1 < Clip.Left || Y + Height - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        GR_Main::Ex_OKGR_PalCopy_XY_XY_WORD(Dest, DestPitch, X, Y, Source, SourcePitch, SourceX, SourceY, Palette, Width, Height);
    }

    void CopyBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip, std::uint8_t UnusedOption) {
        if (X >= Clip.Right || Y >= Clip.Bottom || X + Width - 1 < Clip.Left || Y + Height - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        GR_Main::Ex_OKGR_Copy_XY_XY_WORD(Dest, DestPitch, X, Y, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void DrawAlphaBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, WindowsSdk::TRect Clip) {
        if (X >= Clip.Right || Y >= Clip.Bottom || X + Width - 1 < Clip.Left || Y + Height - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        GR_Main::Ex_OKGR_AlphaSimpleBuf_Draw_16(Dest, DestPitch, X, Y, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void ExpandPaletteToBgra(void* Dest, std::int32_t DestPitch, std::uint32_t Width, std::uint32_t Height, void* Source, std::int32_t SourcePitch, void* Palette) {
        std::uint32_t X{};
        std::uint32_t Y{};
        std::uint32_t Offset{};
        for (auto cpp_range = pas::for_to<std::uint32_t>(0u, Height - 1); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::uint32_t>(0u, Width - 1); cpp_range_2.next(X); ) {
                Offset = pas::shl(static_cast<std::int32_t>(*reinterpret_cast<std::uint8_t*>(static_cast<std::uint8_t*>(Source) + X)), 2);
                reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uint8_t*>(Dest) + X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))->B = *(&reinterpret_cast<GR_GraphBuf::PColorRGBA>(static_cast<std::uint8_t*>(Palette) + Offset)->B);
                *(&reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uint8_t*>(Dest) + X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))->G) = *(&reinterpret_cast<GR_GraphBuf::PColorRGBA>(static_cast<std::uint8_t*>(Palette) + Offset)->G);
                *(&reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uint8_t*>(Dest) + X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))->R) = reinterpret_cast<GR_GraphBuf::PColorRGBA>(static_cast<std::uint8_t*>(Palette) + Offset)->R;
                *(&reinterpret_cast<GR_GraphBuf::PColorBGRA>(static_cast<std::uint8_t*>(Dest) + X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorBGRA)))->A) = *(&reinterpret_cast<GR_GraphBuf::PColorRGBA>(static_cast<std::uint8_t*>(Palette) + Offset)->A);
            }
            Source = EC_Mem::AddPointerOffset(Source, SourcePitch);
            Dest = EC_Mem::AddPointerOffset(Dest, DestPitch);
        }
    }

    // Uses Data.StyleColor from Main.dat and the current pixel format. Missing entries use the defaults; malformed configured RGB text may raise.
    std::uint32_t GetStyleColorGI(pas::WideString StyleName, std::int32_t DefaultRed, std::int32_t DefaultGreen, std::int32_t DefaultBlue) {
        EC_BlockPar::TBlockParEC* Style{};
        pas::WideString ColorText{};
        if (GameDataConfig->CountBlocks(u"StyleColor"_wref.get()) > 0) {
            Style = GameDataConfig->GetBlock(u"StyleColor"sv);
            if (Style->CountParamsByPath(StyleName) > 0) {
                ColorText = Style->GetParamByPath(StyleName);
                std::int32_t extractDigitsToIntW = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 2, u","sv)));
                std::int32_t extractDigitsToIntW_2 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 1, u","sv)));
                std::int32_t extractDigitsToIntW_3 = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(ColorText), 0, u","sv)));
                return CurrentPixelFormat->PackRgb(extractDigitsToIntW_3, extractDigitsToIntW_2, extractDigitsToIntW);
            }
        }
        return CurrentPixelFormat->PackRgb(DefaultRed, DefaultGreen, DefaultBlue);
    }

    // Returns a complete opening <color=...> tag. Configured Data.StyleColor text is inserted verbatim; missing entries use the default RGB values.
    pas::WideString GetStyleColorTagGI(pas::WideString StyleName, std::int32_t DefaultRed, std::int32_t DefaultGreen, std::int32_t DefaultBlue) {
        pas::WideString Result{};
        EC_BlockPar::TBlockParEC* Style{};
        if (GameDataConfig->CountBlocks(u"StyleColor"_wref.get()) > 0) {
            Style = GameDataConfig->GetBlock(u"StyleColor"sv);
            if (Style->CountParamsByPath(StyleName) > 0) {
                Result = Style->GetParamByPath(StyleName);
                return pas::concat_wide({u"<color=", Style->GetParamByPath(StyleName), u">"});
            }
        }
        return pas::concat_wide({u"<color=", EC_Str::IntToWideString(DefaultRed), u",", EC_Str::IntToWideString(DefaultGreen), u",", EC_Str::IntToWideString(DefaultBlue), u">"});
    }

    void CopyGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip, std::uint8_t HalfAlpha, std::uint8_t UnusedOption) {
        if (X >= Clip.Right || Y >= Clip.Bottom || Source->Width + X - 1 < Clip.Left || Source->Height + Y - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        std::int32_t Width = Source->Width;
        std::int32_t Height = Source->Height;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        if (HalfAlpha) {
            void* pixels = Source->GetPixels();
            std::int32_t pitchBytes = Source->PitchBytes;
            GR_Main::Ex_OKGR_HACopy_XY_XY_16(Dest, DestPitch, X, Y, pixels, pitchBytes, SourceX, SourceY, Width, Height);
        } else {
            void* pixels_2 = Source->GetPixels();
            std::int32_t pitchBytes_2 = Source->PitchBytes;
            GR_Main::Ex_OKGR_Copy_XY_XY_WORD(Dest, DestPitch, X, Y, pixels_2, pitchBytes_2, SourceX, SourceY, Width, Height);
        }
    }

    void DrawAlphaGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip) {
        if (X >= Clip.Right || Y >= Clip.Bottom || Source->Width + X - 1 < Clip.Left || Source->Height + Y - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        std::int32_t Width = Source->Width;
        std::int32_t Height = Source->Height;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        {
            void* pixels = Source->GetPixels();
            std::int32_t pitchBytes = Source->PitchBytes;
            GR_Main::Ex_OKGR_AlphaSimpleBuf_Draw_16(Dest, DestPitch, X, Y, pixels, pitchBytes, SourceX, SourceY, Width, Height);
        }
    }

    void CopyTransparentGraphBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBuf::TGraphBufGR* Source, WindowsSdk::TRect Clip, std::uint16_t TransparentColor) {
        if (X >= Clip.Right || Y >= Clip.Bottom || Source->Width + X - 1 < Clip.Left || Source->Height + Y - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        std::int32_t Width = Source->Width;
        std::int32_t Height = Source->Height;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        {
            void* pixels = Source->GetPixels();
            std::int32_t pitchBytes = Source->PitchBytes;
            GR_Main::Ex_OKGR_CopyTrans_XY_XY_WORD(Dest, DestPitch, X, Y, pixels, pitchBytes, SourceX, SourceY, Width, Height, TransparentColor);
        }
    }

    void DrawPaletteAlphaBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBufPal::TGraphBufPalGR* Source, WindowsSdk::TRect Clip) {
        if (X >= Clip.Right || Y >= Clip.Bottom || X + Source->Width - 1 < Clip.Left || Y + Source->Height - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        std::int32_t Width = Source->Width;
        std::int32_t Height = Source->Height;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        GR_Main::Ex_OKGR_AlphaSimpleBufPalAlpha_Draw_16(Dest, DestPitch, X, Y, Source->Pixels, Source->PitchBytes, SourceX, SourceY, Width, Height, Source->Palette);
    }

    void BlendPaletteBuffer16Clipped(void* Dest, std::int32_t DestPitch, std::int32_t X, std::int32_t Y, GR_GraphBufPal::TGraphBufPalGR* Source, WindowsSdk::TRect Clip) {
        if (X >= Clip.Right || Y >= Clip.Bottom || X + Source->Width - 1 < Clip.Left || Y + Source->Height - 1 < Clip.Top) {
            return;
        }
        std::int32_t SourceX = 0;
        std::int32_t SourceY = 0;
        std::int32_t Width = Source->Width;
        std::int32_t Height = Source->Height;
        if (X + Width - 1 >= Clip.Right) {
            Width -= X + Width - 1 - (Clip.Right - 1);
        }
        if (Y + Height - 1 >= Clip.Bottom) {
            Height -= Y + Height - 1 - (Clip.Bottom - 1);
        }
        if (X < Clip.Left) {
            SourceX = Clip.Left - X;
            Width -= SourceX;
            X = Clip.Left;
        }
        if (Y < Clip.Top) {
            SourceY = Clip.Top - Y;
            Height -= SourceY;
            Y = Clip.Top;
        }
        void* SourcePixels = SourceY * Source->PitchBytes + static_cast<std::uint8_t*>(Source->Pixels) + SourceX;
        Dest = Y * DestPitch + static_cast<std::uint8_t*>(Dest) + X * 2;
        std::int32_t SourceSkip = Source->PitchBytes - Width;
        std::int32_t DestSkip = DestPitch - Width * 2;
        void* MulTable = GR_Main::Ex_OKGF_MulTable256x256();
        void* Palette = Source->Palette;
        std::int32_t ColumnCount = Width;
        // Native handwritten loops blend palette alpha into RGB565 or RGB555.
        if (CurrentPixelFormat->TotalChannelBits == 16) {
            BitmapPorts::BlendPalette16(SourcePixels, Dest, Width, Height, SourceSkip, DestSkip, Palette, MulTable, true);
        } else {
            BitmapPorts::BlendPalette16(SourcePixels, Dest, Width, Height, SourceSkip, DestSkip, Palette, MulTable, false);
        }
    }

    pas::WideString GetClipboardWideText() {
        pas::WideString Result{};
        // The game's paste actions run after Application.Handle is the main window.
        if (!WindowsSdk::OpenClipboard(Forms::Application->Handle)) {
            pas::raise(pas::make_exception<pas::Exception>("Cannot open clipboard"_a));
        }
        WindowsSdk::HGLOBAL Handle = WindowsSdk::GetClipboardData(WindowsSdk::CF_UNICODETEXT);
        {
            std::exception_ptr cpp_error{};
            try {
                if (Handle != 0) {
                    Result = static_cast<pas::WideString>(static_cast<char16_t*>(WindowsSdk::GlobalLock(Handle)));
                } else {
                    Result = pas::WideString();
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Handle != 0) {
                WindowsSdk::GlobalUnlock(Handle);
            }
            WindowsSdk::CloseClipboard();
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    void SetClipboardWideText(pas::WideString Text) {
        std::int32_t Size = Text.length() * 2 + 2;
        WindowsSdk::HGLOBAL Handle = WindowsSdk::GlobalAlloc(WindowsSdk::GMEM_MOVEABLE, Size);
        {
            void* globalLock = WindowsSdk::GlobalLock(Handle);
            void* cpp_arg = Text.pchar();
            Windows::CopyMemory(globalLock, cpp_arg, Size);
        }
        WindowsSdk::GlobalUnlock(Handle);
        WindowsSdk::OpenClipboard(0u);
        WindowsSdk::EmptyClipboard();
        WindowsSdk::SetClipboardData(WindowsSdk::CF_UNICODETEXT, Handle);
        WindowsSdk::CloseClipboard();
    }

    void WriteTextFileThreadSafe(pas::AnsiString FileName, pas::AnsiString Text) {
        pas::TextFile F{};
        if (SessionLogLock == nullptr) {
            SessionLogLock = pas::make_critical_section<pas::CriticalSection>();
        }
        pas::critical_enter(SessionLogLock);
        pas::text_assign(F, FileName, false);
        pas::text_open(F, 3, false);
        pas::text_writeln(F, Text, false);
        pas::text_close(F, false);
        pas::critical_leave(SessionLogLock);
    }

    // CRC32 of the C: volume serial and ANSI processor name. Native code ignores volume-query failure.
    std::uint32_t ComputeMachineFingerprintCRC() {
        pas::AnsiString ProcessorName{};
        std::uint32_t Serial{};
        std::uint32_t Flags{};
        WindowsSdk::GetVolumeInformationA(pas::literal_pointer("c:\\"), nullptr, 0u, &Serial, Flags, Flags, nullptr, 0u);
        ProcessorName = static_cast<pas::AnsiString>(GR_Main::ReadRegistryText(WindowsImports::HKEY_LOCAL_MACHINE, u"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"_w, u"ProcessorNameString"_w, pas::WideString()));
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddInt32(Serial);
        if (ProcessorName.length() > 0) {
            Buffer->AddBytes(static_cast<void*>(ProcessorName.data()), ProcessorName.length());
        }
        std::uint32_t Result = Buffer->ComputeCrc32();
        Buffer->Clear();
        pas::free(Buffer);
        return Result;
    }

    std::int32_t PAS_STDCALL OKGF_ZLib_Compress(void* Dest, void* Source, std::int32_t SourceSize, std::int32_t Mode) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("ZLib.dll", "OKGF_ZLib_Compress");
        return cpp_import(Dest, Source, SourceSize, Mode);
    }

    std::int32_t PAS_STDCALL OKGF_ZLib_UnCompress(void* Dest, std::int32_t DestCapacity, void* Source, std::int32_t SourceSize) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, std::int32_t, void*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("ZLib.dll", "OKGF_ZLib_UnCompress");
        return cpp_import(Dest, DestCapacity, Source, SourceSize);
    }

    std::int32_t PAS_STDCALL GlobalMemoryStatusEx(TMemoryStatusEx& Status) {
        using CppImport = std::int32_t (PAS_STDCALL *)(TMemoryStatusEx*);
        static const auto cpp_import = pas::win::load_import<CppImport>("kernel32", "GlobalMemoryStatusEx");
        return cpp_import(&Status);
    }

    void* OKGF_MulTable256x256() {
        using CppImport = void* (PAS_CDECL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_MulTable256x256");
        return cpp_import();
    }

    std::uint32_t OKGF_DXVersion() {
        using CppImport = std::uint32_t (PAS_CDECL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "DXVersion");
        return cpp_import();
    }

    EC_OKGF::POkgfReadContext OKGF_ReadStart_Buf(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height) {
        using CppImport = EC_OKGF::POkgfReadContext (PAS_CDECL *)(void*, std::int32_t, std::int32_t*, std::int32_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_ReadStart_Buf");
        return cpp_import(Source, SourceSize, &Width, &Height);
    }

    std::int32_t OKGF_Read(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t BytesPerPixel) {
        using CppImport = std::int32_t (PAS_CDECL *)(EC_OKGF::POkgfReadContext, void*, std::int32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Read");
        return cpp_import(Context, Pixels, PitchBytes, RedMask, GreenMask, BlueMask, AlphaMask, BytesPerPixel);
    }

    EC_OKGF::POkgfReadContext OKGF_ReadStartPal_Buf(void* Source, std::int32_t SourceSize, std::int32_t& Width, std::int32_t& Height, std::int32_t& PaletteCount, std::int32_t& BytesPerPixel) {
        using CppImport = EC_OKGF::POkgfReadContext (PAS_CDECL *)(void*, std::int32_t, std::int32_t*, std::int32_t*, std::int32_t*, std::int32_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_ReadStartPal_Buf");
        return cpp_import(Source, SourceSize, &Width, &Height, &PaletteCount, &BytesPerPixel);
    }

    std::int32_t OKGF_ReadPal(EC_OKGF::POkgfReadContext Context, void* Pixels, std::int32_t PitchBytes, GR_GraphBuf::PColorRGBA Palette) {
        using CppImport = std::int32_t (PAS_CDECL *)(EC_OKGF::POkgfReadContext, void*, std::int32_t, GR_GraphBuf::PColorRGBA);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_ReadPal");
        return cpp_import(Context, Pixels, PitchBytes, Palette);
    }

    std::int32_t OKGF_Write_PNG_File(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::int32_t HasAlpha, std::int32_t SwapRedBlue) {
        using CppImport = std::int32_t (PAS_CDECL *)(std::uint8_t*, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Write_PNG_File");
        return cpp_import(FileName, Pixels, PitchBytes, Width, Height, HasAlpha, SwapRedBlue);
    }

    std::int32_t OKGF_Write_BMP_File(std::uint8_t* FileName, void* Pixels, std::int32_t PitchBytes, std::int32_t BitsPerPixel, std::uint32_t RedMask, std::uint32_t GreenMask, std::uint32_t BlueMask, std::uint32_t AlphaMask, std::int32_t Width, std::int32_t Height) {
        using CppImport = std::int32_t (PAS_CDECL *)(std::uint8_t*, void*, std::int32_t, std::int32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Write_BMP_File");
        return cpp_import(FileName, Pixels, PitchBytes, BitsPerPixel, RedMask, GreenMask, BlueMask, AlphaMask, Width, Height);
    }

    void OKGR_AlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaBuf_Draw_RGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_TransAlphaBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransAlphaBuf_Draw_RGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_AlphaIndexed_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_Draw_RGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_AlphaIndexed_AlphaDraw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_AlphaDraw_RGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_TransBuf_Draw_RGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_Draw_RGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_TransBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_DrawClip_WORD");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    void OKGR_TransBuf_HADrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_HADrawClip_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    std::int32_t OKGR_TransBuf_Build_WORD(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest, std::uint16_t TransparentColor) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::uint16_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_Build_WORD");
        return cpp_import(Source, Pitch, Width, Height, Dest, TransparentColor);
    }

    std::int32_t OKGR_TransBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_BuildFromRGBA_16");
        return cpp_import(Source, Pitch, Width, Height, Dest);
    }

    void OKGR_TransAlphaBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransAlphaBuf_DrawClip_WORD");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    void OKGR_AlphaBuf_DrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaBuf_DrawClip_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    std::int32_t OKGR_TransAlphaBuf_BuildFromRGBA_16(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransAlphaBuf_BuildFromRGBA_16");
        return cpp_import(Source, Pitch, Width, Height, Dest);
    }

    std::int32_t OKGR_AlphaBuf_BuildFromRGBA(void* Source, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, void* Dest) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaBuf_BuildFromRGBA");
        return cpp_import(Source, Pitch, Width, Height, Dest);
    }

    void OKGR_AlphaSimpleBuf_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaSimpleBuf_Draw_16");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void OKGR_AlphaSimpleBufPalAlpha_Draw_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, GR_GraphBuf::PColorRGBA Palette) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, GR_GraphBuf::PColorRGBA);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaSimpleBufPalAlpha_Draw_16");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height, Palette);
    }

    void OKGR_MaskBuf_DrawClip_DWORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint32_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::uint32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_MaskBuf_DrawClip_DWORD");
        return cpp_import(Dest, Pitch, X, Y, Source, Color, &Clip);
    }

    void OKGR_MaskBuf_DrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_MaskBuf_DrawClip_WORD");
        return cpp_import(Dest, Pitch, X, Y, Source, Color, &Clip);
    }

    void OKGR_TransBuf_FillAlphaClip_RGBA(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint32_t Color) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_FillAlphaClip_RGBA");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip, Color);
    }

    void OKGR_TransBuf_FillAlphaClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint16_t Color) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*, std::uint16_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_TransBuf_FillAlphaClip_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip, Color);
    }

    void OKGR_AlphaIndexed_CopyDrawClip_WORD(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_CopyDrawClip_WORD");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    void OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_CopyDrawClip_Alpha_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip, Alpha);
    }

    void OKGR_AlphaIndexed_AlphaDrawClip_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_AlphaDrawClip_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip);
    }

    void OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(void* Dest, std::int32_t Pitch, std::int32_t X, std::int32_t Y, void* Source, const WindowsSdk::TRect& Clip, std::uint8_t Alpha) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, const WindowsSdk::TRect*, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16");
        return cpp_import(Dest, Pitch, X, Y, Source, &Clip, Alpha);
    }

    void* OKGR_RotateBuf_Build(std::int32_t Width, std::int32_t Height, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t CenterX, std::int32_t CenterY) {
        using CppImport = void* (PAS_CDECL *)(std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_Build");
        return cpp_import(Width, Height, SourceWidth, SourceHeight, CenterX, CenterY);
    }

    void OKGR_RotateBuf_Free(void* Buffer) {
        using CppImport = void (PAS_CDECL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_Free");
        return cpp_import(Buffer);
    }

    void OKGR_RotateBuf_Size(std::int32_t X, std::int32_t Y, std::uint8_t Angle, void* RotationMap, WindowsSdk::TRect& Bounds) {
        using CppImport = void (PAS_CDECL *)(std::int32_t, std::int32_t, std::uint8_t, void*, WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_Size");
        return cpp_import(X, Y, Angle, RotationMap, &Bounds);
    }

    void OKGR_RotateBuf_Draw_DWORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::uint8_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_Draw_DWORD");
        return cpp_import(Dest, DestPitch, Source, SourcePitch, CenterX, CenterY, Angle, RotationMap);
    }

    void OKGR_RotateBuf_Draw_BYTE(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height, std::uint8_t Angle, void* RotationMap) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::uint8_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_Draw_BYTE");
        return cpp_import(Dest, DestPitch, Source, SourcePitch, Width, Height, Angle, RotationMap);
    }

    void OKGR_RotateBuf_DrawTransClip_WORD(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t CenterX, std::int32_t CenterY, std::uint8_t Angle, void* RotationMap, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::uint8_t, void*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_RotateBuf_DrawTransClip_WORD");
        return cpp_import(Dest, DestPitch, Source, SourcePitch, CenterX, CenterY, Angle, RotationMap, &Clip);
    }

    void* OKGR_LightBuf_Create(std::int32_t Width, std::int32_t Height) {
        using CppImport = void* (PAS_CDECL *)(std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_Create");
        return cpp_import(Width, Height);
    }

    void OKGR_LightBuf_Destroy(void* Buffer) {
        using CppImport = void (PAS_CDECL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_Destroy");
        return cpp_import(Buffer);
    }

    void OKGR_LightBuf_SetSme(void* Buffer, std::int32_t X, std::int32_t Y) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_SetSme");
        return cpp_import(Buffer, X, Y);
    }

    void OKGR_LightBuf_Init(void* Buffer, std::uint8_t Value) {
        using CppImport = void (PAS_CDECL *)(void*, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_Init");
        return cpp_import(Buffer, Value);
    }

    void OKGR_LightBuf_LoadFromPalBuf(void* Buffer, void* Source, std::int32_t Width, std::int32_t Height, std::int32_t Pitch, GR_GraphBuf::PColorRGBA Palette) {
        using CppImport = void (PAS_CDECL *)(void*, void*, std::int32_t, std::int32_t, std::int32_t, GR_GraphBuf::PColorRGBA);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_LoadFromPalBuf");
        return cpp_import(Buffer, Source, Width, Height, Pitch, Palette);
    }

    void OKGR_LightBuf_Rotate(void* Dest, void* Source, void* RotationMap, std::uint8_t Angle) {
        using CppImport = void (PAS_CDECL *)(void*, void*, void*, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LightBuf_Rotate");
        return cpp_import(Dest, Source, RotationMap, Angle);
    }

    void* OKGR_Planet2_TemplBuild(void* Source, std::int32_t Pitch, std::int32_t Height, std::int32_t TextureWidth, std::int32_t TextureHeight, std::int32_t& ByteCount) {
        using CppImport = void* (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet2_TemplBuild");
        return cpp_import(Source, Pitch, Height, TextureWidth, TextureHeight, &ByteCount);
    }

    // The native Delphi binding preserves EAX, but the DLL defines no result contract.
    std::int32_t OKGR_Planet2_TemplDel(void* TemplateData) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet2_TemplDel");
        return cpp_import(TemplateData);
    }

    void OKGR_Planet2_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet2_DrawAndLight_32");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
    }

    void OKGR_Planet2_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet2_DrawAndLightClip_16");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, &Clip);
    }

    void OKGR_Planet3_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet3_DrawAndLight_32");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
    }

    void OKGR_Planet3_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet3_DrawAndLightClip_16");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, &Clip);
    }

    void OKGR_Planet4_DrawAndLight_32(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet4_DrawAndLight_32");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y);
    }

    void OKGR_Planet4_DrawAndLightClip_16(void* Dest, std::int32_t DestPitch, void* TemplateData, void* Source, std::int32_t SourcePitch, std::int32_t WidthMask, std::int32_t MapOffset, void* LightBuffer, void* Palette, std::int32_t X, std::int32_t Y, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, void*, std::int32_t, std::int32_t, std::int32_t, void*, void*, std::int32_t, std::int32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Planet4_DrawAndLightClip_16");
        return cpp_import(Dest, DestPitch, TemplateData, Source, SourcePitch, WidthMask, MapOffset, LightBuffer, Palette, X, Y, &Clip);
    }

    void OKGR_Copy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Copy_XY_XY_WORD");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void OKGR_PalCopy_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, void* Palette, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_PalCopy_XY_XY_WORD");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Palette, Width, Height);
    }

    void OKGR_CopyTrans_XY_XY_WORD(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint16_t TransparentColor) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_CopyTrans_XY_XY_WORD");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height, TransparentColor);
    }

    void OKGR_CopySingleBuf_XY_XY_WORD(void* Pixels, std::int32_t Pitch, std::int32_t DestX, std::int32_t DestY, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_CopySingleBuf_XY_XY_WORD");
        return cpp_import(Pixels, Pitch, DestX, DestY, SourceX, SourceY, Width, Height);
    }

    void OKGR_HACopy_XY_XY_16(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_HACopy_XY_XY_16");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void OKGR_StretchGdi_WORD(void* Dest, std::uint32_t Width, std::uint32_t Height, void* Source, std::uint32_t SourceWidth, std::uint32_t SourceHeight) {
        using CppImport = void (PAS_CDECL *)(void*, std::uint32_t, std::uint32_t, void*, std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_StretchGdi_WORD");
        return cpp_import(Dest, Width, Height, Source, SourceWidth, SourceHeight);
    }

    void OKGR_Fill_WORD(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint16_t Color) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::uint16_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Fill_WORD");
        return cpp_import(Pixels, Pitch, Width, Height, Color);
    }

    void OKGF_ConvertRGBto565(void* Source, void* Dest, std::int32_t Pitch, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, void*, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_ConvertRGBto565");
        return cpp_import(Source, Dest, Pitch, Width, Height);
    }

    void OKGF_Convert565toRGB(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Convert565toRGB");
        return cpp_import(Source, SourcePitch, Dest, DestPitch, Width, Height);
    }

    void OKGF_Convert565toBGR(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Convert565toBGR");
        return cpp_import(Source, SourcePitch, Dest, DestPitch, Width, Height);
    }

    void OKGF_Convert565toBGRA(void* Source, std::int32_t SourcePitch, void* Dest, std::int32_t DestPitch, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Convert565toBGRA");
        return cpp_import(Source, SourcePitch, Dest, DestPitch, Width, Height);
    }

    void OKGF_Convert_8888to565(void* Dest, std::int32_t DestPitch, std::int32_t DestX, std::int32_t DestY, void* Source, std::int32_t SourcePitch, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Convert_8888to565");
        return cpp_import(Dest, DestPitch, DestX, DestY, Source, SourcePitch, SourceX, SourceY, Width, Height);
    }

    void OKGR_ShrLight_16(void* Pixels, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::int32_t Shift) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_ShrLight_16");
        return cpp_import(Pixels, Pitch, Width, Height, Shift);
    }

    void OKGR_ShrLightMask_16(void* Dest, std::int32_t DestPitch, void* Source, std::int32_t SourcePitch, std::int32_t Width, std::int32_t Height) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_ShrLightMask_16");
        return cpp_import(Dest, DestPitch, Source, SourcePitch, Width, Height);
    }

    void OKGR_Light_BYTE(void* Pixels, std::int32_t PixelStride, std::int32_t Pitch, std::int32_t Width, std::int32_t Height, std::uint8_t Alpha) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Light_BYTE");
        return cpp_import(Pixels, PixelStride, Pitch, Width, Height, Alpha);
    }

    void OKGR_Circle_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Circle_DrawClip_WORD");
        return cpp_import(Pixels, Pitch, X, Y, Radius, Color, &Clip);
    }

    void OKGR_Circle_DrawClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint8_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Circle_DrawClip_BYTE");
        return cpp_import(Pixels, Pitch, X, Y, Radius, Color, &Clip);
    }

    void OKGR_Circle_DrawFillClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Circle_DrawFillClip_WORD");
        return cpp_import(Pixels, Pitch, X, Y, Radius, Color, &Clip);
    }

    void OKGR_Circle_DrawFillClip_BYTE(void* Pixels, std::int32_t Pitch, std::int32_t X, std::int32_t Y, std::int32_t Radius, std::uint8_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint8_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Circle_DrawFillClip_BYTE");
        return cpp_import(Pixels, Pitch, X, Y, Radius, Color, &Clip);
    }

    void OKGR_PixelAlpha_16(void* Pixel, std::uint16_t Color, std::uint8_t Alpha) {
        using CppImport = void (PAS_CDECL *)(void*, std::uint16_t, std::uint8_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_PixelAlpha_16");
        return cpp_import(Pixel, Color, Alpha);
    }

    std::int32_t OKGR_Line_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, const WindowsSdk::TRect& Clip) {
        using CppImport = std::int32_t (PAS_CDECL *)(std::int32_t*, std::int32_t*, std::int32_t*, std::int32_t*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_Clip");
        return cpp_import(static_cast<std::int32_t*>(X1.address), static_cast<std::int32_t*>(Y1.address), static_cast<std::int32_t*>(X2.address), static_cast<std::int32_t*>(Y2.address), &Clip);
    }

    std::int32_t OKGR_LineColor_Clip(pas::Var<std::int32_t> X1, pas::Var<std::int32_t> Y1, pas::Var<std::uint32_t> Color1, pas::Var<std::int32_t> X2, pas::Var<std::int32_t> Y2, pas::Var<std::uint32_t> Color2, const WindowsSdk::TRect& Clip) {
        using CppImport = std::int32_t (PAS_CDECL *)(std::int32_t*, std::int32_t*, std::uint32_t*, std::int32_t*, std::int32_t*, std::uint32_t*, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_LineColor_Clip");
        return cpp_import(static_cast<std::int32_t*>(X1.address), static_cast<std::int32_t*>(Y1.address), static_cast<std::uint32_t*>(Color1.address), static_cast<std::int32_t*>(X2.address), static_cast<std::int32_t*>(Y2.address), static_cast<std::uint32_t*>(Color2.address), &Clip);
    }

    void OKGR_Line_Draw_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_Draw_WORD");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, Color);
    }

    void OKGR_Line_DrawClip_WORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_DrawClip_WORD");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, Color, &Clip);
    }

    std::int32_t OKGR_Line_CopyToBuf_WORD(void* Dest, void* Source, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_CopyToBuf_WORD");
        return cpp_import(Dest, Source, Pitch, X1, Y1, X2, Y2);
    }

    std::int32_t OKGR_Line_CopyFromBuf_WORD(void* Source, void* Dest, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2) {
        using CppImport = std::int32_t (PAS_CDECL *)(void*, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_CopyFromBuf_WORD");
        return cpp_import(Source, Dest, Pitch, X1, Y1, X2, Y2);
    }

    void OKGR_Line_DrawClip_Alpha_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::uint8_t Alpha, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, std::uint8_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Line_DrawClip_Alpha_16");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, Color, Alpha, &Clip);
    }

    void OKGR_AnimLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, std::int32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AnimLine_Draw_16");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, Color, Phase, &Clip);
    }

    void OKGR_AnimShadowLine_Draw_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::uint16_t Color, std::int32_t Phase, const WindowsSdk::TRect& Clip, void* ShadowPixels, std::int32_t ShadowPitch) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, std::int32_t, const WindowsSdk::TRect*, void*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_AnimShadowLine_Draw_16");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, Color, Phase, &Clip, ShadowPixels, ShadowPitch);
    }

    void OKGR_Alpha64Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Alpha64Trapezium_16");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, X3, X4, Color, &Clip);
    }

    void OKGR_Alpha128Trapezium_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::int32_t X2, std::int32_t Y2, std::int32_t X3, std::int32_t X4, std::uint16_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint16_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_Alpha128Trapezium_16");
        return cpp_import(Pixels, Pitch, X1, Y1, X2, Y2, X3, X4, Color, &Clip);
    }

    void OKGR_FillTrapezium_DWORD(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t X2, std::int32_t Y1, std::int32_t X3, std::int32_t X4, std::int32_t Y2, std::uint32_t Color, const WindowsSdk::TRect& Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, const WindowsSdk::TRect*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_FillTrapezium_DWORD");
        return cpp_import(Pixels, Pitch, X1, X2, Y1, X3, X4, Y2, Color, &Clip);
    }

    void OKGF_Rescale(void* Dest, std::int32_t Width, std::int32_t Height, std::int32_t DestPitch, void* Source, std::int32_t SourceWidth, std::int32_t SourceHeight, std::int32_t SourcePitch, std::int32_t BytesPerPixel, std::int32_t Filter) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Rescale");
        return cpp_import(Dest, Width, Height, DestPitch, Source, SourceWidth, SourceHeight, SourcePitch, BytesPerPixel, Filter);
    }

    void OKGR_F5_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_F5_DrawRGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    void OKGR_F6_DrawRGBA(void* Dest, std::int32_t Pitch, void* Source) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGR_F6_DrawRGBA");
        return cpp_import(Dest, Pitch, Source);
    }

    // Cdecl triangle ABI verified at native caller.
    void OKGF_Triangle_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2, std::int32_t X3, std::int32_t Y3, std::uint32_t Color3, WindowsSdk::PRect Clip) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t, WindowsSdk::PRect);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_Triangle_16");
        return cpp_import(Pixels, Pitch, X1, Y1, Color1, X2, Y2, Color2, X3, Y3, Color3, Clip);
    }

    // Cdecl gradient-line ABI verified at native caller.
    void OKGF_LineIp_16(void* Pixels, std::int32_t Pitch, std::int32_t X1, std::int32_t Y1, std::uint32_t Color1, std::int32_t X2, std::int32_t Y2, std::uint32_t Color2) {
        using CppImport = void (PAS_CDECL *)(void*, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("okgf.dll", "OKGF_LineIp_16");
        return cpp_import(Pixels, Pitch, X1, Y1, Color1, X2, Y2, Color2);
    }

    void TCCInterface_Create(TCCInterface* Self) {
        pas::object_create(Self);
        Self->Lock = pas::make_critical_section<pas::CriticalSection>();
        SystemImports::Randomize();
        Self->Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Self->CommitSnapshot(TCCInterface::CreateEmptySnapshot());
    }

    void TCCInterface_Destroy(TCCInterface* Self) {
        pas::free(Self->Lock);
        pas::free(Self->Buffer);
        Self->ClearSnapshots();
        pas::object_destroy(Self);
    }

    void TCCInterface::Reset() {
        pas::critical_enter(Lock);
        Buffer->Clear();
        ClearSnapshots();
        CommitSnapshot(TCCInterface::CreateEmptySnapshot());
        pas::critical_leave(Lock);
    }

    PCCSnapshot TCCInterface::GetSnapshot() {
        PCCSnapshot Entry = SnapshotHead;
        while (Entry->Prev == nullptr || Entry->Next->Prev == Entry) {
            Entry = Entry->Next;
        }
        return Entry->Next;
    }

    PCCSnapshot TCCInterface::CreateDecoy() {
        PCCSnapshot Result{};
        pas::new_value(Result);
        Result->ResourceChecksumFailed = pas::random(11, &System::RandSeed) > 9;
        Result->TamperDetected = pas::random(11, &System::RandSeed) > 8;
        Result->Flag0A = pas::random(11, &System::RandSeed) > 9;
        Result->ProtectedStateXorSeed = pas::random(2000000000, &System::RandSeed);
        Result->Value10 = pas::random(1000, &System::RandSeed);
        Result->IntegrityStatus = pas::random(1000, &System::RandSeed);
        {
            std::int32_t cpp_left = pas::random(11, &System::RandSeed) > 8;
            Result->IntegrityError = cpp_left * pas::random(1000, &System::RandSeed);
        }
        Result->IntegrityChecksum = pas::random(2000000000, &System::RandSeed);
        Result->IntegrityChecksum1 = pas::random(2000000000, &System::RandSeed);
        Result->IntegrityChecksum2 = pas::random(2000000000, &System::RandSeed);
        Result->EncodedCheatPoints = pas::random(2000000000, &System::RandSeed);
        Result->EditableStateApplied = pas::random(11, &System::RandSeed) > 9;
        return Result;
    }

    PCCSnapshot TCCInterface::CreateEmptySnapshot() {
        PCCSnapshot Result{};
        pas::new_value(Result);
        Result->ResourceChecksumFailed = false;
        Result->TamperDetected = false;
        Result->Flag0A = false;
        Result->ProtectedStateXorSeed = 0;
        Result->Value10 = 0;
        Result->IntegrityStatus = 0;
        Result->IntegrityError = 0;
        Result->IntegrityChecksum = 0u;
        Result->IntegrityChecksum1 = 0u;
        Result->IntegrityChecksum2 = 0u;
        Result->EncodedCheatPoints = 0;
        Result->EditableStateApplied = false;
        return Result;
    }

    PCCSnapshot TCCInterface::CopySnapshot(PCCSnapshot Source) {
        PCCSnapshot Result{};
        pas::new_value(Result);
        Result->ResourceChecksumFailed = Source->ResourceChecksumFailed;
        Result->TamperDetected = Source->TamperDetected;
        Result->Flag0A = Source->Flag0A;
        Result->ProtectedStateXorSeed = Source->ProtectedStateXorSeed;
        Result->Value10 = Source->Value10;
        Result->IntegrityStatus = Source->IntegrityStatus;
        Result->IntegrityError = Source->IntegrityError;
        Result->IntegrityChecksum = Source->IntegrityChecksum;
        Result->IntegrityChecksum1 = Source->IntegrityChecksum1;
        Result->IntegrityChecksum2 = Source->IntegrityChecksum2;
        Result->EncodedCheatPoints = Source->EncodedCheatPoints;
        Result->EditableStateApplied = Source->EditableStateApplied;
        return Result;
    }

    void TCCInterface::CommitSnapshot(PCCSnapshot Snapshot) {
        PCCSnapshot Entry{};
        PCCSnapshot Added{};
        std::int32_t OldPrefixCount = 0;
        if (SnapshotHead != nullptr) {
            Entry = GetSnapshot();
            Added = SnapshotHead;
            while (Entry != Added) {
                ++OldPrefixCount;
                Added = Added->Next;
            }
        }
        std::int32_t RingCount = pas::random(3, &System::RandSeed) + 3;
        std::int32_t PrefixCount = pas::random(3, &System::RandSeed) + 3;
        while (PrefixCount == OldPrefixCount) {
            PrefixCount = pas::random(3, &System::RandSeed) + 3;
        }
        Entry = Snapshot;
        Added = nullptr;
        while (RingCount > 0) {
            --RingCount;
            Added = TCCInterface::CreateDecoy();
            Added->Next = Entry;
            Entry->Prev = Added;
            Entry = Added;
        }
        Added->Prev = Snapshot;
        Snapshot->Next = Added;
        Entry = Snapshot;
        while (PrefixCount > 0) {
            --PrefixCount;
            Added = TCCInterface::CreateDecoy();
            Added->Next = Entry;
            Entry->Prev = Added;
            Entry = Added;
        }
        Added->Prev = nullptr;
        if (SnapshotHead != nullptr) {
            ClearSnapshots();
        }
        SnapshotHead = Added;
    }

    void TCCInterface::ClearSnapshots() {
        PCCSnapshot Next{};
        PCCSnapshot Snapshot = GetSnapshot();
        SnapshotHead = nullptr;
        PCCSnapshot Entry = Snapshot->Next;
        while (Entry != Snapshot) {
            Next = Entry->Next;
            pas::dispose(Entry);
            Entry = Next;
        }
        while (Entry != nullptr) {
            Next = Entry->Prev;
            pas::dispose(Entry);
            Entry = Next;
        }
    }

    std::uint8_t TCCInterface::GetResourceChecksumFailed() {
        pas::critical_enter(Lock);
        std::uint8_t Result = GetSnapshot()->ResourceChecksumFailed;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetResourceChecksumFailed(std::uint8_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->ResourceChecksumFailed = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    // Reads the protected tamper flag under Lock. SetMoney checks its encoded mirror and rereads after Sleep(1); NextDay similarly checks ammunition.
    std::uint8_t TCCInterface::GetTamperDetected() {
        pas::critical_enter(Lock);
        std::uint8_t Result = GetSnapshot()->TamperDetected;
        pas::critical_leave(Lock);
        return Result;
    }

    // Copies the current snapshot, replaces byte 9 and commits it under Lock.
    void TCCInterface::SetTamperDetected(std::uint8_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->TamperDetected = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    // Protected flag consumed by the dormant galaxy checksum; purpose unresolved.
    std::uint8_t TCCInterface::GetFlag0A() {
        pas::critical_enter(Lock);
        std::uint8_t Result = GetSnapshot()->Flag0A;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetFlag0A(std::uint8_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->Flag0A = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    // Read under Lock; used for the score-mod cheat warning.
    std::uint8_t TCCInterface::GetEditableStateApplied() {
        pas::critical_enter(Lock);
        std::uint8_t Result = GetSnapshot()->EditableStateApplied;
        pas::critical_leave(Lock);
        return Result;
    }

    // Replaces the protected score-mod warning flag.
    void TCCInterface::SetEditableStateApplied(std::uint8_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->EditableStateApplied = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    // Protected-state XOR seed; zero denotes restored state. Rangers reads it after the screen loop.
    std::int32_t TCCInterface::GetProtectedStateXorSeed() {
        pas::critical_enter(Lock);
        std::int32_t Result = GetSnapshot()->ProtectedStateXorSeed;
        pas::critical_leave(Lock);
        return Result;
    }

    // Protected-state XOR seed; zero denotes restored state.
    void TCCInterface::SetProtectedStateXorSeed(std::int32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->ProtectedStateXorSeed = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    // Protected payload with unresolved purpose.
    std::int32_t TCCInterface::GetValue10() {
        pas::critical_enter(Lock);
        std::int32_t Result = GetSnapshot()->Value10;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetValue10(std::int32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->Value10 = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::int32_t TCCInterface::GetIntegrityStatus() {
        pas::critical_enter(Lock);
        std::int32_t Result = GetSnapshot()->IntegrityStatus;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetIntegrityStatus(std::int32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->IntegrityStatus = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::int32_t TCCInterface::GetIntegrityError() {
        pas::critical_enter(Lock);
        std::int32_t Result = GetSnapshot()->IntegrityError;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetIntegrityError(std::int32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->IntegrityError = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::uint32_t TCCInterface::GetIntegrityChecksum() {
        pas::critical_enter(Lock);
        std::uint32_t Result = GetSnapshot()->IntegrityChecksum;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetIntegrityChecksum(std::uint32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->IntegrityChecksum = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::uint32_t TCCInterface::GetIntegrityChecksum1() {
        pas::critical_enter(Lock);
        std::uint32_t Result = GetSnapshot()->IntegrityChecksum1;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetIntegrityChecksum1(std::uint32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->IntegrityChecksum1 = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::uint32_t TCCInterface::GetIntegrityChecksum2() {
        pas::critical_enter(Lock);
        std::uint32_t Result = GetSnapshot()->IntegrityChecksum2;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetIntegrityChecksum2(std::uint32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->IntegrityChecksum2 = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    std::int32_t TCCInterface::GetEncodedCheatPoints() {
        pas::critical_enter(Lock);
        std::int32_t Result = GetSnapshot()->EncodedCheatPoints;
        pas::critical_leave(Lock);
        return Result;
    }

    void TCCInterface::SetEncodedCheatPoints(std::int32_t Value) {
        pas::critical_enter(Lock);
        PCCSnapshot Snapshot = TCCInterface::CopySnapshot(GetSnapshot());
        Snapshot->EncodedCheatPoints = Value;
        CommitSnapshot(Snapshot);
        pas::critical_leave(Lock);
    }

    void TCCInterface::p_destroy() {
        GR_Main::TCCInterface_Destroy(this);
    }

} // namespace GR_Main
