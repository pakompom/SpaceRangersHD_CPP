#pragma once
#include "types/GlobalsV.hpp"

namespace GlobalsV {
    extern std::uint8_t DumpLoadedConfig;

    extern std::uint8_t HalfGovAnim;

    // // Zero-based.
    extern std::int32_t QuestStyleIndex;

    extern std::uint8_t QuestPageAnimationEnabled;

    extern std::int32_t DefaultOrder;

    extern std::int32_t RightClickOnShip;

    // Native purpose remains unresolved.
    extern std::uint8_t EstOptionEnabled;

    extern std::uint8_t SendRecordOff;

    extern std::int32_t ChangeAutoPilot;

    extern std::uint8_t DisableAutoPilot;

    extern std::uint8_t SkipGiper;

    extern std::int32_t Wind;

    // Native default; standalone selector replaces it with a numeric quest ID or a quest resource name.
    extern pas::WideString PendingQuestName;

    extern GlobalsV::TGameScreenTable RegisteredScreens;

    extern std::uint8_t SkipSavedPixelRestore;

    extern std::uint8_t HardwareRenderingRequested;

    extern std::uint8_t HardwareRenderingEnabled;

    // Native startup checks ntdll wine_get_version/wine_get_host_version.
    extern std::uint8_t RunningUnderWine;

    extern std::uint8_t ScaleViewportToWindow;

    extern std::uint8_t UseTablesForGov;

    extern pas::WideString RangerFontName;

    extern pas::WideString MiniFontName;

    // Initialized to Font.2Small.
    extern pas::WideString SmallFontName;

    // Initialized to Font.2SmallBold.
    extern pas::WideString SmallBoldFontName;

    // Initialized to Font.2Normal.
    extern pas::WideString NormalFontName;

    // Initialized to Font.2NormalBold.
    extern pas::WideString NormalBoldFontName;

    // Initialized to Font.2Big.
    extern pas::WideString BigFontName;

    extern pas::WideString HugeFontName;

    extern pas::WideString IntroFontName;

    extern pas::WideString AuthorsFontName;

    // Initialized to Font.Verdana8.
    extern pas::WideString SmoothSmallFontName;

    // Initialized to Font.Verdana8bold.
    extern pas::WideString SmoothSmallBoldFontName;

    // Initialized to Font.Verdana9.
    extern pas::WideString SmoothNormalFontName;

    // Initialized to Font.Verdana9bold.
    extern pas::WideString SmoothNormalBoldFontName;

    extern pas::WideString SmoothBigFontName;

    extern pas::WideString SmoothHugeFontName;

    extern pas::WideString SmoothIntroFontName;

    extern pas::AnsiString PendingLoadFileName;

    extern std::int32_t LoadedFilmCount;

    // One selects death; other native ending codes remain unresolved.
    extern std::int32_t GameEndReason;

    extern std::int32_t ShipTail;

    // 3D setting, cleared on EDirectXRender during device startup.
    extern std::uint8_t ThreeDimensionalModeEnabled;

    extern std::uint8_t AnimCaptain;

    extern std::uint8_t AnimItem;

    extern std::int32_t Comet;

    extern std::uint8_t BGImage;

    extern std::uint8_t AnimShipFull;

    extern std::uint8_t AnimCity;

    extern std::uint8_t AnimMenuShip;

    extern std::int32_t AnimGov;

    extern std::uint8_t AnimStar;

    extern std::uint8_t AnimHangar;

    extern std::uint8_t CircleAction;

    extern std::uint8_t StaticBackground;

    extern std::int32_t ScrollTime;

    extern std::int32_t ScrollStep;

    extern std::int32_t ScrollSense;

    extern std::int32_t FilmSpeed;

    extern std::int32_t BGOCount;

    extern std::int32_t BGOTime;

    extern std::int32_t SpaceImage;

    extern std::uint8_t SoundEnabled;

    extern std::uint8_t SoundInSpaceEnabled;

    extern float SoundVolume;

    extern float RobotSoundVolume;

    extern std::uint8_t MusicEnabled;

    extern std::uint8_t MusicInSpaceEnabled;

    extern std::uint8_t MusicInHyperEnabled;

    extern std::uint8_t MusicInPlanetEnabled;

    extern float MusicVolume;

    extern float MusicVolumeScale;

    extern float RobotMusicVolume;

    extern std::int32_t MaxFilmStepSkip;

    // CountFilmSave setting. UI range is 1..100; config loading clamps only the minimum to 1. Limit applies when a recording is added, not while loading a save.
    extern std::int32_t FilmHistoryLimit;

    extern float BeginCalcNextTurn;

    extern std::uint8_t DoNotChangeMusicInBattle;

    extern std::uint8_t ViewFollowShip;

    extern std::uint8_t ActionDoubleClick;

