#pragma once
#include "types/Globals.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/fSaveManager.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Expression {
    struct TVarArrayEC;

} // namespace EC_Expression

namespace EC_Struct {
    struct TPointF;

} // namespace EC_Struct

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace SE_Process {
    struct TProcessSE;

} // namespace SE_Process

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace ThreadCalc {
    struct TThreadCalc;

} // namespace ThreadCalc

namespace aEFilm {
    struct TEFilm;

} // namespace aEFilm

namespace aEFilmEnd {
    struct TEFilmEnd;

} // namespace aEFilmEnd

namespace aItem {
    struct TItem;

} // namespace aItem

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aShip {
    struct TShip;

} // namespace aShip

namespace ab_MainForm {
    struct TfAB;

} // namespace ab_MainForm

namespace fAbout {
    struct TfAbout;

} // namespace fAbout

namespace fAchievements {
    struct TfAchievements;

} // namespace fAchievements

namespace fCfgSettings {
    struct TfCfgSettings;

} // namespace fCfgSettings

namespace fEquipmentShop {
    struct TfEquipmentShop;

} // namespace fEquipmentShop

namespace fFilm {
    struct TfFilm;

} // namespace fFilm

namespace fFilmFile {
    struct TFilmFile;

} // namespace fFilmFile

namespace fGalaxy2 {
    struct TfGalaxy2;

} // namespace fGalaxy2

namespace fGameEnd {
    struct TfGameEnd;

} // namespace fGameEnd

namespace fGameLoad {
    struct TfGameLoad;

} // namespace fGameLoad

namespace fGameMenu {
    struct TfGameMenu;

} // namespace fGameMenu

namespace fGameSettings {
    struct TThreadCreateNewGame;

} // namespace fGameSettings

namespace fGameSettings2 {
    struct TfGameSettings2;

} // namespace fGameSettings2

namespace fGoodsShop2 {
    struct TfGoodsShop2;

} // namespace fGoodsShop2

namespace fGov {
    struct TfGov;

} // namespace fGov

namespace fHangar {
    struct TfHangar;

} // namespace fHangar

namespace fInfo {
    struct TfInfo;

} // namespace fInfo

namespace fIntroduction {
    struct TfIntroduction;

} // namespace fIntroduction

namespace fJournal {
    struct TfJournal;

} // namespace fJournal

namespace fJump {
    struct TfJump;

} // namespace fJump

namespace fLoad {
    struct TCacheLoader;

    struct TfLoad;

} // namespace fLoad

namespace fLoadAB {
    struct TfLoadAB;

} // namespace fLoadAB

namespace fLoadQuest {
    struct TfLoadQuest;

} // namespace fLoadQuest

namespace fLoadRobot {
    struct TfLoadRobot;

} // namespace fLoadRobot

namespace fMainForm {
    struct TfMainForm;

} // namespace fMainForm

namespace fPlanet {
    struct TfPlanet;

} // namespace fPlanet

namespace fPlanetNO {
    struct TfPlanetNO;

} // namespace fPlanetNO

namespace fPlanetQuest {
    struct TfPlanetQuest;

} // namespace fPlanetQuest

namespace fRating2 {
    struct TfRating2;

} // namespace fRating2

namespace fRewards {
    struct TfRewards;

} // namespace fRewards

namespace fRuinsTalk {
    struct TfRuinsTalk;

} // namespace fRuinsTalk

namespace fScaner {
    struct TfScaner;

} // namespace fScaner

namespace fScore {
    struct TfScore;

} // namespace fScore

namespace fSelectFace {
    struct TfSelectFace;

} // namespace fSelectFace

namespace fShip2 {
    struct TfShip2;

} // namespace fShip2

namespace fStarMap {
    struct TfStarMap;

} // namespace fStarMap

namespace fTalk {
    struct TfTalk;

} // namespace fTalk

namespace Globals {
    extern pas::Array<Globals::TMessagePlayerTypeGraph, 0, 10> PlayerMessagePresentations;

    extern fMainForm::TfMainForm* MainMenuScreen;

    extern fGameSettings2::TfGameSettings2* NewGameScreen;

    extern fIntroduction::TfIntroduction* IntroductionScreen;

    extern fHangar::TfHangar* HangarScreen;

    extern fPlanet::TfPlanet* PlanetScreen;

    extern fPlanetNO::TfPlanetNO* UninhabitedPlanetScreen;

    extern fPlanetQuest::TfPlanetQuest* PlanetQuestScreen;

    extern fRuinsTalk::TfRuinsTalk* RuinsTalkScreen;

    extern ab_MainForm::TfAB* ArcadeBattleScreen;

    extern fEquipmentShop::TfEquipmentShop* EquipmentShopScreen;

    extern fGoodsShop2::TfGoodsShop2* GoodsShopScreen;

