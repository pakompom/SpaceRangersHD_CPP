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
    // Native image defaults and turn lifetimes.
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

    // Unregistered loop centered on the back buffer; hosts space-object controls.
    extern GI_MessageLoop::TMessageLoopGI* SpaceObjectUiLoop;

    extern fJournal::TfJournal* JournalScreen;

    extern fLoadRobot::TfLoadRobot* LoadRobotScreen;

    extern fLoadQuest::TfLoadQuest* LoadQuestScreen;

    extern fLoadAB::TfLoadAB* LoadArcadeScreen;

    // Native UI shutdown does not free this screen.
    extern fAchievements::TfAchievements* AchievementsScreen;

    // Map scroll offset and scene sound attenuation origin.
    extern EC_Struct::TPointF SpaceViewPosition;

    extern std::uint8_t FilmCameraFollow;

    extern aShip::TShip* TalkShip;

    // Planet dialogue target, assigned by TPlanet.RequestDialog.
    extern aPlanet::TPlanet* TalkPlanet;

    // Set for the scripted Keller dialogue.
    extern std::uint8_t TalkScripted;

    // tk* conversation ID set by TShip.ShowPlayerDialogue and exposed by SF_GetTalkType.
    extern std::uint8_t TalkType;

    // Negotiated amount; ShowPlayerDialogue overwrites it only for positive inputs.
    extern std::int32_t TalkAmount;

    // Response selected by the conversation UI.
    extern std::uint8_t TalkResponse;

    // Message supplied to the conversation UI.
    extern pas::WideString TalkText;

    // Item currently executing OnUse; native runner clears it after success.
    extern aItem::TItem* ScriptUseItem;

    extern pas::List* ScriptItemContextStack;

    extern pas::List* ScriptItemInfoContextStack;

    extern pas::List* ScriptActionShipStack;

    extern pas::List* ScriptActionObject1Stack;

    extern pas::List* ScriptActionObject2Stack;

    extern pas::List* ScriptActionParamStack;

    extern pas::List* ScriptActionTypeStack;

    // Startup sets 0, or 3 for a screen with composite loading assets.
    extern std::uint8_t ScreenLoadMode;

    // Owns native TScriptTemplUnit entries.
    extern pas::List* ScriptTemplates;

    // Shared scope used by script compilation, execution and text-quest external parameters.
    extern EC_Expression::TVarArrayEC* SharedScriptVariables;

    // Persistent script globals, initially GRunFrom and GRunStar.
    extern EC_Expression::TVarArrayEC* GlobalScriptVariables;

    // Set by script condition code; checked after running each inactive template.
    extern std::uint8_t ScriptTemplateStartRequested;

    // Assigned by TPlayer.LoadFromBuffer; broader UI-cache role unresolved.
    extern pas::WideString LastLoadedPlayerName;

    // Native initial value is True. Reset sets this flag; UI initialization reloads script and ship templates then clears it.
    extern std::uint8_t ReloadScriptTemplates;

    extern std::uint8_t ReloadModsRequested;

    // The quest selector sets True; planet/government/script launches set False. Guards campaign quest checks and turn advancement.
    extern std::uint8_t StandaloneQuestMode;

    // Borrowed target passed from the star map to the scanner screen.
    extern pas::Object* ScannerTarget;

    // Selected script dialogue; -1 while resolving a dialogue variable.
    extern std::int32_t ScriptDialogIndex;

    // Borrowed ship selected by inventory, scanner or ranger ranking for the medals screen.
    extern pas::Object* AwardSubject;

    extern std::uint8_t PlayerStarDayPrepared;

    extern std::uint32_t PreviousFilmActivity;

    // Temporarily disabled while the film slider seeks through steps.
    extern std::uint8_t FilmSoundEffectsEnabled;

    // Owned here; ThreadCalc and Rangers access it through the imported reference cell.
    extern ThreadCalc::TThreadCalc* TurnCalculationThread;

    extern fFilmFile::TFilmFile* FilmHistory;

    extern fLoad::TCacheLoader* CacheLoader;

    extern fSaveManager::TSaveManagerMode SaveManagerMode;

    // Auto-reset event: ship/planet turn workers request a player conversation.
    extern std::uint32_t TalkRequestEvent;

    // Auto-reset event signaled when the star-map UI returns from conversation.
    extern std::uint32_t TalkCompletedEvent;

    // Raised by the turn thread when requesting a UI-side conversation.
    extern std::uint32_t ScriptUiRequestEvent;

    // Aborts a pending turn-thread conversation wait; the worker returns False.
    extern std::uint32_t ScriptUiAbortEvent;

    // Owns TPlanetTempl instances.
    extern pas::List* PlanetRenderTemplates;

    // Film/star-map draw cadence; reset by manual minimap scrolling.
    extern std::int32_t MinimapFrameCounter;

    extern std::uint8_t Skip1C;

    extern std::uint8_t SkipVideo;

    extern std::uint8_t SkipIntro;

    extern fGameSettings::TThreadCreateNewGame* NewGameGenerationThread;

    // Bit mask; native shifts use the low five bits of the tip index.
    extern std::uint32_t ShownPlayerTips;

    // Toggled by TfStarMap.ToggleWeaponPanelClicked; reset on arcade exit.
    extern std::uint8_t StarMapWeaponPanelOpen;

    extern aEFilm::TEFilm* PrimaryFilm;

    extern aEFilm::TEFilm* SecondaryFilm;

    // Native shared trailing-effect owner.
    extern aEFilmEnd::TEFilmEnd* TrailingFilmEffects;

    extern SE_Process::TProcessSE* SpaceProcess;

    // Borrowed during LoadGameFromFile; exposed for progress reporting.
    extern EC_Buf::TBufEC* ActiveLoadBuffer;

    extern pas::CriticalSection* PersistentPlayerMessageLock;

    extern Globals::TMessagePlayer* FirstPersistentPlayerMessage;

    extern Globals::TMessagePlayer* LastPersistentPlayerMessage;

    // ABSound.Explosion values.
    extern pas::DynArray<pas::WideString> ArcadeExplosionSounds;

    // ABSound.Item values.
    extern pas::DynArray<pas::WideString> ArcadeItemSounds;

    // ABSound.Hit values.
    extern pas::DynArray<pas::WideString> ArcadeHitSounds;

    // ABSound.WeaponFirst.
    extern pas::Array<pas::WideString, 0, 17> ArcadeWeaponFirstSounds;

    // ABSound.WeaponLoop entries, after the time value.
    extern pas::Array<pas::WideString, 0, 17> ArcadeWeaponLoopSounds;

    // First configured value divided by 20; -1 when absent.
    extern pas::Array<std::int32_t, 0, 17> ArcadeWeaponLoopTicks;

    // Retained SE.Ship templates indexed by race and six ordinary ship kinds.
    extern pas::Array<pas::Array<SE_Space::TObjectSE*, 0, 5>, 0, 7> RaceShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> BlazerShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> KellerShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> TerronShipTemplates;

    extern pas::Array<SE_Space::TObjectSE*, 0, 7> PirateClanShipTemplates;

    extern pas::DynArray<Globals::TPlanetSpaceTemplate> PlanetSpaceTemplates;

    // UselessItems.CntRemains.
    extern std::int32_t UselessItemRemainsCount;

    extern pas::DynArray<Globals::TRobotMap> RobotMapDefinitions;

    extern pas::DynArray<Globals::TShipGreetingsInfo> ShipGreetingDefinitions;

    extern std::int32_t ShipGreetingCount;

    extern pas::DynArray<Globals::TGovGreetingsInfo> GovernmentGreetingDefinitions;

    extern std::int32_t GovernmentGreetingCount;

    extern pas::DynArray<Globals::TPlanetAdvtGroup> PlanetAdvertDefinitions;

    extern pas::Array<pas::WideString, 0, 10> ScriptVariableTypeNames;

    // Case-sensitive; returns -1 when absent. Requires the template list. Native callers include UI loading and script builtins.
    std::int32_t FindScriptTemplateIndex(const std::u16string_view& Name);

    // Clears Dest, borrows templates with ActiveScriptIndex < 0, then performs twice Count seeded swaps. Chaotic RNG mode ignores the seeds.
    void CollectInactiveScriptTemplates(pas::List* Dest);

    void RecreateSpaceProcess(const pas::WideString& ConfigName);

    // Sets the shown bit and enqueues localized Tips.00-style player text; returns whether a new tip was shown.
    std::uint8_t ShowPlayerTipOnce(std::int32_t Index);

    std::uint8_t HasShownPlayerTip(std::int32_t Index);

    // Weighted selection restricted to Kind; returns 0 if no weight is available.
    std::int32_t SelectSpaceImageTemplateFromSeed(std::int32_t Kind, std::uint32_t Seed);

    std::int32_t SelectSpaceImageTemplate(std::int32_t Kind);

    // Raises when no template matches the two SE.Planet.Style values.
    std::int32_t FindPlanetSpaceTemplateIndex(std::int32_t Style, std::int32_t StyleVariant);

    // Retained empty ExitScreenLoop test; assigned by Rangers.start to an otherwise unread hook.
    void HandleRuntimeExitCheck1();

    // Retained empty ExitScreenLoop test; assigned by Rangers.start to an otherwise unread hook.
    void HandleRuntimeExitCheck2();

    void InitializeScriptHostRuntime();

    void FinalizeScriptHostRuntime();

    aGalaxyStruct::TOwnerMask ParseRobotMapRaceMask(pas::WideString Text);

    // Returns -1 for an unknown map.
    std::int32_t FindRobotMapById(std::int32_t MapId);

    void InitializeRobotMapDefinitions();

    void InitializeShipGreetingDefinitions();

    void InitializeGovernmentGreetingDefinitions();

    void InitializePlanetAdvertDefinitions();

    void InitializeGlobalUiRuntime();

    void FinalizeGlobalUiRuntime();

    void ResetScriptHostRuntimeState();

    // Case-sensitive; returns nil when absent.
    GI_MessageLoop::TMessageLoopGI* FindMessageLoop(const std::u16string_view& Name);

    // Consumes RequestedScreenId before each run; zero ends dispatch. StarMap, Film and arcade battle use RunContinuous.
    void RunMainScreenStateLoop();

    // Exchanges the producer and playback films.
    void SwapTurnFilms();

    // Follows ChildLoop from the current registered screen.
    GI_MessageLoop::TMessageLoopGI* GetInnermostScreenLoop();

    // Key is an optional substring; returned queue node is borrowed.
    TMessagePlayer* FindPlayerMessageExceptKinds(const pas::WideString& Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock);

    // Returns whether any queued message was removed.
    std::uint8_t RemovePlayerMessagesExceptKinds(pas::WideString Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock);

    void ClearPersistentPlayerMessages();

    std::int32_t CountPersistentPlayerMessages();

    std::uint8_t IsPersistentPlayerMessageQueued(TMessagePlayer* MessageEntry, std::uint8_t SkipLock);

    // Requires a queued node; unlinks and frees it.
    void RemovePersistentPlayerMessage(TMessagePlayer* MessageEntry, std::uint8_t SkipLock);

    // Returns a borrowed queue node or nil.
    TMessagePlayer* FindPlayerBubbleByText(const pas::WideString& Text, std::uint8_t SkipLock);

    // Returns a borrowed queue node or nil.
    TMessagePlayer* FindPlayerBubbleByKey(const pas::WideString& Key, std::uint8_t SkipLock);

    // Removes matching prefix keys whose integer suffix is at least FirstPage; leaves unnumbered keys alone.
    void RemovePlayerBubblePages(const pas::WideString& Prefix, std::int32_t FirstPage);

    // Removes only the first exact match.
    void RemovePlayerBubbleByKey(const pas::WideString& Key);

    // Appends a new node owned by the global message queue.
    TMessagePlayer* CreatePersistentPlayerMessage();

    // Returns a borrowed queue node. An existing key updates kind/turn and nonempty text; otherwise an exact text match is returned unchanged.
    TMessagePlayer* AddOrUpdatePlayerBubble(std::uint8_t Kind, std::int32_t Turn, const pas::WideString& Text, const pas::WideString& Key);

    void PruneExpiredPersistentPlayerMessages();

    // Targets are serialized as all three ship IDs, then all three planet IDs.
    // Prev, Next and Button are not serialized; both flags are persistent.
    // Does not link the object into the global message queue.
    void TMessagePlayer_Create(TMessagePlayer* Self);

    void TScriptTemplUnit_Create(TScriptTemplUnit* Self);

    void TScriptTemplUnit_Destroy(TScriptTemplUnit* Self);

} // namespace Globals
