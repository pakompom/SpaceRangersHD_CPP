#include "layout/Robot.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_Music.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheFont.hpp"
#include "units/EC_HsFile.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/GlobalsV.hpp"
#include "units/Robot.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aGalaxy.hpp"
#include "units/fPanelLoad.hpp"

namespace Robot {
    Types::TPoint MeasureRobotTextSize(std::int32_t& Width, std::int32_t& Wrap, std::int32_t& TopAdjustment, EC_Str::TStringsEC*& Lines, EC_CacheFont::TCFontEC*& Font, EC_Str::TStringsEC*& WrappedLines);

    Robot::PRobotInterfacePrefix RobotInterface = nullptr;

    Robot::TRobotCallbacks RobotCallbacks{};

    pas::Array<std::int32_t, 0, 5> RobotBattleStatistics{};

    pas::DynArray<std::int32_t> SupportedMultiSamples{};

    Robot::TRobotDisplaySettingsPrefix RobotSettings = Robot::TRobotDisplaySettingsPrefix{.ShowStencilShadows = true, .ShowProjShadows = true, .LandTexturesGloss = true, .ObjTexturesGloss = true, .Sky = static_cast<std::uint8_t>(2), .RobotShadow = static_cast<std::uint8_t>(1), .ColorDepth = 32, .ScreenWidth = 1024, .ScreenHeight = 768, .Brightness = 0.5f, .Contrast = 0.5f, .MaxDistance = 0.0f};

    std::uint8_t RobotSound = true;

    std::uint8_t RobotMusic = true;

    std::uint8_t RobotVSync = false;

    std::int32_t RobotFSAASamples = 0;

    std::int32_t RobotAnisotropy = 0;

    std::int32_t RobotMaxDistance = 0;

    std::int32_t SupportedMultiSampleCount = 0;

    std::uint32_t MaximumAnisotropy = 0u;

    std::uint32_t RobotModule = 0u;