    extern fGov::TfGov* GovernmentScreen;

    extern fInfo::TfInfo* InfoScreen;

    extern fRating2::TfRating2* RangerRatingScreen;

    extern fRewards::TfRewards* RewardsScreen;

    extern fShip2::TfShip2* ShipScreen;

    extern fTalk::TfTalk* TalkScreen;

    extern fScaner::TfScaner* ScannerScreen;

    extern fStarMap::TfStarMap* StarMapScreen;

    extern fFilm::TfFilm* FilmScreen;

    extern fGalaxy2::TfGalaxy2* GalaxyScreen;

    extern fJump::TfJump* JumpScreen;

    extern fLoad::TfLoad* LoadScreen;

    extern fSaveManager::TfSaveManager* SaveManagerScreen;

    extern fGameLoad::TfGameLoad* GameLoadScreen;

    extern fGameMenu::TfGameMenu* GameMenuScreen;

    extern fCfgSettings::TfCfgSettings* SettingsScreen;

    extern fGameEnd::TfGameEnd* GameEndScreen;

    extern fAbout::TfAbout* AboutScreen;

    extern fScore::TfScore* ScoreScreen;

    extern fSelectFace::TfSelectFace* SelectFaceScreen;

    extern GI_MessageLoop::TMessageLoopGI* SpaceObjectUiLoop;

    extern fJournal::TfJournal* JournalScreen;

    extern fLoadRobot::TfLoadRobot* LoadRobotScreen;

    extern fLoadQuest::TfLoadQuest* LoadQuestScreen;

    extern fLoadAB::TfLoadAB* LoadArcadeScreen;

    extern fAchievements::TfAchievements* AchievementsScreen;

    extern EC_Struct::TPointF SpaceViewPosition;

    extern std::uint8_t FilmCameraFollow;

    extern aShip::TShip* TalkShip;

    extern aPlanet::TPlanet* TalkPlanet;

    extern std::uint8_t TalkScripted;

    extern std::uint8_t TalkType;

    extern std::int32_t TalkAmount;

    extern std::uint8_t TalkResponse;

    extern pas::WideString TalkText;

    extern aItem::TItem* ScriptUseItem;

    extern pas::List* ScriptItemContextStack;

    extern pas::List* ScriptItemInfoContextStack;

    extern pas::List* ScriptActionShipStack;

    extern pas::List* ScriptActionObject1Stack;

    extern pas::List* ScriptActionObject2Stack;

    extern pas::List* ScriptActionParamStack;

    extern pas::List* ScriptActionTypeStack;

    extern std::uint8_t ScreenLoadMode;

    extern pas::List* ScriptTemplates;

    extern EC_Expression::TVarArrayEC* SharedScriptVariables;

    extern EC_Expression::TVarArrayEC* GlobalScriptVariables;

    extern std::uint8_t ScriptTemplateStartRequested;

    extern pas::WideString LastLoadedPlayerName;

    extern std::uint8_t ReloadScriptTemplates;

    extern std::uint8_t ReloadModsRequested;

    extern std::uint8_t StandaloneQuestMode;

    extern pas::Object* ScannerTarget;

    extern std::int32_t ScriptDialogIndex;

    extern pas::Object* AwardSubject;

    extern std::uint8_t PlayerStarDayPrepared;

    extern std::uint32_t PreviousFilmActivity;

    extern std::uint8_t FilmSoundEffectsEnabled;

    extern ThreadCalc::TThreadCalc* TurnCalculationThread;

    extern fFilmFile::TFilmFile* FilmHistory;

    extern fLoad::TCacheLoader* CacheLoader;

    extern fSaveManager::TSaveManagerMode SaveManagerMode;

    extern std::uint32_t TalkRequestEvent;

    extern std::uint32_t TalkCompletedEvent;

    extern std::uint32_t ScriptUiRequestEvent;

    extern std::uint32_t ScriptUiAbortEvent;

    extern pas::List* PlanetRenderTemplates;

    extern std::int32_t MinimapFrameCounter;

    extern std::uint8_t Skip1C;

    extern std::uint8_t SkipVideo;

    extern std::uint8_t SkipIntro;

    extern fGameSettings::TThreadCreateNewGame* NewGameGenerationThread;

    extern std::uint32_t ShownPlayerTips;

    extern std::uint8_t StarMapWeaponPanelOpen;

    extern aEFilm::TEFilm* PrimaryFilm;

    extern aEFilm::TEFilm* SecondaryFilm;

    extern aEFilmEnd::TEFilmEnd* TrailingFilmEffects;

    extern SE_Process::TProcessSE* SpaceProcess;

    extern EC_Buf::TBufEC* ActiveLoadBuffer;

    extern pas::CriticalSection* PersistentPlayerMessageLock;