    // FontGalaxy setting; default gmfNormalBold. Out-of-range values also use the normal-bold font.
    extern GlobalsV::TGalaxyMapFontChoice GalaxyMapFontChoice;

    extern std::int32_t FontQuest;

    extern std::int32_t FontDialog;

    // FontSmooth setting.
    extern std::uint8_t FontSmoothingEnabled;

    // 0=BMP, 1=PNG, 2=JPEG.
    extern std::int32_t ScreenshotFormat;

    extern std::int32_t ScreenshotJpegQuality;

    extern std::uint8_t DynamicTipsPos;

    extern std::uint8_t ViewPathLength;

    extern std::int32_t AfterburnerStopCondition;

    extern std::int32_t TurnSaveStep;

    extern std::int32_t QuickSaveExtraSlots;

    extern std::int32_t MaxPlayerNews;

    extern std::int32_t MaxSearchResult;

    extern std::uint8_t ClickAutoCloseForm;

    // Native purpose remains unresolved.
    extern std::uint8_t UiRuntimeFlag;

    extern std::uint8_t MultiThreadEnabled;

    extern std::uint8_t ShowWineWarning;

    extern std::uint8_t XonarSoundDevice;

    extern std::uint8_t ShowXonarWarning;

    extern float PlanetDepth;

    extern float ShipPathDepth;

    extern float ShipPathEndDepth;

    extern float UnitPathDepth;

    extern float UnitPathEndDepth;

    extern float ActionButtonDepth;

    extern float GalaxyStarDepth;

    extern float GalaxyStarNameDepth;

    extern float GalaxyWarDepth;

    extern float ConstellationLineDepth;

    extern float ConstellationColorDepth;

    extern std::uint8_t MemorySnapshotActive;

    extern GlobalsV::TGameScreenId PreviousScreenId;

    extern GlobalsV::TGameScreenId CurrentScreenId;

    extern GlobalsV::TGameScreenId RequestedScreenId;

    extern GlobalsV::TGameScreenId PostLoadScreenId;

    extern GlobalsV::TGameScreenId ShipReturnScreenId;

    extern GlobalsV::TGameScreenId TalkReturnScreenId;

    extern GlobalsV::TGameScreenId ScannerReturnScreenId;

    // Quest selector or campaign screen that launched the active text quest.
    extern GlobalsV::TGameScreenId QuestReturnScreenId;

    // Return screen for native screen ID 13; its purpose remains unresolved.
    extern GlobalsV::TGameScreenId Screen13ReturnScreenId;

    // Set by the star-map/ruins caller; consumed by galaxy-map Back.
    extern GlobalsV::TGameScreenId GalaxyReturnScreenId;

    extern GlobalsV::TGameScreenId SaveManagerReturnScreenId;

    extern GlobalsV::TGameScreenId GameMenuReturnScreenId;

    extern GlobalsV::TGameScreenId SettingsReturnScreenId;

    extern GlobalsV::TGameScreenId AchievementsReturnScreenId;

    extern std::int32_t LoadedSaveVersion;

    // -1 outside the film-loading phase.
    extern std::int32_t LoadingFilmCount;

    extern std::uint8_t BackgroundShade;

    extern std::uint8_t BackgroundBlur;

    extern std::uint8_t BackgroundGrayscale;

    extern std::uint8_t PlanetClouds;

    extern std::uint8_t PlanetAtm;

    extern std::uint8_t AnimChangeForm;

    extern std::uint8_t AnimMainFon;

    extern std::uint8_t SputnikShow;

    // Used by TPlanetGI.SetImageFromTemplate when creating its light buffers.
    extern pas::WideString SatelliteLightMapPath;

    extern std::int32_t SatelliteTemplateParameter1;

    extern std::int32_t SatelliteTemplateParameter2;

    extern std::int32_t MinimumSatelliteTemplateRadius;

    // Base for procedural satellite display sizes, separate from render-template limits.
    extern std::int32_t GeneratedSatelliteBaseRadius;

    extern std::int32_t MaximumSatelliteTemplateRadius;

    // Owns TSputnikTempl instances.
    extern pas::List* SatelliteRenderTemplates;

    extern pas::DynArray<GlobalsV::TSpaceImageTemplate> SpaceImageTemplates;

    extern pas::DynArray<GlobalsV::TStarFieldImageTemplate> StarFieldImageTemplates;

    // Nonnegative forces this quest ID in government offers; native default is -1.
    extern std::int32_t ForcedPlanetQuestId;

    // Raises when absent; nil matches the first empty slot.
    TGameScreenId FormToId(pas::Object* Screen);

    // No bounds check.
    pas::Object* GetRegisteredScreenLoop(TGameScreenId ScreenId);

    std::uint8_t IsSpaceBackdropScreen(TGameScreenId ScreenId);

    std::uint8_t ScreenUsesCompositeLoadAssets(TGameScreenId ScreenId);

} // namespace GlobalsV
