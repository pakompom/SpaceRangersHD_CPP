#include "layout/GlobalsV.hpp"
#include "units/GlobalsV.hpp"

namespace GlobalsV {
    std::uint8_t DumpLoadedConfig = false;

    std::uint8_t HalfGovAnim = false;

    std::int32_t QuestStyleIndex = 0;

    std::uint8_t QuestPageAnimationEnabled = true;

    std::int32_t DefaultOrder = 0;

    std::int32_t RightClickOnShip = 0;

    std::uint8_t EstOptionEnabled = false;

    std::uint8_t SendRecordOff = false;

    std::int32_t ChangeAutoPilot = 4;

    std::uint8_t DisableAutoPilot = false;

    std::uint8_t SkipGiper = false;

    std::int32_t Wind = 2;

    pas::WideString PendingQuestName = u"Prison"_w;

    GlobalsV::TGameScreenTable RegisteredScreens{};

    std::uint8_t SkipSavedPixelRestore{};

    std::uint8_t HardwareRenderingRequested{};

    std::uint8_t HardwareRenderingEnabled{};

    std::uint8_t RunningUnderWine{};

    std::uint8_t ScaleViewportToWindow{};

    std::uint8_t UseTablesForGov{};

    pas::WideString RangerFontName{};

    pas::WideString MiniFontName{};

    pas::WideString SmallFontName{};

    pas::WideString SmallBoldFontName{};

    pas::WideString NormalFontName{};

    pas::WideString NormalBoldFontName{};

    pas::WideString BigFontName{};

    pas::WideString HugeFontName{};

    pas::WideString IntroFontName{};

    pas::WideString AuthorsFontName{};

    pas::WideString SmoothSmallFontName{};

    pas::WideString SmoothSmallBoldFontName{};

    pas::WideString SmoothNormalFontName{};

    pas::WideString SmoothNormalBoldFontName{};

    pas::WideString SmoothBigFontName{};

    pas::WideString SmoothHugeFontName{};

    pas::WideString SmoothIntroFontName{};

    pas::AnsiString PendingLoadFileName{};

    std::int32_t LoadedFilmCount{};

    std::int32_t GameEndReason{};

    std::int32_t ShipTail = 0;

    std::uint8_t ThreeDimensionalModeEnabled = false;

    std::uint8_t AnimCaptain = false;

    std::uint8_t AnimItem = false;

    std::int32_t Comet = 1;

    std::uint8_t BGImage = false;

    std::uint8_t AnimShipFull = false;

    std::uint8_t AnimCity = false;

    std::uint8_t AnimMenuShip = true;

    std::int32_t AnimGov = 2;

    std::uint8_t AnimStar = true;

    std::uint8_t AnimHangar = true;

    std::uint8_t CircleAction = true;

    std::uint8_t StaticBackground = true;

    std::int32_t ScrollTime = 20;

    std::int32_t ScrollStep = 5;

    std::int32_t ScrollSense = 1;

    std::int32_t FilmSpeed = 1;

    std::int32_t BGOCount = 50;

    std::int32_t BGOTime = 300;

    std::int32_t SpaceImage = 0;

    std::uint8_t SoundEnabled = false;

    std::uint8_t SoundInSpaceEnabled = false;

    float SoundVolume = 1.0f;

    float RobotSoundVolume = 1.0f;

    std::uint8_t MusicEnabled = false;

    std::uint8_t MusicInSpaceEnabled = false;

    std::uint8_t MusicInHyperEnabled = true;

    std::uint8_t MusicInPlanetEnabled = true;

    float MusicVolume = 0.75f;

    float MusicVolumeScale = 1.0f;

    float RobotMusicVolume = 0.75f;

    std::int32_t MaxFilmStepSkip = 3;

    std::int32_t FilmHistoryLimit = 1;

    float BeginCalcNextTurn = 1.0f;

    std::uint8_t DoNotChangeMusicInBattle = false;

    std::uint8_t ViewFollowShip = true;

    std::uint8_t ActionDoubleClick = true;

    GlobalsV::TGalaxyMapFontChoice GalaxyMapFontChoice = GlobalsV::gmfNormalBold;

    std::int32_t FontQuest = 0;

    std::int32_t FontDialog = 0;

    std::uint8_t FontSmoothingEnabled = false;

    std::int32_t ScreenshotFormat = 1;

    std::int32_t ScreenshotJpegQuality = 85;

    std::uint8_t DynamicTipsPos = true;

    std::uint8_t ViewPathLength = false;

    std::int32_t AfterburnerStopCondition = 35;

    std::int32_t TurnSaveStep = 0;

    std::int32_t QuickSaveExtraSlots = 0;

    std::int32_t MaxPlayerNews = 30;

    std::int32_t MaxSearchResult = 100;

    std::uint8_t ClickAutoCloseForm = true;

    std::uint8_t UiRuntimeFlag = false;