    extern Globals::TMessagePlayer* FirstPersistentPlayerMessage;

    extern Globals::TMessagePlayer* LastPersistentPlayerMessage;

    extern pas::DynArray<pas::WideString> ArcadeExplosionSounds;

    extern pas::DynArray<pas::WideString> ArcadeItemSounds;

    extern pas::DynArray<pas::WideString> ArcadeHitSounds;

    extern pas::Array<pas::WideString, 0, 17> ArcadeWeaponFirstSounds;

    extern pas::Array<pas::WideString, 0, 17> ArcadeWeaponLoopSounds;

    extern pas::Array<std::int32_t, 0, 17> ArcadeWeaponLoopTicks;

    extern pas::Array<pas::Array<SE_Space::TObjectSE*, 0, 5>, 0, 7> RaceShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> BlazerShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> KellerShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> TerronShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> PirateClanShipTemplates;

    extern pas::DynArray<Globals::TPlanetSpaceTemplate> PlanetSpaceTemplates;

    extern std::int32_t UselessItemRemainsCount;

    extern pas::DynArray<Globals::TRobotMap> RobotMapDefinitions;

    extern pas::DynArray<Globals::TShipGreetingsInfo> ShipGreetingDefinitions;

    extern std::int32_t ShipGreetingCount;

    extern pas::DynArray<Globals::TGovGreetingsInfo> GovernmentGreetingDefinitions;

    extern std::int32_t GovernmentGreetingCount;

    extern pas::DynArray<Globals::TPlanetAdvtGroup> PlanetAdvertDefinitions;

    extern pas::Array<pas::WideString, 0, 10> ScriptVariableTypeNames;

    std::int32_t FindScriptTemplateIndex(const pas::WideString& Name);

    void CollectInactiveScriptTemplates(pas::List* Dest);

    void RecreateSpaceProcess(const pas::WideString& ConfigName);

    std::uint8_t ShowPlayerTipOnce(std::int32_t Index);

    std::uint8_t HasShownPlayerTip(std::int32_t Index);

    std::int32_t SelectSpaceImageTemplateFromSeed(std::int32_t Kind, std::uint32_t Seed);

    std::int32_t SelectSpaceImageTemplate(std::int32_t Kind);

    std::int32_t FindPlanetSpaceTemplateIndex(std::int32_t Style, std::int32_t StyleVariant);

    void HandleRuntimeExitCheck1();

    void HandleRuntimeExitCheck2();

    void InitializeScriptHostRuntime();

    void FinalizeScriptHostRuntime();

    aGalaxyStruct::TOwnerMask ParseRobotMapRaceMask(pas::WideString Text);

    std::int32_t FindRobotMapById(std::int32_t MapId);

    void InitializeRobotMapDefinitions();

    void InitializeShipGreetingDefinitions();

    void InitializeGovernmentGreetingDefinitions();

    void InitializePlanetAdvertDefinitions();

    void InitializeGlobalUiRuntime();

    void FinalizeGlobalUiRuntime();

    void ResetScriptHostRuntimeState();

    GI_MessageLoop::TMessageLoopGI* FindMessageLoop(pas::WideString Name);

    void RunMainScreenStateLoop();

    void SwapTurnFilms();

    GI_MessageLoop::TMessageLoopGI* GetInnermostScreenLoop();

    TMessagePlayer* FindPlayerMessageExceptKinds(const pas::WideString& Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock);

    std::uint8_t RemovePlayerMessagesExceptKinds(pas::WideString Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock);

    void ClearPersistentPlayerMessages();

    std::int32_t CountPersistentPlayerMessages();

    std::uint8_t IsPersistentPlayerMessageQueued(TMessagePlayer* MessageEntry, std::uint8_t SkipLock);

    void RemovePersistentPlayerMessage(TMessagePlayer* MessageEntry, std::uint8_t SkipLock);

    TMessagePlayer* FindPlayerBubbleByText(const pas::WideString& Text, std::uint8_t SkipLock);

    TMessagePlayer* FindPlayerBubbleByKey(const pas::WideString& Key, std::uint8_t SkipLock);

    void RemovePlayerBubblePages(const pas::WideString& Prefix, std::int32_t FirstPage);

    void RemovePlayerBubbleByKey(const pas::WideString& Key);

    TMessagePlayer* CreatePersistentPlayerMessage();

    TMessagePlayer* AddOrUpdatePlayerBubble(std::uint8_t Kind, std::int32_t Turn, const pas::WideString& Text, const pas::WideString& Key);

    void PruneExpiredPersistentPlayerMessages();

    void TMessagePlayer_Create(TMessagePlayer* Self);

    void TScriptTemplUnit_Create(TScriptTemplUnit* Self);

    void TScriptTemplUnit_Destroy(TScriptTemplUnit* Self);

} // namespace Globals