    std::int32_t GetRobotMultiSampleIndex() {
        std::int32_t I{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SupportedMultiSampleCount - 1); cpp_range.next(I); ) {
            if (SupportedMultiSamples[I] == RobotFSAASamples) {
                return I;
            }
        }
        return 0;
    }

    void InitializeRobotRuntime() {
        TGetRobotInterface GetInterface{};
        pas::WideString OverrideName{};
        PRobotCallbacks Callbacks{};
        Robot::FinalizeRobotRuntime();
        if (GR_Main::IsInstallFeatureEnabled(u"Robot"_wref.get())) {
            Callbacks = &RobotCallbacks;
            pas::fill_memory(Callbacks, static_cast<std::int32_t>(sizeof(TRobotCallbacks)), static_cast<std::uint8_t>(0));
            RobotCallbacks.PlaySound = TRobotPlaySound(RobotPlaySound);
            RobotCallbacks.CreateSound = TRobotCreateSound(RobotCreateSound);
            RobotCallbacks.FreeSound = TRobotSoundAction(RobotFreeSound);
            RobotCallbacks.StartSound = TRobotSoundAction(RobotStartSound);
            RobotCallbacks.IsSoundPlaying = TRobotSoundQuery(RobotIsSoundPlaying);
            RobotCallbacks.SetSoundVolume = TRobotSoundSetValue(RobotSetSoundVolume);
            RobotCallbacks.SetSoundPan = TRobotSoundSetValue(RobotSetSoundPan);
            RobotCallbacks.GetSoundVolume = TRobotSoundGetValue(RobotGetSoundVolume);
            RobotCallbacks.GetSoundPan = TRobotSoundGetValue(RobotGetSoundPan);
            RobotCallbacks.RenderText = TRobotRenderText(RobotRenderText);
            RobotCallbacks.FreeText = TRobotFreeText(RobotFreeText);
            RobotCallbacks.SetProgress = TRobotProgress(RobotSetProgress);
            RobotCallbacks.PlayMusic = TRobotAction(RobotPlayMusic);
            RobotCallbacks.ReleaseTextures = TRobotAction(RobotReleaseTextures);
            RobotCallbacks.GetMusicVolume = TRobotGetVolume(RobotGetMusicVolume);
            RobotCallbacks.SetMusicVolume = TRobotSetVolume(RobotSetMusicVolume);
            if (GR_Main::DirectXVersion >= 0x00090000) {
                if (GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"_wref.get())->CountParams(u"MatrixOverride"_wref.get()) > 0) {
                    OverrideName = GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"_wref.get())->GetParam(u"MatrixOverride"_wref.get());
                    RobotModule = WindowsSdk::LoadLibraryW(OverrideName.pchar());
                } else {
                    RobotModule = WindowsImports::LoadLibrary(pas::literal_pointer("MatrixGame.dll"));
                }
                if (RobotModule != 0) {
                    GetInterface = pas::callback_from_address<TGetRobotInterface>(WindowsImports::GetProcAddress(RobotModule, pas::literal_pointer("GetRobotInterface")));
                    if (GetInterface() == nullptr) {
                        WindowsImports::FreeLibrary(RobotModule);
                        RobotModule = 0u;
                    } else {
                        RobotInterface = GetInterface();
                        RobotInterface->Initialize(&RobotCallbacks);
                        GR_Main::AppendLogLineThreadSafe("Load MatrixGame.dll .... ok"_a);
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Robot.Support()=", EC_Str::IntToWideString(RobotInterface->Support())})));
                    }
                }
            }
        }
    }

    void FinalizeRobotRuntime() {
        if (RobotInterface != nullptr) {
            RobotInterface->Finalize();
            RobotInterface = nullptr;
            if (RobotModule != 0) {
                WindowsImports::FreeLibrary(RobotModule);
                RobotModule = 0u;
            }
        }
    }

    std::int32_t FRun(const pas::WideString& MapName, const pas::WideString& StartText, const pas::WideString& WinText, const pas::WideString& LossText, const pas::WideString& TerronName) {
        std::int32_t Result{};
        pas::WideString ConfigOverride{};
        pas::AnsiString SavedDirectory{};
        pas::AnsiString RobotDirectory{};
        float SavedSoundVolume{};
        float SavedMusicVolume{};
        std::uint8_t Failed{};
        GI_MessageLoop::TCursorStateGI CursorState{};
        GR_Main::TMemoryStatusEx Memory{};
        if (GlobalsV::MusicEnabled && GR_Main::MusicManager->HasSelectedMusic()) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        GR_Main::AppendLogLineThreadSafe("Preparing to start planetary battle"_a);
        SavedDirectory = SysUtilsImports::GetCurrentDir();
        EC_HsFile::LooseFileRoot = pas::concat_ansi({SavedDirectory, "\\"});
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->CaptureCursorState(&CursorState);
                if (GR_Main::InstallConfig->CountParams(u"RobotPath"_wref.get()) > 0) {
                    SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(GR_Main::InstallConfig->GetParam(u"RobotPath"_wref.get())));
                    RobotDirectory = SysUtilsImports::GetCurrentDir();
                }
                Robot::RobotSetProgress(0.0f);
                Memory.Length = static_cast<std::int32_t>(sizeof(GR_Main::TMemoryStatusEx));
                GR_Main::GlobalMemoryStatusEx(Memory);
                if (GlobalsV::MemorySnapshotActive || aGalaxy::Galaxy != nullptr || pas::shr(static_cast<std::int64_t>(Memory.AvailPhys), 30) <= 0) {
                    GR_Main::GlobalCache->TrimToBudget(0);
                }
                Result = 1;
                SavedSoundVolume = GlobalsV::SoundVolume;
                SavedMusicVolume = GlobalsV::MusicVolume;
                GlobalsV::SoundVolume = GlobalsV::RobotSoundVolume;
                GlobalsV::MusicVolume = GlobalsV::RobotMusicVolume;
                RobotSettings.Brightness = pas::real_divide(GR_Main::RobotBrightness, 2.0L) + 0.5L;
                RobotSettings.Contrast = pas::real_divide(GR_Main::RobotContrast, 2.0L) + 0.5L;
                RobotSettings.ColorDepth = 32;
                if (GR_Main::Direct3DPresentParameters.Windowed) {
                    if (GR_Main::AlternateViewportEnabled) {
                        RobotSettings.ScreenWidth = GR_Main::PresentationWidth;
                        RobotSettings.ScreenHeight = GR_Main::PresentationHeight;
                    } else {
                        RobotSettings.ScreenWidth = GR_Main::GameScreenWidth;
                        RobotSettings.ScreenHeight = GR_Main::GameScreenHeight;
                    }
                    RobotSettings.RefreshRate = 0;
                } else if (GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].Width == 0) {
                    if (GR_Main::AlternateViewportEnabled) {
                        RobotSettings.ScreenWidth = GR_Main::PresentationWidth;
                        RobotSettings.ScreenHeight = GR_Main::PresentationHeight;
                    } else {
                        RobotSettings.ScreenWidth = GR_Main::GameScreenWidth;
                        RobotSettings.ScreenHeight = GR_Main::GameScreenHeight;
                    }
                    RobotSettings.RefreshRate = GR_Main::GameDisplayModes[GR_Main::SelectedGameDisplayMode].RefreshRate;
                } else {
                    RobotSettings.ScreenWidth = GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].Width;
                    RobotSettings.ScreenHeight = GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].Height;
                    RobotSettings.RefreshRate = GR_Main::RobotDisplayModes[GR_Main::SelectedRobotDisplayMode].RefreshRate;
                }
                RobotSettings.VSync = RobotVSync;
                RobotSettings.FSAASamples = RobotFSAASamples;
                RobotSettings.Anisotropy = RobotAnisotropy;
                RobotSettings.MaxDistance = pas::real_divide(RobotMaxDistance, 1.0E+2L);
                RobotBattleStatistics[0] = 0;
                RobotBattleStatistics[1] = 0;
                RobotBattleStatistics[2] = 0;
                RobotBattleStatistics[3] = 0;
                RobotBattleStatistics[4] = 0;
                RobotBattleStatistics[5] = 0;
                RobotSettings.Direct3D = static_cast<void*>(GR_Main::Direct3D.get());
                RobotSettings.Device = static_cast<void*>(GR_Main::Direct3DDevice.get());
                GR_Main::AppendLogLineThreadSafe("Starting planetary battle"_a);
                GR_Main::RobotBattleActive = true;
                Failed = false;
                try {
                    if (RobotInterface != nullptr) {
                        if (GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"_wref.get())->CountParams(u"CfgOverride"_wref.get()) > 0) {
                            ConfigOverride = GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"_wref.get())->GetParam(u"CfgOverride"_wref.get());
                        } else {
                            ConfigOverride = GR_Main::LanguageInstallConfig->GetParam(u"Lang"_wref.get());
                        }
                        Result = RobotInterface->Run(System::HInstance, GR_Main::MainWindowHandle, MapName.pchar(), &RobotSettings, ConfigOverride.pchar(), StartText.pchar(), WinText.pchar(), LossText.pchar(), TerronName.pchar(), &RobotBattleStatistics[0]);
                    }
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                        Failed = true;
                    } else {
                        throw;
                    }
                }
                if (Result >= 100) {
                    Result -= 100;
                    if (Result >= 1) {
                        GR_Main::AppendLogLineThreadSafe("Warning! There was an error on exit from planetary battle!"_a);
                    }
                    if (Result == 0) {
                        Failed = true;
                    }
                }
                GR_Main::RobotBattleActive = false;
                GR_Main::AppendLogLineThreadSafe("Planetary battle finished"_a);
                GR_Main::GR_DXReset();
                WindowsSdk::SetWindowTextA(GR_Main::MainWindowHandle, pas::literal_pointer("Rangers"));
                GR_Main::ApplyGammaRamp(GR_Main::DisplayBrightness, GR_Main::DisplayContrast);
                GlobalsV::SoundVolume = SavedSoundVolume;
                GlobalsV::MusicVolume = SavedMusicVolume;
                GlobalsV::MusicVolumeScale = 1.0f;
                if (Result == 0 && static_cast<std::uint8_t>(Failed ^ 1)) {
                    GR_Main::ExitScreenLoop = true;
                    GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                    reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
                    Result = 0;
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RestoreCursorState(&CursorState);
                if (GR_Main::ShowSystemMouse) {
                    while (WindowsSdk::ShowCursor(-1) < 0) {
                    }
                } else {
                    while (WindowsSdk::ShowCursor(0) >= 0) {
                    }
                }
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->InvalidateViewport();
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            SysUtilsImports::SetCurrentDir(SavedDirectory);
            EC_HsFile::LooseFileRoot = pas::AnsiString();
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        GR_Main::AppendLogLineThreadSafe("Cleanup after planetary battle finished"_a);
        if (GlobalsV::MusicEnabled && GR_Main::MusicManager->HasSelectedMusic()) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        if (Failed) {
            pas::raise(pas::make_exception<pas::Exception>("Error in GIRobot.FRun"_a));
        }
        return Result;
    }

    void PAS_STDCALL RobotPlaySound(char16_t* Name) {
        GR_Main::SoundManager->PlaySound(static_cast<pas::WideString>(Name));
    }

    GR_Sound::TSoundBufferControl* PAS_STDCALL RobotCreateSound(char16_t* Name, std::int32_t Group, std::int32_t Looping) {
        GR_Sound::TSoundBufferControl* Sound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
        if (RobotSound) {
            Sound->Configure(static_cast<pas::WideString>(Name), Group, Looping != 0);
        }
        return Sound;
    }

    void PAS_STDCALL RobotFreeSound(GR_Sound::TSoundBufferControl* Sound) {
        if (Sound != nullptr) {
            pas::free(Sound);
        }
    }

    void PAS_STDCALL RobotStartSound(GR_Sound::TSoundBufferControl* Sound) {
        if (Sound != nullptr) {
            if (RobotSound) {
                Sound->Play();
            }
        }
    }

    std::int32_t PAS_STDCALL RobotIsSoundPlaying(GR_Sound::TSoundBufferControl* Sound) {
        std::int32_t Result = 0;
        if (Sound != nullptr) {
            return Sound->IsPlaying() & 127;
        }
        return Result;
    }

    void PAS_STDCALL RobotSetSoundVolume(GR_Sound::TSoundBufferControl* Sound, float Value) {
        if (Sound != nullptr) {
            Sound->SetVolume(Value);
        }
    }

    void PAS_STDCALL RobotSetSoundPan(GR_Sound::TSoundBufferControl* Sound, float Value) {
        if (Sound != nullptr) {
            Sound->SetPan(Value);
        }
    }

    float PAS_STDCALL RobotGetSoundVolume(GR_Sound::TSoundBufferControl* Sound) {
        if (Sound == nullptr) {
            return 0.0f;
        }
        return Sound->Volume;
    }

    float PAS_STDCALL RobotGetSoundPan(GR_Sound::TSoundBufferControl* Sound) {
        if (Sound == nullptr) {
            return 0.0f;
        }
        return Sound->Pan;
    }

    void PAS_STDCALL RobotRenderText(char16_t* Text, char16_t* FontName, std::uint32_t Color, std::int32_t Width, std::int32_t Height, std::int32_t AlignX, std::int32_t AlignY, std::int32_t Wrap, std::int32_t OffsetX, std::int32_t OffsetY, Types::PRect Clip, PRobotTextImage Image) {
        std::int32_t TopAdjustment{};
        EC_Str::TStringsEC* Lines{};
        EC_Str::TStringsEC* WrappedLines{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        std::int32_t DrawX{};
        std::int32_t DrawY{};
        std::int32_t CurrentY{};
        Types::TPoint ImageSize{};
        Types::TRect DrawClip{};
        Types::TRect Bounds{};
        Types::TRect SourceClip{};
        EC_CacheFont::TCFontControlEC* Control = nullptr;
        EC_CacheFont::TCFontEC* Font = nullptr;
        Lines = nullptr;
        WrappedLines = nullptr;
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                Lines->SetText(static_cast<pas::WideString>(Text));
                Control = pas::construct_call<EC_CacheFont::TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(static_cast<pas::WideString>(FontName));
                Font = EC_CacheFont::AcquireCachedFont(Control);
                Font->ResetTextMeasureState();
                Font->UseARGBColors = true;
                Font->ColorTagsEnabled = false;
                if (Wrap != 0) {
                    WrappedLines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                }
                if (Width == 0 && Wrap != 0) {
                    GR_Main::RaiseWideMessage(u"robot text"_wref.get());
                }
                ImageSize = Robot::MeasureRobotTextSize(Width, Wrap, TopAdjustment, Lines, Font, WrappedLines);
                SourceClip = pas::load_unaligned<Types::TRect>(Clip);
                if (Width == 0) {
                    Width = ImageSize.X + 4;
                    SourceClip.Right = Width;
                }
                if (Height == 0) {
                    Height = ImageSize.Y + 4;
                    SourceClip.Bottom = Height;
                }
                Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                Buffer->AllocateRgbaTight(Width, Height);
                Buffer->ClearPixels();
                Image->Buffer = Buffer;
                Image->Pixels = Buffer->GetPixels();
                Image->Pitch = Buffer->PitchBytes;
                Image->Width = Buffer->Width;
                Image->Height = Buffer->Height;
                DrawClip = ClassesImports::Rect(0, 0, Width, Height);
                if (!WindowsSdk::IntersectRect(DrawClip, DrawClip, SourceClip)) {
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                DrawX = 0;
                DrawY = 0;
                if (AlignX == 0 || Wrap != 0) {
                    DrawX = 2;
                } else if (AlignX == 2) {
                    DrawX = Width - ImageSize.X - 2;
                } else if (AlignX == 1) {
                    DrawX = Width / 2 - ImageSize.X / 2;
                } else if (AlignX == 3) {
                    DrawX = 2;
                }
                if (AlignY == 0) {
                    DrawY = TopAdjustment + 2;
                } else if (AlignY == 2) {
                    DrawY = Height - ImageSize.Y - 2 + TopAdjustment;
                } else if (AlignY == 1) {
                    DrawY = Height / 2 - ImageSize.Y / 2 + TopAdjustment;
                } else if (AlignY == 3) {
                    DrawY = TopAdjustment + 2;
                }
                Font->ColorTagsEnabled = true;
                Font->DefaultColor = Color;
                if (Wrap == 0) {
                    if (AlignY == 1) {
                        CurrentY = Height / 2 - (Font->GetLineHeight() * (Lines->GetCount() - 1) + Font->GetCenteringHeight()) / 2 + Font->GetCenteringHeight();
                    } else {
                        CurrentY = Font->AboveBaseline + DrawY - 2;
                    }
                    Lines->First();
                    while (!Lines->IsAtEnd()) {
                        {
                            const pas::WideString& currentText = Lines->GetCurrentText();
                            WindowsSdk::TRect drawClip = DrawClip;
                            void* pixels = Buffer->GetPixels();
                            std::int32_t pitchBytes = Buffer->PitchBytes;
                            Font->DrawTaggedText32(pixels, pitchBytes, DrawX + OffsetX, CurrentY + OffsetY, currentText, drawClip);
                        }
                        CurrentY += Font->GetLineHeight();
                        Lines->Next();
                    }
                } else {
                    CurrentY = Font->AboveBaseline + DrawY - 2;
                    Lines->First();
                    while (!Lines->IsAtEnd()) {
                        Font->WrapTaggedTextIntoLines(WrappedLines, Lines->GetCurrentText(), Width - 4);
                        WrappedLines->First();
                        while (!WrappedLines->IsAtEnd()) {
                            if (AlignX == 0) {
                                const pas::WideString& currentText_2 = WrappedLines->GetCurrentText();
                                WindowsSdk::TRect drawClip_2 = DrawClip;
                                void* pixels_2 = Buffer->GetPixels();
                                std::int32_t pitchBytes_2 = Buffer->PitchBytes;
                                Font->DrawTaggedText32(pixels_2, pitchBytes_2, DrawX + OffsetX, CurrentY + OffsetY, currentText_2, drawClip_2);
                            } else if (AlignX == 2) {
                                Bounds = Font->MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, 0, nullptr);
                                {
                                    const pas::WideString& currentText_3 = WrappedLines->GetCurrentText();
                                    WindowsSdk::TRect drawClip_3 = DrawClip;
                                    void* pixels_3 = Buffer->GetPixels();
                                    std::int32_t pitchBytes_3 = Buffer->PitchBytes;
                                    Font->DrawTaggedText32(pixels_3, pitchBytes_3, Width - (Bounds.Right - Bounds.Left) - 2 + OffsetX, CurrentY + OffsetY, currentText_3, drawClip_3);
                                }
                            } else if (AlignX == 1) {
                                Bounds = Font->MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, 0, nullptr);
                                {
                                    const pas::WideString& currentText_4 = WrappedLines->GetCurrentText();
                                    WindowsSdk::TRect drawClip_4 = DrawClip;
                                    void* pixels_4 = Buffer->GetPixels();
                                    std::int32_t pitchBytes_4 = Buffer->PitchBytes;
                                    Font->DrawTaggedText32(pixels_4, pitchBytes_4, (Width - 0) / 2 - (Bounds.Right - Bounds.Left) / 2 + OffsetX, CurrentY + OffsetY, currentText_4, drawClip_4);
                                }
                            } else if (AlignX == 3 && static_cast<std::uint8_t>(WrappedLines->IsAtLast() ^ 1)) {
                                const pas::WideString& currentText_5 = WrappedLines->GetCurrentText();
                                WindowsSdk::TRect drawClip_5 = DrawClip;
                                void* pixels_5 = Buffer->GetPixels();
                                std::int32_t pitchBytes_5 = Buffer->PitchBytes;
                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText32(Font, pixels_5, pitchBytes_5, DrawX + OffsetX, CurrentY + OffsetY, currentText_5, Width - 4, drawClip_5);
                            } else {
                                const pas::WideString& currentText_6 = WrappedLines->GetCurrentText();
                                WindowsSdk::TRect drawClip_6 = DrawClip;
                                void* pixels_6 = Buffer->GetPixels();
                                std::int32_t pitchBytes_6 = Buffer->PitchBytes;
                                Font->DrawTaggedText32(pixels_6, pitchBytes_6, DrawX + OffsetX, CurrentY + OffsetY, currentText_6, drawClip_6);
                            }
                            CurrentY += Font->GetLineHeight();
                            WrappedLines->Next();
                        }
                        Lines->Next();
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (Font != nullptr) {
                Control->Release();
            }
            if (Control != nullptr) {
                pas::free(Control);
            }
            if (Lines != nullptr) {
                pas::free(Lines);
            }
            if (WrappedLines != nullptr) {
                pas::free(WrappedLines);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
    }

    void PAS_STDCALL RobotFreeText(PRobotTextImage Image) {
        if (Image->Buffer != nullptr) {
            pas::free(Image->Buffer);
        }
        pas::fill_memory(Image, static_cast<std::int32_t>(sizeof(TRobotTextImage)), static_cast<std::uint8_t>(0));
    }

    void PAS_STDCALL RobotSetProgress(float Fraction) {
        fPanelLoad::TfPanelLoad* Panel{};
        if (GR_Main::Direct3DDevice != nullptr) {
            if (fPanelLoad::ActiveLoadPanel != nullptr) {
                Panel = fPanelLoad::ActiveLoadPanel;
                Panel->SelectBackgroundStyle(3);
                Panel->RefreshBackgroundImages();
                Panel->SetShutterOpenFraction(0.0f);
                Panel->SetProgress(Fraction);
                Panel->Show();
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->SetCursorActive(false);
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->InvalidateViewport();
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->Present();
                Panel->Hide();
            }
        }
    }

    float PAS_STDCALL RobotGetMusicVolume() {
        return GlobalsV::MusicVolumeScale;
    }

    void PAS_STDCALL RobotSetMusicVolume(float Value) {
        GR_Sound::TSoundBuffer* Buffer{};
        GlobalsV::MusicVolumeScale = Value;
        if (GR_Main::SoundManager != nullptr) {
            Buffer = GR_Main::SoundManager->FirstBuffer;
            while (Buffer != nullptr) {
                if (Buffer->Streaming) {
                    Buffer->SetVolume(static_cast<long double>(GlobalsV::MusicVolume) * GlobalsV::MusicVolumeScale);
                }
                Buffer = Buffer->Next;
            }
        }
    }

    void PAS_STDCALL RobotPlayMusic() {
        if (GlobalsV::MusicEnabled && RobotMusic && static_cast<std::uint8_t>(GR_Main::MusicManager->HasSelectedMusic() ^ 1)) {
            GR_Main::MusicManager->HasSelectedMusic();
            GR_Main::MusicManager->PlayCategory(u"Robot"_wref.get());
        }
    }

    void PAS_STDCALL RobotReleaseTextures() {
        GR_DX::ReleaseAllTextureSurfaces();
    }

    Types::TPoint MeasureRobotTextSize(std::int32_t& Width, std::int32_t& Wrap, std::int32_t& TopAdjustment, EC_Str::TStringsEC*& Lines, EC_CacheFont::TCFontEC*& Font, EC_Str::TStringsEC*& WrappedLines) {
        Types::TPoint Result{};
        std::uint8_t First{};
        Types::TRect MergedBounds{};
        Types::TRect LineBounds{};
        MergedBounds.Left = 0;
        MergedBounds.Right = 0;
        MergedBounds.Top = 0;
        MergedBounds.Bottom = 0;
        std::int32_t Y = 0;
        Lines->First();
        if (Wrap == 0) {
            if (!Lines->IsAtEnd()) {
                MergedBounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, &TopAdjustment);
                Y += Font->GetLineHeight();
                Lines->Next();
            }
            while (!Lines->IsAtEnd()) {
                LineBounds = Font->MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, Y, nullptr);
                WindowsSdk::UnionRect(MergedBounds, MergedBounds, LineBounds);
                Y += Font->GetLineHeight();
                Lines->Next();
            }
        } else {
            First = true;
            while (!Lines->IsAtEnd()) {
                Font->WrapTaggedTextIntoLines(WrappedLines, Lines->GetCurrentText(), Width - 4);
                if (!WrappedLines->IsEmpty()) {
                    WrappedLines->First();
                    if (First) {
                        MergedBounds = Font->MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, Y, &TopAdjustment);
                        First = false;
                        Y += Font->GetLineHeight();
                        WrappedLines->Next();
                    }
                    while (!WrappedLines->IsAtEnd()) {
                        LineBounds = Font->MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, Y, nullptr);
                        WindowsSdk::UnionRect(MergedBounds, MergedBounds, LineBounds);
                        Y += Font->GetLineHeight();
                        WrappedLines->Next();
                    }
                }
                Lines->Next();
            }
        }
        Result = ClassesImports::Point(MergedBounds.Right - MergedBounds.Left, MergedBounds.Bottom - MergedBounds.Top);
        return Result;
    }

} // namespace Robot