    std::uint8_t MultiThreadEnabled = false;

    std::uint8_t ShowWineWarning = false;

    std::uint8_t XonarSoundDevice = false;

    std::uint8_t ShowXonarWarning = false;

    float PlanetDepth = 0.0f;

    float ShipPathDepth = 15.0f;

    float ShipPathEndDepth = 14.0f;

    float UnitPathDepth = 13.0f;

    float UnitPathEndDepth = 12.0f;

    float ActionButtonDepth = 9.0f;

    float GalaxyStarDepth = 2.0E+1f;

    float GalaxyStarNameDepth = 19.0f;

    float GalaxyWarDepth = 18.0f;

    float ConstellationLineDepth = 21.0f;

    float ConstellationColorDepth = 22.0f;

    std::uint8_t MemorySnapshotActive = false;

    GlobalsV::TGameScreenId PreviousScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId CurrentScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId RequestedScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId PostLoadScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId ShipReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId TalkReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId ScannerReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId QuestReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId Screen13ReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId GalaxyReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId SaveManagerReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId GameMenuReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId SettingsReturnScreenId = GlobalsV::screenNone;

    GlobalsV::TGameScreenId AchievementsReturnScreenId = GlobalsV::screenNone;

    std::int32_t LoadedSaveVersion = 0;

    std::int32_t LoadingFilmCount = -1;

    std::uint8_t BackgroundShade = false;

    std::uint8_t BackgroundBlur = false;

    std::uint8_t BackgroundGrayscale = false;

    std::uint8_t PlanetClouds = true;

    std::uint8_t PlanetAtm = true;

    std::uint8_t AnimChangeForm = true;

    std::uint8_t AnimMainFon = true;

    std::uint8_t SputnikShow = true;

    pas::WideString SatelliteLightMapPath = u"Bm.Planet.S.Light094"_w;

    std::int32_t SatelliteTemplateParameter1 = 128;

    std::int32_t SatelliteTemplateParameter2 = 60;

    std::int32_t MinimumSatelliteTemplateRadius = 10;

    std::int32_t GeneratedSatelliteBaseRadius = 13;

    std::int32_t MaximumSatelliteTemplateRadius = 60;

    pas::List* SatelliteRenderTemplates = nullptr;

    pas::DynArray<GlobalsV::TSpaceImageTemplate> SpaceImageTemplates = nullptr;

    pas::DynArray<GlobalsV::TStarFieldImageTemplate> StarFieldImageTemplates = nullptr;

    std::int32_t ForcedPlanetQuestId = -1;

    TGameScreenId FormToId(pas::Object* Screen) {
        TGameScreenId Id = screenNone;
        do {
            if (RegisteredScreens[Id] == Screen) {
                return Id;
            }
            pas::inc(Id);
        } while (!(Id == static_cast<TGameScreenId>(42)));
        pas::raise(pas::make_exception<pas::Exception>("FormToId"_a));
    }

    pas::Object* GetRegisteredScreenLoop(TGameScreenId ScreenId) {
        return RegisteredScreens[ScreenId];
    }

    std::uint8_t IsSpaceBackdropScreen(TGameScreenId ScreenId) {
        return ScreenId == screenStarMap || ScreenId == screenGalaxy || ScreenId == screenFilm || ScreenId == screenTalk;
    }

    std::uint8_t ScreenUsesCompositeLoadAssets(TGameScreenId ScreenId) {
        std::uint8_t Result = false;
        if (GlobalsV::IsSpaceBackdropScreen(ScreenId)) {
            return true;
        }
        if (ScreenId == screenLoad && GlobalsV::IsSpaceBackdropScreen(PostLoadScreenId)) {
            return true;
        }
        if (ScreenId == screenShip && GlobalsV::IsSpaceBackdropScreen(ShipReturnScreenId)) {
            return true;
        }
        if (ScreenId == screenScanner && GlobalsV::IsSpaceBackdropScreen(ScannerReturnScreenId)) {
            return true;
        }
        if (ScreenId == static_cast<TGameScreenId>(13) && GlobalsV::IsSpaceBackdropScreen(Screen13ReturnScreenId)) {
            return true;
        }
        if (ScreenId == screenSaveManager && GlobalsV::IsSpaceBackdropScreen(SaveManagerReturnScreenId)) {
            return true;
        }
        if (ScreenId == screenGameMenu && GlobalsV::IsSpaceBackdropScreen(GameMenuReturnScreenId)) {
            return true;
        }
        if (ScreenId == screenSettings && GlobalsV::IsSpaceBackdropScreen(SettingsReturnScreenId)) {
            return true;
        }
        if (ScreenId == screenSettings && SettingsReturnScreenId == screenGameMenu && GlobalsV::IsSpaceBackdropScreen(GameMenuReturnScreenId)) {
            return true;
        }
        return Result;
    }

} // namespace GlobalsV
