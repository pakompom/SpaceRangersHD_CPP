#include "layout/Globals.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_CacheGAI.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Image.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aEFilmEnd.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "types/ab_Global.hpp"
#include "types/fAbout.hpp"
#include "types/fCfgSettings.hpp"
#include "types/fFilm.hpp"
#include "types/fGameEnd.hpp"
#include "types/fGameMenu.hpp"
#include "types/fGameSettings.hpp"
#include "types/fGameSettings2.hpp"
#include "types/fIntroduction.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fRewards.hpp"
#include "types/fScaner.hpp"
#include "types/fSelectFace.hpp"
#include "types/fTalk.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/PopUp.hpp"
#include "units/Robot.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/aConst.hpp"
#include "units/aEFilm.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPath.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/ab_MainForm.hpp"
#include "units/fAchievements.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fFilmFile.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fGameLoad.hpp"
#include "units/fGoodsShop2.hpp"
#include "units/fGov.hpp"
#include "units/fHangar.hpp"
#include "units/fInfo.hpp"
#include "units/fJournal.hpp"
#include "units/fJump.hpp"
#include "units/fLoad.hpp"
#include "units/fLoadAB.hpp"
#include "units/fLoadQuest.hpp"
#include "units/fLoadRobot.hpp"
#include "units/fMainForm.hpp"
#include "units/fPlanet.hpp"
#include "units/fPlanetNO.hpp"
#include "units/fRating2.hpp"
#include "units/fRuinsTalk.hpp"
#include "units/fSaveManager.hpp"
#include "units/fScore.hpp"
#include "units/fShip2.hpp"
#include "units/fStarMap.hpp"

namespace Globals {
    // Native image defaults and turn lifetimes.
    pas::Array<Globals::TMessagePlayerTypeGraph, 0, 10> PlayerMessagePresentations = pas::Array<Globals::TMessagePlayerTypeGraph, 0, 10>{{
        {.NormalImage = u"GalaxyN"_w, .ActiveImage = u"GalaxyA"_w, .PressedImage = u"GalaxyD"_w, .LifetimeTurns = 10},
        {.NormalImage = u"EtherN"_w, .ActiveImage = u"EtherA"_w, .PressedImage = u"EtherD"_w},
        {.NormalImage = u"ShipPlusN"_w, .ActiveImage = u"ShipPlusA"_w, .PressedImage = u"ShipPlusD"_w, .LifetimeTurns = 5},
        {.NormalImage = u"QuestNormalN"_w, .ActiveImage = u"QuestNormalA"_w, .PressedImage = u"QuestNormalD"_w, .LifetimeTurns = Globals::PersistentMessageLifetimeTurns},
        {.NormalImage = u"QuestOkN"_w, .ActiveImage = u"QuestOkA"_w, .PressedImage = u"QuestOkD"_w, .LifetimeTurns = Globals::PersistentMessageLifetimeTurns},
        {.NormalImage = u"QuestCancelN"_w, .ActiveImage = u"QuestCancelA"_w, .PressedImage = u"QuestCancelD"_w, .LifetimeTurns = Globals::PersistentMessageLifetimeTurns},
        {.NormalImage = u"TipsN"_w, .ActiveImage = u"TipsA"_w, .PressedImage = u"TipsD"_w, .LifetimeTurns = 182},
        {.NormalImage = u"UserN"_w, .ActiveImage = u"UserA"_w, .PressedImage = u"UserD"_w, .LifetimeTurns = Globals::PersistentMessageLifetimeTurns},
        {.NormalImage = u"ShipMinusN"_w, .ActiveImage = u"ShipMinusA"_w, .PressedImage = u"ShipMinusD"_w, .LifetimeTurns = 5},
        {.NormalImage = u"StorageN"_w, .ActiveImage = u"StorageA"_w, .PressedImage = u"StorageD"_w, .LifetimeTurns = Globals::PersistentMessageLifetimeTurns},
        {.NormalImage = u"Ether2N"_w, .ActiveImage = u"Ether2A"_w, .PressedImage = u"Ether2D"_w},
    }};

    fMainForm::TfMainForm* MainMenuScreen{};

    fGameSettings2::TfGameSettings2* NewGameScreen{};

    fIntroduction::TfIntroduction* IntroductionScreen{};

    fHangar::TfHangar* HangarScreen{};

    fPlanet::TfPlanet* PlanetScreen{};

    fPlanetNO::TfPlanetNO* UninhabitedPlanetScreen{};

    fPlanetQuest::TfPlanetQuest* PlanetQuestScreen{};

    fRuinsTalk::TfRuinsTalk* RuinsTalkScreen{};

    ab_MainForm::TfAB* ArcadeBattleScreen{};

    fEquipmentShop::TfEquipmentShop* EquipmentShopScreen{};

    fGoodsShop2::TfGoodsShop2* GoodsShopScreen{};

    fGov::TfGov* GovernmentScreen{};

    fInfo::TfInfo* InfoScreen{};

    fRating2::TfRating2* RangerRatingScreen{};

    fRewards::TfRewards* RewardsScreen{};

    fShip2::TfShip2* ShipScreen{};

    fTalk::TfTalk* TalkScreen{};

    fScaner::TfScaner* ScannerScreen{};

    fStarMap::TfStarMap* StarMapScreen{};

    fFilm::TfFilm* FilmScreen{};

    fGalaxy2::TfGalaxy2* GalaxyScreen{};

    fJump::TfJump* JumpScreen{};

    fLoad::TfLoad* LoadScreen{};

    fSaveManager::TfSaveManager* SaveManagerScreen{};

    fGameLoad::TfGameLoad* GameLoadScreen{};

    fGameMenu::TfGameMenu* GameMenuScreen{};

    fCfgSettings::TfCfgSettings* SettingsScreen{};

    fGameEnd::TfGameEnd* GameEndScreen{};

    fAbout::TfAbout* AboutScreen{};

    fScore::TfScore* ScoreScreen{};

    fSelectFace::TfSelectFace* SelectFaceScreen{};

    // Unregistered loop centered on the back buffer; hosts space-object controls.
    GI_MessageLoop::TMessageLoopGI* SpaceObjectUiLoop{};

    fJournal::TfJournal* JournalScreen{};

    fLoadRobot::TfLoadRobot* LoadRobotScreen{};

    fLoadQuest::TfLoadQuest* LoadQuestScreen{};

    fLoadAB::TfLoadAB* LoadArcadeScreen{};

    // Native UI shutdown does not free this screen.
    fAchievements::TfAchievements* AchievementsScreen{};

    // Map scroll offset and scene sound attenuation origin.
    EC_Struct::TPointF SpaceViewPosition{};

    std::uint8_t FilmCameraFollow{};

    aShip::TShip* TalkShip{};

    // Planet dialogue target, assigned by TPlanet.RequestDialog.
    aPlanet::TPlanet* TalkPlanet{};

    // Set for the scripted Keller dialogue.
    std::uint8_t TalkScripted{};

    // tk* conversation ID set by TShip.ShowPlayerDialogue and exposed by SF_GetTalkType.
    aGalaxyStruct::TTalkKind TalkType{};

    // Negotiated amount; ShowPlayerDialogue overwrites it only for positive inputs.
    std::int32_t TalkAmount{};

    // Response selected by the conversation UI.
    std::uint8_t TalkResponse{};

    // Message supplied to the conversation UI.
    pas::WideString TalkText{};

    // Item currently executing OnUse; native runner clears it after success.
    aItem::TItem* ScriptUseItem{};

    pas::List* ScriptItemContextStack{};

    pas::List* ScriptItemInfoContextStack{};

    pas::List* ScriptActionShipStack{};

    pas::List* ScriptActionObject1Stack{};

    pas::List* ScriptActionObject2Stack{};

    pas::List* ScriptActionParamStack{};

    pas::List* ScriptActionTypeStack{};

    // Startup sets 0, or 3 for a screen with composite loading assets.
    std::uint8_t ScreenLoadMode{};

    // Owns native TScriptTemplUnit entries.
    pas::List* ScriptTemplates{};

    // Shared scope used by script compilation, execution and text-quest external parameters.
    EC_Expression::TVarArrayEC* SharedScriptVariables{};

    // Persistent script globals, initially GRunFrom and GRunStar.
    EC_Expression::TVarArrayEC* GlobalScriptVariables{};

    // Set by script condition code; checked after running each inactive template.
    std::uint8_t ScriptTemplateStartRequested{};

    // Assigned by TPlayer.LoadFromBuffer; broader UI-cache role unresolved.
    pas::WideString LastLoadedPlayerName{};

    // Native initial value is True. Reset sets this flag; UI initialization reloads script and ship templates then clears it.
    std::uint8_t ReloadScriptTemplates = true;

    std::uint8_t ReloadModsRequested = false;

    // The quest selector sets True; planet/government/script launches set False. Guards campaign quest checks and turn advancement.
    std::uint8_t StandaloneQuestMode = false;

    // Borrowed target passed from the star map to the scanner screen.
    pas::Object* ScannerTarget = nullptr;

    // Selected script dialogue; -1 while resolving a dialogue variable.
    std::int32_t ScriptDialogIndex = -1;

    // Borrowed ship selected by inventory, scanner or ranger ranking for the medals screen.
    pas::Object* AwardSubject = nullptr;

    std::uint8_t PlayerStarDayPrepared = false;

    std::uint32_t PreviousFilmActivity = 0u;

    // Temporarily disabled while the film slider seeks through steps.
    std::uint8_t FilmSoundEffectsEnabled = true;

    // Owned here; ThreadCalc and Rangers access it through the imported reference cell.
    ThreadCalc::TThreadCalc* TurnCalculationThread = nullptr;

    fFilmFile::TFilmFile* FilmHistory = nullptr;

    fLoad::TCacheLoader* CacheLoader = nullptr;

    fSaveManager::TSaveManagerMode SaveManagerMode = fSaveManager::smmLoad;

    // Auto-reset event: ship/planet turn workers request a player conversation.
    std::uint32_t TalkRequestEvent = 0u;

    // Auto-reset event signaled when the star-map UI returns from conversation.
    std::uint32_t TalkCompletedEvent = 0u;

    // Raised by the turn thread when requesting a UI-side conversation.
    std::uint32_t ScriptUiRequestEvent = 0u;

    // Aborts a pending turn-thread conversation wait; the worker returns False.
    std::uint32_t ScriptUiAbortEvent = 0u;

    // Owns TPlanetTempl instances.
    pas::List* PlanetRenderTemplates = nullptr;

    // Film/star-map draw cadence; reset by manual minimap scrolling.
    std::int32_t MinimapFrameCounter = 0;

    std::uint8_t Skip1C = false;

    std::uint8_t SkipVideo = false;

    std::uint8_t SkipIntro = false;

    fGameSettings::TThreadCreateNewGame* NewGameGenerationThread{};

    // Bit mask; native shifts use the low five bits of the tip index.
    std::uint32_t ShownPlayerTips{};

    // Toggled by TfStarMap.ToggleWeaponPanelClicked; reset on arcade exit.
    std::uint8_t StarMapWeaponPanelOpen{};

    aEFilm::TEFilm* PrimaryFilm{};

    aEFilm::TEFilm* SecondaryFilm{};

    // Native shared trailing-effect owner.
    aEFilmEnd::TEFilmEnd* TrailingFilmEffects{};

    SE_Process::TProcessSE* SpaceProcess{};

    // Borrowed during LoadGameFromFile; exposed for progress reporting.
    EC_Buf::TBufEC* ActiveLoadBuffer{};

    pas::CriticalSection* PersistentPlayerMessageLock{};

    Globals::TMessagePlayer* FirstPersistentPlayerMessage{};

    Globals::TMessagePlayer* LastPersistentPlayerMessage{};

    // ABSound.Explosion values.
    pas::DynArray<pas::WideString> ArcadeExplosionSounds{};

    // ABSound.Item values.
    pas::DynArray<pas::WideString> ArcadeItemSounds{};

    // ABSound.Hit values.
    pas::DynArray<pas::WideString> ArcadeHitSounds{};

    // ABSound.WeaponFirst.
    pas::Array<pas::WideString, 0, 17> ArcadeWeaponFirstSounds{};

    // ABSound.WeaponLoop entries, after the time value.
    pas::Array<pas::WideString, 0, 17> ArcadeWeaponLoopSounds{};

    // First configured value divided by 20; -1 when absent.
    pas::Array<std::int32_t, 0, 17> ArcadeWeaponLoopTicks{};

    // Retained SE.Ship templates indexed by owner and six ordinary hull kinds.
    pas::Array<pas::Array<SE_Space::TObjectSE*, 0, 5>, 0, 7> RaceShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> BlazerShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> KellerShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> TerronShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> PirateClanShipTemplates{};

    pas::DynArray<Globals::TPlanetSpaceTemplate> PlanetSpaceTemplates{};

    // UselessItems.CntRemains.
    std::int32_t UselessItemRemainsCount{};

    pas::DynArray<Globals::TRobotMap> RobotMapDefinitions{};

    pas::DynArray<Globals::TShipGreetingsInfo> ShipGreetingDefinitions{};

    std::int32_t ShipGreetingCount{};

    pas::DynArray<Globals::TGovGreetingsInfo> GovernmentGreetingDefinitions{};

    std::int32_t GovernmentGreetingCount{};

    pas::DynArray<Globals::TPlanetAdvtGroup> PlanetAdvertDefinitions{};

    pas::Array<pas::WideString, 0, 10> ScriptVariableTypeNames = pas::Array<pas::WideString, 0, 10>{{
        u"Unknown"_w, u"Int"_w, u"DW"_w, u"Float"_w, u"Str"_w, u"ExternFun"_w, u"LibraryFun"_w, u"Fun"_w,
        u"Class"_w, u"Array"_w, u"Ref"_w,
    }};

    // Case-sensitive; returns -1 when absent. Requires the template list. Native callers include UI loading and script builtins.
    std::int32_t FindScriptTemplateIndex(const std::u16string_view& Name) {
        TScriptTemplUnit* Item{};
        std::int32_t Index{};
        std::int32_t Count = pas::list_count(ScriptTemplates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<TScriptTemplUnit>(ScriptTemplates, Index);
            if (pas::view(Item->Name) == Name) {
                return Index;
            }
        }
        return -1;
    }

    // Clears Dest, borrows templates with ActiveScriptIndex < 0, then performs twice Count seeded swaps. Chaotic RNG mode ignores the seeds.
    void CollectInactiveScriptTemplates(pas::List* Dest) {
        TScriptTemplUnit* Item{};
        std::int32_t Index{};
        std::int32_t First{};
        std::int32_t Second{};
        pas::list_clear(Dest);
        std::int32_t Count = pas::list_count(ScriptTemplates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<TScriptTemplUnit>(ScriptTemplates, Index);
            if (Item->ActiveScriptIndex < 0) {
                pas::list_add(Dest, reinterpret_cast<void*>(Item));
            }
        }
        if (pas::list_count(Dest) >= 2) {
            Count = pas::list_count(Dest) * 2;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
                First = aMyFunction::SeededRandomIntRange(0, pas::list_count(Dest) - 1, aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) * static_cast<std::uint32_t>(Index));
                Second = aMyFunction::SeededRandomIntRange(0, pas::list_count(Dest) - 1, aGalaxy::Galaxy->GenerationSeed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn + Index));
                if (First != Second) {
                    Item = pas::list_at<TScriptTemplUnit>(Dest, First);
                    pas::list_put(Dest, First, pas::list_get(Dest, Second));
                    pas::list_put(Dest, Second, reinterpret_cast<void*>(Item));
                }
            }
        }
    }

    void RecreateSpaceProcess(const pas::WideString& ConfigName) {
        if (SpaceProcess != nullptr) {
            pas::free(SpaceProcess);
            SpaceProcess = nullptr;
        }
        SpaceProcess = pas::construct_call<SE_Process::TProcessSE>(SE_Process::TProcessSE_Create, ConfigName);
    }

    // Sets the shown bit and enqueues localized Tips.00-style player text; returns whether a new tip was shown.
    std::uint8_t ShowPlayerTipOnce(std::int32_t Index) {
        std::uint8_t Result = false;
        if ((pas::shr(ShownPlayerTips, Index) & 1) == 0) {
            ShownPlayerTips |= pas::shl(1, Index);
            if (Index < 10) {
                const pas::WideString& localizedColorText = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Tips.0", SysUtils::IntToStr(Index)})));
                std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(pmTip, currentTurn, localizedColorText, u""_wref.get());
            } else {
                const pas::WideString& localizedColorText_2 = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Tips.", SysUtils::IntToStr(Index)})));
                std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(pmTip, currentTurn_2, localizedColorText_2, u""_wref.get());
            }
            return true;
        }
        return Result;
    }

    std::uint8_t HasShownPlayerTip(std::int32_t Index) {
        return (pas::shr(ShownPlayerTips, Index) & 1) != 0;
    }

    // Weighted selection restricted to Kind; returns 0 if no weight is available.
    std::int32_t SelectSpaceImageTemplateFromSeed(std::int32_t Kind, std::uint32_t Seed) {
        std::int32_t Index{};
        std::int32_t Weight = 0;
        {
            const std::int32_t cpp_last = GlobalsV::SpaceImageTemplates.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (GlobalsV::SpaceImageTemplates[Index].Kind == Kind) {
                        Weight += GlobalsV::SpaceImageTemplates[Index].Weight;
                    }
                }
            }
        }
        if (Weight == 0) {
            return 0;
        }
        Weight = aMyFunction::SeededRandomIntRange(0, Weight - 1, Seed);
        {
            const std::int32_t cpp_last_2 = GlobalsV::SpaceImageTemplates.length() - 1;
            if (0 <= cpp_last_2) {
                for (Index = 0; Index <= cpp_last_2; ++Index) {
                    if (GlobalsV::SpaceImageTemplates[Index].Kind == Kind) {
                        Weight -= GlobalsV::SpaceImageTemplates[Index].Weight;
                        if (Weight < 0) {
                            return Index;
                        }
                    }
                }
            }
        }
        return 0;
    }

    std::int32_t SelectSpaceImageTemplate(std::int32_t Kind) {
        return Globals::SelectSpaceImageTemplateFromSeed(Kind, aMyFunction::RandomIntRange(0, 2000000000));
    }

    // Raises when no template matches the two SE.Planet.Style values.
    std::int32_t FindPlanetSpaceTemplateIndex(std::int32_t Style, std::int32_t StyleVariant) {
        std::int32_t Index{};
        {
            const std::int32_t cpp_last = PlanetSpaceTemplates.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (PlanetSpaceTemplates[Index].Style == Style && PlanetSpaceTemplates[Index].StyleVariant == StyleVariant) {
                        return Index;
                    }
                }
            }
        }
        std::int32_t Result = -1;
        GR_Main::RaiseWideMessage(u"find planet"_wref.get());
        return Result;
    }

    // Retained empty ExitScreenLoop test; assigned by Rangers.start to an otherwise unread hook.
    void HandleRuntimeExitCheck1() {
        static_cast<void>(GR_Main::ExitScreenLoop);
    }

    // Retained empty ExitScreenLoop test; assigned by Rangers.start to an otherwise unread hook.
    void HandleRuntimeExitCheck2() {
        static_cast<void>(GR_Main::ExitScreenLoop);
    }

    void InitializeScriptHostRuntime() {
        PersistentPlayerMessageLock = pas::make_critical_section<pas::CriticalSection>();
        aSaveLoad::SaveLoadLock = pas::make_critical_section<pas::CriticalSection>();
        WindowsImports::GetLastError();
        aPath::InitializePathNodePool();
        FilmHistory = pas::construct_call<fFilmFile::TFilmFile>(fFilmFile::TFilmFile_Create);
        TalkRequestEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        TalkCompletedEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        ScriptUiRequestEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
        ScriptUiAbortEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
        TurnCalculationThread = pas::construct_call<ThreadCalc::TThreadCalc>(EC_Thread::TThreadEC_Create);
        TurnCalculationThread->SetPriority(2);
        ScriptTemplates = pas::make_object<pas::List>();
        GlobalScriptVariables = pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create);
        SharedScriptVariables = pas::construct_call<EC_Expression::TVarArrayEC>(EC_Expression::TVarArrayEC_Create);
        GlobalScriptVariables->Add(u"GRunFrom"_wref.get(), EC_Expression::vkInt)->SetInt(0);
        GlobalScriptVariables->Add(u"GRunStar"_wref.get(), EC_Expression::vkDword)->SetInt(0);
        aScript::InitializeScriptEngine();
    }

    void FinalizeScriptHostRuntime() {
        aGalaxyStruct::TOwnerId Race{};
        aGalaxyStruct::THullType Kind{};
        aGalaxyStruct::TKlingType KlingKind{};
        pas::Object* Item{};
        std::int32_t Index{};
        aScript::FinalizeScriptEngine();
        if (GlobalScriptVariables != nullptr) {
            pas::free(GlobalScriptVariables);
            GlobalScriptVariables = nullptr;
        }
        if (SharedScriptVariables != nullptr) {
            pas::free(SharedScriptVariables);
            SharedScriptVariables = nullptr;
        }
        if (ScriptTemplates != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ScriptTemplates) - 1); cpp_range.next(Index); ) {
                Item = pas::list_at<pas::Object>(ScriptTemplates, Index);
                pas::free(Item);
            }
            pas::free(ScriptTemplates);
            ScriptTemplates = nullptr;
        }
        if (TurnCalculationThread != nullptr) {
            pas::free(TurnCalculationThread);
            TurnCalculationThread = nullptr;
        }
        if (TalkRequestEvent != 0) {
            WindowsImports::CloseHandle(TalkRequestEvent);
            TalkRequestEvent = 0u;
        }
        if (TalkCompletedEvent != 0) {
            WindowsImports::CloseHandle(TalkCompletedEvent);
            TalkCompletedEvent = 0u;
        }
        if (ScriptUiRequestEvent != 0) {
            WindowsImports::CloseHandle(ScriptUiRequestEvent);
            ScriptUiRequestEvent = 0u;
        }
        if (ScriptUiAbortEvent != 0) {
            WindowsImports::CloseHandle(ScriptUiAbortEvent);
            ScriptUiAbortEvent = 0u;
        }
        for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_2.next(Race); ) {
            for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::THullType>(aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat); cpp_range_3.next(Kind); ) {
                if (RaceShipTemplates[Race][Kind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][Kind]));
                }
            }
            if (PirateClanShipTemplates[Race] != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]));
            }
        }
        for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_4.next(KlingKind); ) {
            if (KlingKind != aGalaxyStruct::ktBoss) {
                if (BlazerShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[KlingKind]));
                }
                if (KellerShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[KlingKind]));
                }
                if (TerronShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[KlingKind]));
                }
            }
        }
        if (FilmHistory != nullptr) {
            pas::free(FilmHistory);
            FilmHistory = nullptr;
        }
        aPath::FinalizePathNodePool();
        if (aSaveLoad::SaveLoadLock != nullptr) {
            pas::free(aSaveLoad::SaveLoadLock);
            aSaveLoad::SaveLoadLock = nullptr;
        }
        if (PersistentPlayerMessageLock != nullptr) {
            pas::free(PersistentPlayerMessageLock);
            PersistentPlayerMessageLock = nullptr;
        }
    }

    aGalaxyStruct::TOwnerMask ParseRobotMapRaceMask(pas::WideString Text) {
        aGalaxyStruct::TOwnerMask Result{};
        pas::AnsiString Names{};
        Result = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
        if (Text != u"" && Text != u"Any") {
            Names = static_cast<pas::AnsiString>(Text);
            if (pas::pos("Maloc", Names) > 0) {
                pas::include_at(&Result, aGalaxyStruct::oiMaloc);
            }
            if (pas::pos("Peleng", Names) > 0) {
                pas::include_at(&Result, aGalaxyStruct::oiPeleng);
            }
            if (pas::pos("People", Names) > 0) {
                pas::include_at(&Result, aGalaxyStruct::oiHuman);
            }
            if (pas::pos("Fei", Names) > 0) {
                pas::include_at(&Result, aGalaxyStruct::oiFeyan);
            }
            if (pas::pos("Gaal", Names) > 0) {
                pas::include_at(&Result, aGalaxyStruct::oiGaal);
            }
        }
        return Result;
    }

    // Returns -1 for an unknown map.
    std::int32_t FindRobotMapById(std::int32_t MapId) {
        std::int32_t Index{};
        {
            const std::int32_t cpp_last = RobotMapDefinitions.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (RobotMapDefinitions[Index].Id == MapId) {
                        return Index;
                    }
                }
            }
        }
        return -1;
    }

    void InitializeRobotMapDefinitions() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* Root{};
        std::int32_t Previous{};
        std::int32_t Index{};
        pas::WideString Text{};
        // Nested in InitializeRobotMapDefinitions; joins repeated fields with CRLF.
        auto ReadMapText = [&](const pas::WideString& Path) -> pas::WideString {
            pas::WideString Result{};
            std::int32_t Part{};
            std::int32_t PartCount = Block->CountParamsByPath(Path);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, PartCount - 1); cpp_range.next(Part); ) {
                if (Result != u"") {
                    Result = pas::concat_wide({Result, u"\r\n"});
                }
                Result = pas::concat_wide({Result, Block->GetParamByPath(pas::concat_wide({Path, u":", pas::wide_int_to_str(Part)}))});
            }
            return Result;
        };
        Root = GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"sv);
        std::int32_t Count = Root->GetBlockCount();
        RobotMapDefinitions.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            RobotMapDefinitions[Index].Id = EC_Str::ExtractDigitsToIntW(pas::view(Root->GetBlockNameByIndex(Index)));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Index - 1); cpp_range_2.next(Previous); ) {
                if (RobotMapDefinitions[Index].Id == RobotMapDefinitions[Previous].Id) {
                    GR_Main::RaiseWideMessage(u"RobotMap.Id"_wref.get());
                }
            }
            Block = Root->GetBlockByIndex(Index);
            RobotMapDefinitions[Index].Name = ReadMapText(u"Name"_wref.get());
            RobotMapDefinitions[Index].Map = ReadMapText(u"Map"_wref.get());
            Text = ReadMapText(u"Group"_wref.get());
            if (Text != u"") {
                RobotMapDefinitions[Index].Group = EC_Str::ExtractSignedDigitsToIntW(pas::view(ReadMapText(u"Group"_wref.get())));
            } else {
                RobotMapDefinitions[Index].Group = -1;
            }
            RobotMapDefinitions[Index].Access = EC_Str::ExtractSignedDigitsToIntW(pas::view(ReadMapText(u"Access"_wref.get())));
            Text = ReadMapText(u"Side"_wref.get());
            RobotMapDefinitions[Index].Side = 0;
            if (pas::pos("Red", static_cast<pas::AnsiString>(Text)) > 0) {
                RobotMapDefinitions[Index].Side = RobotMapDefinitions[Index].Side | 1;
            }
            if (pas::pos("Green", static_cast<pas::AnsiString>(Text)) > 0) {
                RobotMapDefinitions[Index].Side = RobotMapDefinitions[Index].Side | 2;
            }
            if (pas::pos("Blue", static_cast<pas::AnsiString>(Text)) > 0) {
                RobotMapDefinitions[Index].Side = RobotMapDefinitions[Index].Side | 4;
            }
            RobotMapDefinitions[Index].Length = EC_Str::ExtractSignedDigitsToIntW(pas::view(ReadMapText(u"Length"_wref.get())));
            Text = ReadMapText(u"PlanetRace"_wref.get());
            RobotMapDefinitions[Index].PlanetRace = Globals::ParseRobotMapRaceMask(Text);
            Text = ReadMapText(u"PlayerRace"_wref.get());
            RobotMapDefinitions[Index].PlayerRace = Globals::ParseRobotMapRaceMask(Text);
            Text = ReadMapText(u"PlayerStatus"_wref.get());
            RobotMapDefinitions[Index].PlayerStatus = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({});
            if (Text != u"" && Text != u"Any") {
                if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, aGalaxyStruct::rcTrader);
                }
                if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, aGalaxyStruct::rcPirate);
                }
                if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, aGalaxyStruct::rcWarrior);
                }
            }
            RobotMapDefinitions[Index].MinWins = EC_Str::ExtractSignedDigitsToIntW(pas::view(ReadMapText(u"MinWins"_wref.get())));
            RobotMapDefinitions[Index].MaxWins = EC_Str::ExtractSignedDigitsToIntW(pas::view(ReadMapText(u"MaxWins"_wref.get())));
            RobotMapDefinitions[Index].Reiteration = EC_Str::ExtractDigitsToIntW(pas::view(ReadMapText(u"Reiteration"_wref.get())));
            RobotMapDefinitions[Index].ReinforcementsDisabled = GI_Main::ParseEnabledNameGI(pas::view(ReadMapText(u"ReinforcementsDisabled"_wref.get())));
            RobotMapDefinitions[Index].Terron = GI_Main::ParseEnabledNameGI(pas::view(ReadMapText(u"Terron"_wref.get())));
            RobotMapDefinitions[Index].Demo = GI_Main::ParseEnabledNameGI(pas::view(ReadMapText(u"Demo"_wref.get())));
            RobotMapDefinitions[Index].AfterLiberation = GI_Main::ParseEnabledNameGI(pas::view(ReadMapText(u"AfterLiberation"_wref.get())));
            RobotMapDefinitions[Index].GovTextStart = ReadMapText(u"GovTextStart"_wref.get());
            RobotMapDefinitions[Index].GovTextWin = ReadMapText(u"GovTextWin"_wref.get());
            RobotMapDefinitions[Index].GovTextLoss = ReadMapText(u"GovTextLoss"_wref.get());
            RobotMapDefinitions[Index].RobotsStart = ReadMapText(u"RobotsStart"_wref.get());
            RobotMapDefinitions[Index].RobotsWin = ReadMapText(u"RobotsWin"_wref.get());
            RobotMapDefinitions[Index].RobotsLoss = ReadMapText(u"RobotsLoss"_wref.get());
            RobotMapDefinitions[Index].FromAuthor = ReadMapText(u"FromAuthor"_wref.get());
        }
    }

    void InitializeShipGreetingDefinitions() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        std::int32_t EntryIndex{};
        std::int32_t Item{};
        pas::WideString Text{};
        auto ReadShipGreetingField = [&](const pas::WideString& FieldName) -> pas::WideString {
            if (Block->CountParams(FieldName) > 0) {
                return Block->GetParam(pas::view(FieldName));
            }
            return pas::WideString();
        };
        ShipGreetingCount = 0;
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"ShipGreetings.CountShipGreetings"_wref.get())));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"ShipGreetings"sv);
                return block->CountBlocks(intToStr);
            }()) > 0) {
                ++ShipGreetingCount;
            }
        }
        ShipGreetingDefinitions.set_length(ShipGreetingCount);
        ShipGreetingCount = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block_2 = GR_Main::LanguageDataConfig->GetBlock(u"ShipGreetings"sv);
                return block_2->CountBlocks(intToStr_2);
            }()) != 0) {
                ++ShipGreetingCount;
                EntryIndex = ShipGreetingCount - 1;
                Block = GR_Main::LanguageDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"ShipGreetings.", SysUtils::IntToStr(Index)})));
                {
                    TShipGreetingsInfo& cpp_with = ShipGreetingDefinitions[EntryIndex];
                    cpp_with.Name = pas::wide_int_to_str(Index);
                    Text = ReadShipGreetingField(u"Priority"_wref.get());
                    if (Text == u"") {
                        cpp_with.Priority = 10;
                    } else {
                        cpp_with.Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
                    }
                    Text = ReadShipGreetingField(u"AutoTalk"_wref.get());
                    if (Text == u"" || Text == u"No") {
                        cpp_with.AutoTalk = gcNo;
                    } else if (Text == u"Any") {
                        cpp_with.AutoTalk = gcAny;
                    } else {
                        cpp_with.AutoTalk = gcYes;
                    }
                    Text = ReadShipGreetingField(u"FlyType"_wref.get());
                    if (Text == u"Any" || Text == u"") {
                        cpp_with.FlyType = gfAny;
                    } else if (Text == u"ToPlanet") {
                        cpp_with.FlyType = gfToPlanet;
                    } else if (Text == u"ToStar") {
                        cpp_with.FlyType = gfToStar;
                    } else if (Text == u"ToItem") {
                        cpp_with.FlyType = gfToItem;
                    } else if (Text == u"ToShip") {
                        cpp_with.FlyType = gfToShip;
                    } else {
                        GR_Main::RaiseWideMessage(Text);
                    }
                    Text = ReadShipGreetingField(u"ShipType"_wref.get());
                    cpp_with.ShipType = pas::constant_set<TGreetingShipCategories>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Transport", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscTransport);
                        }
                        if (pas::pos("Liner", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscLiner);
                        }
                        if (pas::pos("Diplomat", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscDiplomat);
                        }
                        if (pas::pos("Ranger", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscRanger);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscWarrior);
                        }
                        if (pas::pos("Kling", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscKling);
                        }
                        if (pas::pos("Pirat", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipType, aGalaxyStruct::gscPirateClan);
                        }
                    }
                    Text = ReadShipGreetingField(u"Relations"_wref.get());
                    cpp_with.Relations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipRace"_wref.get());
                    cpp_with.ShipRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadShipGreetingField(u"PlayerRace"_wref.get());
                    cpp_with.PlayerRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadShipGreetingField(u"ShipRaceIsPlayerRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipRaceIsPlayerRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ShipRaceIsPlayerRace = gcNo;
                    } else {
                        cpp_with.ShipRaceIsPlayerRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"PlayerAttackGoodShip"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.PlayerAttackGoodShip = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.PlayerAttackGoodShip = gcNo;
                    } else {
                        cpp_with.PlayerAttackGoodShip = gcAny;
                    }
                    Text = ReadShipGreetingField(u"InFear"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.InFear = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.InFear = gcAny;
                    } else {
                        cpp_with.InFear = gcNo;
                    }
                    Text = ReadShipGreetingField(u"ShipBadFlyToShip"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipBadFlyToShip = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ShipBadFlyToShip = gcAny;
                    } else {
                        cpp_with.ShipBadFlyToShip = gcNo;
                    }
                    Text = ReadShipGreetingField(u"ShipBadType"_wref.get());
                    cpp_with.ShipBadType = pas::constant_set<TGreetingShipCategories>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Transport", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscTransport);
                        }
                        if (pas::pos("Liner", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscLiner);
                        }
                        if (pas::pos("Diplomat", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscDiplomat);
                        }
                        if (pas::pos("Ranger", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscRanger);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscWarrior);
                        }
                        if (pas::pos("Kling", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscKling);
                        }
                        if (pas::pos("PirateClan", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadType, aGalaxyStruct::gscPirateClan);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipBadRace"_wref.get());
                    cpp_with.ShipBadRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadShipGreetingField(u"ShipFlyToPlayer"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipFlyToPlayer = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ShipFlyToPlayer = gcNo;
                    } else {
                        cpp_with.ShipFlyToPlayer = gcAny;
                    }
                    Text = ReadShipGreetingField(u"PlayerFlyToShip"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.PlayerFlyToShip = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.PlayerFlyToShip = gcNo;
                    } else {
                        cpp_with.PlayerFlyToShip = gcAny;
                    }
                    Text = ReadShipGreetingField(u"PlayerIsShipBad"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.PlayerIsShipBad = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.PlayerIsShipBad = gcNo;
                    } else {
                        cpp_with.PlayerIsShipBad = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ShipTurnBeforeEndOrder"_wref.get());
                    cpp_with.ShipTurnBeforeEndOrder = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_3 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_3, text);
                            }()) > 0) {
                                pas::include_at(&cpp_with.ShipTurnBeforeEndOrder, Item);
                            }
                        }
                        if (pas::pos("Far", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipTurnBeforeEndOrder, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerTurnBeforeEndOrder"_wref.get());
                    cpp_with.PlayerTurnBeforeEndOrder = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_4 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_2 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_4, text_2);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PlayerTurnBeforeEndOrder, Item);
                            }
                        }
                        if (pas::pos("Far", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerTurnBeforeEndOrder, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipBadTurnBeforeEndOrder"_wref.get());
                    cpp_with.ShipBadTurnBeforeEndOrder = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_5 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_3 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_5, text_3);
                            }()) > 0) {
                                pas::include_at(&cpp_with.ShipBadTurnBeforeEndOrder, Item);
                            }
                        }
                        if (pas::pos("Far", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipBadTurnBeforeEndOrder, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipStatus"_wref.get());
                    cpp_with.ShipStatus = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, aGalaxyStruct::rcTrader);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, aGalaxyStruct::rcPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, aGalaxyStruct::rcWarrior);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerStatus"_wref.get());
                    cpp_with.PlayerStatus = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcTrader);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcWarrior);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipStrength"_wref.get());
                    cpp_with.ShipStrength = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 1);
                        }
                        // Native uses Pirate here, unlike the other strength/size filters.
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerStrength"_wref.get());
                    cpp_with.PlayerStrength = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStrength, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStrength, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStrength, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStrength, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStrength, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipStructure"_wref.get());
                    cpp_with.ShipStructure = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStructure, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStructure, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStructure, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStructure, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStructure, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerStructure"_wref.get());
                    cpp_with.PlayerStructure = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStructure, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStructure, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStructure, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStructure, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStructure, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipRating"_wref.get());
                    cpp_with.ShipRating = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRating, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRating, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRating, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRating, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRating, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerRating"_wref.get());
                    cpp_with.PlayerRating = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipRank"_wref.get());
                    cpp_with.ShipRank = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Rookie", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 0);
                        }
                        if (pas::pos("Cadet", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 1);
                        }
                        if (pas::pos("Pilot", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 2);
                        }
                        if (pas::pos("Wingman", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 3);
                        }
                        if (pas::pos("Leader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 4);
                        }
                        if (pas::pos("Ace", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 5);
                        }
                        if (pas::pos("Commander", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 6);
                        }
                        if (pas::pos("Admiral", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipRank, 7);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerRank"_wref.get());
                    cpp_with.PlayerRank = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Rookie", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 0);
                        }
                        if (pas::pos("Cadet", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 1);
                        }
                        if (pas::pos("Pilot", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 2);
                        }
                        if (pas::pos("Wingman", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 3);
                        }
                        if (pas::pos("Leader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 4);
                        }
                        if (pas::pos("Ace", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 5);
                        }
                        if (pas::pos("Commander", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 6);
                        }
                        if (pas::pos("Admiral", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 7);
                        }
                    }
                    Text = ReadShipGreetingField(u"RatingShipWithPlayer"_wref.get());
                    cpp_with.RatingShipWithPlayer = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RatingShipWithPlayer, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RatingShipWithPlayer, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RatingShipWithPlayer, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RatingShipWithPlayer, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RatingShipWithPlayer, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"RankShipWithPlayer"_wref.get());
                    cpp_with.RankShipWithPlayer = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayer, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayer, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayer, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayer, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayer, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"StrengthShipWithPlayer"_wref.get());
                    cpp_with.StrengthShipWithPlayer = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.StrengthShipWithPlayer, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.StrengthShipWithPlayer, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.StrengthShipWithPlayer, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.StrengthShipWithPlayer, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.StrengthShipWithPlayer, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"Goods"_wref.get());
                    if (Text == u"") {
                        cpp_with.Goods = aGalaxyStruct::UnspecifiedGoods;
                    } else if (Text == u"Food") {
                        cpp_with.Goods = 0;
                    } else if (Text == u"Medicine") {
                        cpp_with.Goods = 1;
                    } else if (Text == u"Technics") {
                        cpp_with.Goods = 2;
                    } else if (Text == u"Luxury") {
                        cpp_with.Goods = 3;
                    } else if (Text == u"Minerals") {
                        cpp_with.Goods = 4;
                    } else if (Text == u"Alcohol") {
                        cpp_with.Goods = 5;
                    } else if (Text == u"Arms") {
                        cpp_with.Goods = 6;
                    } else if (Text == u"Narcotics") {
                        cpp_with.Goods = 7;
                    } else {
                        cpp_with.Goods = aGalaxyStruct::UnspecifiedGoods;
                    }
                    Text = ReadShipGreetingField(u"ShipGoodsCnt"_wref.get());
                    cpp_with.ShipGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipGoodsCnt, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerGoodsCnt"_wref.get());
                    cpp_with.PlayerGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerGoodsCnt, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipHaveGoods"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipHaveGoods = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ShipHaveGoods = gcNo;
                    } else {
                        cpp_with.ShipHaveGoods = gcAny;
                    }
                    Text = ReadShipGreetingField(u"PlayerHaveGoods"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.PlayerHaveGoods = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.PlayerHaveGoods = gcNo;
                    } else {
                        cpp_with.PlayerHaveGoods = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ShipGoodsTypeCnt"_wref.get());
                    cpp_with.ShipGoodsTypeCnt = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 8; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_6 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_4 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_6, text_4);
                            }()) > 0) {
                                pas::include_at(&cpp_with.ShipGoodsTypeCnt, Item);
                            }
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerGoodsTypeCnt"_wref.get());
                    cpp_with.PlayerGoodsTypeCnt = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 8; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_7 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_5 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_7, text_5);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PlayerGoodsTypeCnt, Item);
                            }
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipMayScanPlayer"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipMayScanPlayer = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ShipMayScanPlayer = gcNo;
                    } else {
                        cpp_with.ShipMayScanPlayer = gcAny;
                    }
                    Text = ReadShipGreetingField(u"RangerInCurStar"_wref.get());
                    cpp_with.RangerInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_8 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_6 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_8, text_6);
                            }()) > 0) {
                                pas::include_at(&cpp_with.RangerInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RangerInCurStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"PirateInCurStar"_wref.get());
                    cpp_with.PirateInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_9 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_7 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_9, text_7);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInCurStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"KlingInCurStar"_wref.get());
                    cpp_with.KlingInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_10 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_8 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_10, text_8);
                            }()) > 0) {
                                pas::include_at(&cpp_with.KlingInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.KlingInCurStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"WarriorInCurStar"_wref.get());
                    cpp_with.WarriorInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_11 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_9 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_11, text_9);
                            }()) > 0) {
                                pas::include_at(&cpp_with.WarriorInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.WarriorInCurStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"TransportInCurStar"_wref.get());
                    cpp_with.TransportInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_12 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_10 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_12, text_10);
                            }()) > 0) {
                                pas::include_at(&cpp_with.TransportInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.TransportInCurStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetRace"_wref.get());
                    if (Text == u"Any") {
                        cpp_with.LastPlanetRace = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
                    } else {
                        cpp_with.LastPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    }
                    Text = ReadShipGreetingField(u"LastPlanetRelations"_wref.get());
                    cpp_with.LastPlanetRelations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetGoodsCnt"_wref.get());
                    cpp_with.LastPlanetGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsCnt, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetGoodsSale"_wref.get());
                    cpp_with.LastPlanetGoodsSale = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsSale, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsSale, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsSale, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsSale, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsSale, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetGoodsBuy"_wref.get());
                    cpp_with.LastPlanetGoodsBuy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsBuy, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsBuy, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsBuy, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsBuy, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGoodsBuy, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetIsHomePlanet"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetIsHomePlanet = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetIsHomePlanet = gcNo;
                    } else {
                        cpp_with.LastPlanetIsHomePlanet = gcAny;
                    }
                    Text = ReadShipGreetingField(u"LastPlanetRaceIsShipRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetRaceIsShipRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetRaceIsShipRace = gcNo;
                    } else {
                        cpp_with.LastPlanetRaceIsShipRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"LastPlanetRaceIsPlayerRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetRaceIsPlayerRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetRaceIsPlayerRace = gcNo;
                    } else {
                        cpp_with.LastPlanetRaceIsPlayerRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"LastPlanetEconomy"_wref.get());
                    cpp_with.LastPlanetEconomy = pas::constant_set<aGalaxyStruct::TPlanetEconomies>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, aGalaxyStruct::peAgricultural);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, aGalaxyStruct::peMixed);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, aGalaxyStruct::peIndustrial);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetGoverment"_wref.get());
                    cpp_with.LastPlanetGovernment = pas::constant_set<aGalaxyStruct::TPlanetGovernments>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, aGalaxyStruct::pgAnarchy);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, aGalaxyStruct::pgDictatorship);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, aGalaxyStruct::pgMonarchy);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, aGalaxyStruct::pgRepublic);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, aGalaxyStruct::pgDemocracy);
                        }
                    }
                    Text = ReadShipGreetingField(u"LastPlanetInCurStar"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetInCurStar = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetInCurStar = gcNo;
                    } else {
                        cpp_with.LastPlanetInCurStar = gcAny;
                    }
                    Text = ReadShipGreetingField(u"LastPlanetDistToShipInTurn"_wref.get());
                    cpp_with.LastPlanetDistToShipInTurn = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 1; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_13 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_11 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_13, text_11);
                            }()) > 0) {
                                pas::include_at(&cpp_with.LastPlanetDistToShipInTurn, Item);
                            }
                        }
                        if (pas::pos("Far", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetDistToShipInTurn, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"RangerInLastPlanetStar"_wref.get());
                    cpp_with.RangerInLastPlanetStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_14 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_12 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_14, text_12);
                            }()) > 0) {
                                pas::include_at(&cpp_with.RangerInLastPlanetStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RangerInLastPlanetStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"PirateInLastPlanetStar"_wref.get());
                    cpp_with.PirateInLastPlanetStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_15 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_13 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_15, text_13);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInLastPlanetStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInLastPlanetStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"KlingInLastPlanetStar"_wref.get());
                    cpp_with.KlingInLastPlanetStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_16 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_14 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_16, text_14);
                            }()) > 0) {
                                pas::include_at(&cpp_with.KlingInLastPlanetStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.KlingInLastPlanetStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"WarriorInLastPlanetStar"_wref.get());
                    cpp_with.WarriorInLastPlanetStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_17 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_15 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_17, text_15);
                            }()) > 0) {
                                pas::include_at(&cpp_with.WarriorInLastPlanetStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.WarriorInLastPlanetStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"TransportInLastPlanetStar"_wref.get());
                    cpp_with.TransportInLastPlanetStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_18 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_16 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_18, text_16);
                            }()) > 0) {
                                pas::include_at(&cpp_with.TransportInLastPlanetStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.TransportInLastPlanetStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetRace"_wref.get());
                    cpp_with.ToPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadShipGreetingField(u"ToPlanetRelations"_wref.get());
                    cpp_with.ToPlanetRelations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetGoodsCnt"_wref.get());
                    cpp_with.ToPlanetGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetGoodsSale"_wref.get());
                    cpp_with.ToPlanetGoodsSale = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetGoodsBuy"_wref.get());
                    cpp_with.ToPlanetGoodsBuy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetIsHomePlanet"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetIsHomePlanet = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetIsHomePlanet = gcNo;
                    } else {
                        cpp_with.ToPlanetIsHomePlanet = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToPlanetRaceIsShipRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsShipRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsShipRace = gcNo;
                    } else {
                        cpp_with.ToPlanetRaceIsShipRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToPlanetRaceIsPlayerRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcNo;
                    } else {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToPlanetEconomy"_wref.get());
                    cpp_with.ToPlanetEconomy = pas::constant_set<aGalaxyStruct::TPlanetEconomies>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peAgricultural);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peMixed);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peIndustrial);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetGoverment"_wref.get());
                    cpp_with.ToPlanetGovernment = pas::constant_set<aGalaxyStruct::TPlanetGovernments>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgAnarchy);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDictatorship);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgMonarchy);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgRepublic);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDemocracy);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToPlanetIsLastPlanet"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetIsLastPlanet = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToPlanetIsLastPlanet = gcAny;
                    } else {
                        cpp_with.ToPlanetIsLastPlanet = gcNo;
                    }
                    Text = ReadShipGreetingField(u"ToPlanetRaceIsLastPlanetRace"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = gcNo;
                    } else {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = gcAny;
                    }
                    Text = ReadShipGreetingField(u"HomePlanetInToStar"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.HomePlanetInToStar = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.HomePlanetInToStar = gcNo;
                    } else {
                        cpp_with.HomePlanetInToStar = gcAny;
                    }
                    Text = ReadShipGreetingField(u"HomePlanetInCurStar"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.HomePlanetInCurStar = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.HomePlanetInCurStar = gcNo;
                    } else {
                        cpp_with.HomePlanetInCurStar = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToStarControlByKling"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByKling = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByKling = gcAny;
                    } else {
                        cpp_with.ToStarControlByKling = gcNo;
                    }
                    Text = ReadShipGreetingField(u"ToStarInBattle"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToStarInBattle = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarInBattle = gcAny;
                    } else {
                        cpp_with.ToStarInBattle = gcNo;
                    }
                    Text = ReadShipGreetingField(u"RangerInToStar"_wref.get());
                    cpp_with.RangerInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_19 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_17 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_19, text_17);
                            }()) > 0) {
                                pas::include_at(&cpp_with.RangerInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RangerInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"PirateInToStar"_wref.get());
                    cpp_with.PirateInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_20 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_18 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_20, text_18);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"KlingInToStar"_wref.get());
                    cpp_with.KlingInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_21 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_19 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_21, text_19);
                            }()) > 0) {
                                pas::include_at(&cpp_with.KlingInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.KlingInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"WarriorInToStar"_wref.get());
                    cpp_with.WarriorInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_22 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_20 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_22, text_20);
                            }()) > 0) {
                                pas::include_at(&cpp_with.WarriorInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.WarriorInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"TransportInToStar"_wref.get());
                    cpp_with.TransportInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_23 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_21 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_23, text_21);
                            }()) > 0) {
                                pas::include_at(&cpp_with.TransportInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.TransportInToStar, 10);
                        }
                    }
                    cpp_with.ItemType = ReadShipGreetingField(u"ItemType"_wref.get());
                    // Native repeats this assignment; preserve both reads.
                    Text = ReadShipGreetingField(u"ToPlanetGoverment"_wref.get());
                    cpp_with.ToPlanetGovernment = pas::constant_set<aGalaxyStruct::TPlanetGovernments>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgAnarchy);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDictatorship);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgMonarchy);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgRepublic);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDemocracy);
                        }
                    }
                    Text = ReadShipGreetingField(u"ShipNeedInItem"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ShipNeedInItem = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ShipNeedInItem = gcNo;
                    } else {
                        cpp_with.ShipNeedInItem = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToShipType"_wref.get());
                    cpp_with.ToShipType = pas::constant_set<TGreetingShipCategories>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Transport", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscTransport);
                        }
                        if (pas::pos("Liner", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscLiner);
                        }
                        if (pas::pos("Diplomat", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscDiplomat);
                        }
                        if (pas::pos("Ranger", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscRanger);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscWarrior);
                        }
                        if (pas::pos("Kling", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipType, aGalaxyStruct::gscKling);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToShipRace"_wref.get());
                    cpp_with.ToShipRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadShipGreetingField(u"ToShipInPlanet"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToShipInPlanet = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToShipInPlanet = gcNo;
                    } else {
                        cpp_with.ToShipInPlanet = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToShipBad"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToShipBad = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToShipBad = gcNo;
                    } else {
                        cpp_with.ToShipBad = gcAny;
                    }
                    Text = ReadShipGreetingField(u"ToShipRelations"_wref.get());
                    cpp_with.ToShipRelations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadShipGreetingField(u"PlayerPirateRank"_wref.get());
                    cpp_with.PlayerPirateRank = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Noobie", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 0);
                        }
                        if (pas::pos("Kid", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 1);
                        }
                        if (pas::pos("Rader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 2);
                        }
                        if (pas::pos("Skipper", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 3);
                        }
                        if (pas::pos("Rough", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 4);
                        }
                        if (pas::pos("Ataman", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 5);
                        }
                        if (pas::pos("Khan", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 6);
                        }
                        if (pas::pos("Baron", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 7);
                        }
                    }
                    Text = ReadShipGreetingField(u"RankShipWithPlayer"_wref.get());
                    cpp_with.RankShipWithPlayerExtra = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayerExtra, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayerExtra, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayerExtra, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayerExtra, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RankShipWithPlayerExtra, 5);
                        }
                    }
                    Text = ReadShipGreetingField(u"Female"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.Female = 0;
                    } else {
                        cpp_with.Female = 1;
                    }
                    Text = ReadShipGreetingField(u"PirateClanInToStar"_wref.get());
                    cpp_with.PirateClanInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_24 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_22 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_24, text_22);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateClanInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateClanInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"ToStarControlByPirates"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByPirates = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByPirates = gcAny;
                    } else {
                        cpp_with.ToStarControlByPirates = gcNo;
                    }
                    Text = ReadShipGreetingField(u"PirateClanInCurStar"_wref.get());
                    cpp_with.PirateClanInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_25 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_23 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_25, text_23);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateClanInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateClanInCurStar, 10);
                        }
                    }
                    // Native repeats this assignment; preserve both reads.
                    Text = ReadShipGreetingField(u"PirateInToStar"_wref.get());
                    cpp_with.PirateInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_26 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_24 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_26, text_24);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInToStar, 10);
                        }
                    }
                    Text = ReadShipGreetingField(u"CoalitionAlreadyDefeated"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.CoalitionAlreadyDefeated = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.CoalitionAlreadyDefeated = gcNo;
                    } else {
                        cpp_with.CoalitionAlreadyDefeated = gcAny;
                    }
                    Text = ReadShipGreetingField(u"DominatorsAlreadyDefeated"_wref.get());
                    if (Text == u"Yes") {
                        cpp_with.DominatorsAlreadyDefeated = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.DominatorsAlreadyDefeated = gcNo;
                    } else {
                        cpp_with.DominatorsAlreadyDefeated = gcAny;
                    }
                }
            }
        }
    }

    void InitializeGovernmentGreetingDefinitions() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        std::int32_t EntryIndex{};
        std::int32_t Item{};
        pas::WideString Text{};
        auto ReadGovernmentGreetingField = [&](pas::WideString FieldName) -> pas::WideString {
            if (Block->CountParams(FieldName) > 0) {
                return Block->GetParam(pas::view(FieldName));
            }
            return pas::WideString();
        };
        GovernmentGreetingCount = 0;
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"GovGreetings.CountGovGreetings"_wref.get())));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"GovGreetings"sv);
                return block->CountBlocks(intToStr);
            }()) > 0) {
                ++GovernmentGreetingCount;
            }
        }
        GovernmentGreetingDefinitions.set_length(GovernmentGreetingCount);
        GovernmentGreetingCount = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block_2 = GR_Main::LanguageDataConfig->GetBlock(u"GovGreetings"sv);
                return block_2->CountBlocks(intToStr_2);
            }()) != 0) {
                ++GovernmentGreetingCount;
                EntryIndex = GovernmentGreetingCount - 1;
                Block = GR_Main::LanguageDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"GovGreetings.", SysUtils::IntToStr(Index)})));
                {
                    TGovGreetingsInfo& cpp_with = GovernmentGreetingDefinitions[EntryIndex];
                    cpp_with.Name = pas::wide_int_to_str(Index);
                    Text = ReadGovernmentGreetingField(u"Priority"_w);
                    if (Text == u"") {
                        cpp_with.Priority = 10;
                    } else {
                        cpp_with.Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
                    }
                    Text = ReadGovernmentGreetingField(u"PlayerRace"_w);
                    cpp_with.PlayerRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadGovernmentGreetingField(u"PlayerStatus"_w);
                    cpp_with.PlayerStatus = pas::constant_set<aGalaxyStruct::TRangerCareerSet>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcTrader);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcPirate);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, aGalaxyStruct::rcWarrior);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"PlayerRating"_w);
                    cpp_with.PlayerRating = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRating, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"PlayerRank"_w);
                    cpp_with.PlayerRank = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Rookie", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 0);
                        }
                        if (pas::pos("Cadet", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 1);
                        }
                        if (pas::pos("Pilot", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 2);
                        }
                        if (pas::pos("Wingman", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 3);
                        }
                        if (pas::pos("Leader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 4);
                        }
                        if (pas::pos("Ace", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 5);
                        }
                        if (pas::pos("Commander", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 6);
                        }
                        if (pas::pos("Admiral", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerRank, 7);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"Goods"_w);
                    if (Text == u"") {
                        cpp_with.Goods = aGalaxyStruct::UnspecifiedGoods;
                    } else if (Text == u"Food") {
                        cpp_with.Goods = 0;
                    } else if (Text == u"Medicine") {
                        cpp_with.Goods = 1;
                    } else if (Text == u"Technics") {
                        cpp_with.Goods = 2;
                    } else if (Text == u"Luxury") {
                        cpp_with.Goods = 3;
                    } else if (Text == u"Minerals") {
                        cpp_with.Goods = 4;
                    } else if (Text == u"Alcohol") {
                        cpp_with.Goods = 5;
                    } else if (Text == u"Arms") {
                        cpp_with.Goods = 6;
                    } else if (Text == u"Narcotics") {
                        cpp_with.Goods = 7;
                    } else {
                        cpp_with.Goods = aGalaxyStruct::UnspecifiedGoods;
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetRace"_w);
                    cpp_with.CurPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = ReadGovernmentGreetingField(u"CurPlanetRaceIsPlayerRace"_w);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetRaceIsPlayerRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetRaceIsPlayerRace = gcNo;
                    } else {
                        cpp_with.CurPlanetRaceIsPlayerRace = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetRelations"_w);
                    cpp_with.CurPlanetRelations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetGoodsPermit"_w);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetGoodsPermit = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetGoodsPermit = gcNo;
                    } else {
                        cpp_with.CurPlanetGoodsPermit = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetGoodsCnt"_w);
                    cpp_with.CurPlanetGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsCnt, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetGoodsSale"_w);
                    cpp_with.CurPlanetGoodsSale = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsSale, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsSale, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsSale, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsSale, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsSale, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetGoodsBuy"_w);
                    cpp_with.CurPlanetGoodsBuy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsBuy, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsBuy, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsBuy, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsBuy, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGoodsBuy, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetEconomy"_w);
                    cpp_with.CurPlanetEconomy = pas::constant_set<aGalaxyStruct::TPlanetEconomies>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, aGalaxyStruct::peAgricultural);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, aGalaxyStruct::peMixed);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, aGalaxyStruct::peIndustrial);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetGoverment"_w);
                    cpp_with.CurPlanetGovernment = pas::constant_set<aGalaxyStruct::TPlanetGovernments>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, aGalaxyStruct::pgAnarchy);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, aGalaxyStruct::pgDictatorship);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, aGalaxyStruct::pgMonarchy);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, aGalaxyStruct::pgRepublic);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, aGalaxyStruct::pgDemocracy);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"RangerInCurStar"_w);
                    cpp_with.RangerInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_3 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_3, text);
                            }()) > 0) {
                                pas::include_at(&cpp_with.RangerInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RangerInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"PirateInCurStar"_w);
                    cpp_with.PirateInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_4 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_2 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_4, text_2);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"KlingInCurStar"_w);
                    cpp_with.KlingInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_5 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_3 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_5, text_3);
                            }()) > 0) {
                                pas::include_at(&cpp_with.KlingInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.KlingInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"WarriorInCurStar"_w);
                    cpp_with.WarriorInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_6 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_4 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_6, text_4);
                            }()) > 0) {
                                pas::include_at(&cpp_with.WarriorInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.WarriorInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"TransportInCurStar"_w);
                    cpp_with.TransportInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_7 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_5 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_7, text_5);
                            }()) > 0) {
                                pas::include_at(&cpp_with.TransportInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.TransportInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"CurStarInBattle"_w);
                    if (Text == u"Yes") {
                        cpp_with.CurStarInBattle = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.CurStarInBattle = gcAny;
                    } else {
                        cpp_with.CurStarInBattle = gcNo;
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetRace"_w);
                    if (Text == u"Any") {
                        cpp_with.ToPlanetRace = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal}});
                    } else {
                        cpp_with.ToPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetRaceIsPlayerRace"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcNo;
                    } else {
                        cpp_with.ToPlanetRaceIsPlayerRace = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetRaceIsCurPlanetRace"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = gcNo;
                    } else {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetRelations"_w);
                    cpp_with.ToPlanetRelations = pas::constant_set<aGalaxyStruct::TRelationLevels>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlHostile);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlBad);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlNormal);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlGood);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, aGalaxyStruct::rlExcellent);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetGoodsPermit"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetGoodsPermit = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetGoodsPermit = gcNo;
                    } else {
                        cpp_with.ToPlanetGoodsPermit = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetGoodsCnt"_w);
                    cpp_with.ToPlanetGoodsCnt = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Zero", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 0);
                        }
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsCnt, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetGoodsSale"_w);
                    cpp_with.ToPlanetGoodsSale = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsSale, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetGoodsBuy"_w);
                    cpp_with.ToPlanetGoodsBuy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 1);
                        }
                        if (pas::pos("Small", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 2);
                        }
                        if (pas::pos("Average", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 3);
                        }
                        if (pas::pos("Big", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 4);
                        }
                        if (pas::pos("Huge", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGoodsBuy, 5);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetEconomy"_w);
                    cpp_with.ToPlanetEconomy = pas::constant_set<aGalaxyStruct::TPlanetEconomies>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peAgricultural);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peMixed);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, aGalaxyStruct::peIndustrial);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetGoverment"_w);
                    cpp_with.ToPlanetGovernment = pas::constant_set<aGalaxyStruct::TPlanetGovernments>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgAnarchy);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDictatorship);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgMonarchy);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgRepublic);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, aGalaxyStruct::pgDemocracy);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToPlanetInCurStar"_w);
                    if (Text == u"Any") {
                        cpp_with.ToPlanetInCurStar = gcAny;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetInCurStar = gcNo;
                    } else {
                        cpp_with.ToPlanetInCurStar = gcYes;
                    }
                    Text = ReadGovernmentGreetingField(u"RangerInToStar"_w);
                    cpp_with.RangerInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_8 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_6 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_8, text_6);
                            }()) > 0) {
                                pas::include_at(&cpp_with.RangerInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.RangerInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"PirateInToStar"_w);
                    cpp_with.PirateInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_9 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_7 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_9, text_7);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"KlingInToStar"_w);
                    cpp_with.KlingInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_10 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_8 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_10, text_8);
                            }()) > 0) {
                                pas::include_at(&cpp_with.KlingInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.KlingInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"WarriorInToStar"_w);
                    cpp_with.WarriorInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_11 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_9 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_11, text_9);
                            }()) > 0) {
                                pas::include_at(&cpp_with.WarriorInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.WarriorInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"TransportInToStar"_w);
                    cpp_with.TransportInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_12 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_10 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_12, text_10);
                            }()) > 0) {
                                pas::include_at(&cpp_with.TransportInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.TransportInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToStarControlByKling"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByKling = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByKling = gcAny;
                    } else {
                        cpp_with.ToStarControlByKling = gcNo;
                    }
                    Text = ReadGovernmentGreetingField(u"ToStarInBattle"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToStarInBattle = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarInBattle = gcAny;
                    } else {
                        cpp_with.ToStarInBattle = gcNo;
                    }
                    Text = ReadGovernmentGreetingField(u"CurPlanetPirateClan"_w);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetPirateClan = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetPirateClan = gcNo;
                    } else {
                        cpp_with.CurPlanetPirateClan = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"CurStarInBattlePirates"_w);
                    if (Text == u"Yes") {
                        cpp_with.CurStarInBattlePirates = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.CurStarInBattlePirates = gcAny;
                    } else {
                        cpp_with.CurStarInBattlePirates = gcNo;
                    }
                    Text = ReadGovernmentGreetingField(u"PirateClanInCurStar"_w);
                    cpp_with.PirateClanInCurStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_13 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_11 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_13, text_11);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateClanInCurStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateClanInCurStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"PirateClanInToStar"_w);
                    cpp_with.PirateClanInToStar = pas::constant_set<aGalaxyStruct::TGreetingCountMask>({});
                    if (Text != u"" && Text != u"Any") {
                        for (Item = 0; Item <= 9; ++Item) {
                            if (([&] {
                                const pas::AnsiString& intToStr_14 = SysUtils::IntToStr(Item);
                                const pas::AnsiString& text_12 = static_cast<pas::AnsiString>(Text);
                                return pas::pos(intToStr_14, text_12);
                            }()) > 0) {
                                pas::include_at(&cpp_with.PirateClanInToStar, Item);
                            }
                        }
                        if (pas::pos("Many", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PirateClanInToStar, 10);
                        }
                    }
                    Text = ReadGovernmentGreetingField(u"ToStarControlByPirates"_w);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByPirates = gcYes;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByPirates = gcAny;
                    } else {
                        cpp_with.ToStarControlByPirates = gcNo;
                    }
                    Text = ReadGovernmentGreetingField(u"CoalitionAlreadyDefeated"_w);
                    if (Text == u"Yes") {
                        cpp_with.CoalitionAlreadyDefeated = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.CoalitionAlreadyDefeated = gcNo;
                    } else {
                        cpp_with.CoalitionAlreadyDefeated = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"DominatorsAlreadyDefeated"_w);
                    if (Text == u"Yes") {
                        cpp_with.DominatorsAlreadyDefeated = gcYes;
                    } else if (Text == u"No") {
                        cpp_with.DominatorsAlreadyDefeated = gcNo;
                    } else {
                        cpp_with.DominatorsAlreadyDefeated = gcAny;
                    }
                    Text = ReadGovernmentGreetingField(u"PlayerPirateRank"_w);
                    cpp_with.PlayerPirateRank = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Noobie", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 0);
                        }
                        if (pas::pos("Kid", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 1);
                        }
                        if (pas::pos("Rader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 2);
                        }
                        if (pas::pos("Skipper", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 3);
                        }
                        if (pas::pos("Rough", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 4);
                        }
                        if (pas::pos("Ataman", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 5);
                        }
                        if (pas::pos("Khan", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 6);
                        }
                        if (pas::pos("Baron", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerPirateRank, 7);
                        }
                    }
                }
            }
        }
    }

    void InitializePlanetAdvertDefinitions() {
        EC_BlockPar::TBlockParEC* GroupBlock{};
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t GroupIndex{};
        std::int32_t BlockIndex{};
        std::int32_t AdvertIndex{};
        std::int32_t FoundIndex{};
        std::int32_t Count{};
        pas::WideString Text{};
        pas::WideString Name{};
        EC_BlockPar::TBlockParEC* Root = GR_Main::MainDataConfig->GetBlockByPath(u"Data\\PlanetAdvt"_wref.get());
        PlanetAdvertDefinitions.set_length(Root->GetBlockCount());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PlanetAdvertDefinitions.length() - 1); cpp_range.next(GroupIndex); ) {
            GroupBlock = Root->GetBlockByIndex(GroupIndex);
            {
                WindowsSdk::TPoint cpp_value = GI_Main::GetPointGI(pas::view(GroupBlock->GetParamByPathOrMarker(u"Info.Pos"_wref.get())));
                auto cpp_target = &PlanetAdvertDefinitions[GroupIndex].Position;
                pas::store_unaligned<WindowsSdk::TPoint>(cpp_target, cpp_value);
            }
            if (GroupBlock->GetBlock(u"Info"sv)->CountParams(u"Image1"_wref.get()) > 0) {
                PlanetAdvertDefinitions[GroupIndex].Image1 = GroupBlock->GetParamByPathOrMarker(u"Info.Image1"_wref.get());
            }
            if (GroupBlock->GetBlock(u"Info"sv)->CountParams(u"Image2"_wref.get()) > 0) {
                PlanetAdvertDefinitions[GroupIndex].Image2 = GroupBlock->GetParamByPathOrMarker(u"Info.Image2"_wref.get());
            }
            Count = GroupBlock->GetBlockCount();
            PlanetAdvertDefinitions[GroupIndex].Adverts.set_length(Count - 2);
            AdvertIndex = 0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(BlockIndex); ) {
                Text = GroupBlock->GetBlockNameByIndex(BlockIndex);
                if (Text != u"List" && Text != u"Info") {
                    Block = GroupBlock->GetBlockByIndex(BlockIndex);
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Name = Text;
                    if (Block->CountParams(u"Image1"_wref.get()) > 0) {
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Image1 = Block->GetParam(u"Image1"sv);
                    }
                    if (Block->CountParams(u"Image2"_wref.get()) > 0) {
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Image2 = Block->GetParam(u"Image2"sv);
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = 0;
                    if (Block->CountParams(u"War"_wref.get()) > 0) {
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = EC_Str::ExtractSignedDigitsToIntW(pas::view(Block->GetParam(u"War"sv)));
                        if (PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War < -1) {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = -1;
                        } else if (PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War > 1) {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = 1;
                        }
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = aGalaxyStruct::UnspecifiedGoods;
                    if (Block->CountParams(u"Goods"_wref.get()) > 0) {
                        Text = Block->GetParam(u"Goods"sv);
                        if (Text == u"Food") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 0;
                        } else if (Text == u"Medicine") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 1;
                        } else if (Text == u"Technics") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 2;
                        } else if (Text == u"Luxury") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 3;
                        } else if (Text == u"Minerals") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 4;
                        } else if (Text == u"Alcohol") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 5;
                        } else if (Text == u"Arms") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 6;
                        } else if (Text == u"Narcotics") {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 7;
                        } else {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = aGalaxyStruct::UnspecifiedGoods;
                        }
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
                    if (Block->CountParams(u"Owner"_wref.get()) > 0) {
                        Text = Block->GetParam(u"Owner"sv);
                        if (pas::pos("Maloc", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, aGalaxyStruct::oiMaloc);
                        }
                        if (pas::pos("Peleng", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, aGalaxyStruct::oiPeleng);
                        }
                        if (pas::pos("People", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, aGalaxyStruct::oiHuman);
                        }
                        if (pas::pos("Fei", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, aGalaxyStruct::oiFeyan);
                        }
                        if (pas::pos("Gaal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, aGalaxyStruct::oiGaal);
                        }
                    }
                    ++AdvertIndex;
                    if (AdvertIndex >= Count - 1) {
                        break;
                    }
                }
            }
            Block = GroupBlock->GetBlock(u"List"sv);
            PlanetAdvertDefinitions[GroupIndex].Lists.set_length(Block->GetParamCount());
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, PlanetAdvertDefinitions[GroupIndex].Lists.length() - 1); cpp_range_3.next(BlockIndex); ) {
                PlanetAdvertDefinitions[GroupIndex].Lists[BlockIndex].Weight = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(BlockIndex)));
                Text = Block->GetParamValue(BlockIndex);
                Count = EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv);
                PlanetAdvertDefinitions[GroupIndex].Lists[BlockIndex].Indices.set_length(Count);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(AdvertIndex); ) {
                    Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(pas::view(Text), AdvertIndex, u","sv));
                    FoundIndex = 0;
                    // Native stops before comparing the last entry, and retains it as fallback.
                    while (FoundIndex < PlanetAdvertDefinitions[GroupIndex].Adverts.length() - 1) {
                        if (PlanetAdvertDefinitions[GroupIndex].Adverts[FoundIndex].Name == Name) {
                            break;
                        }
                        ++FoundIndex;
                    }
                    if (FoundIndex > PlanetAdvertDefinitions[GroupIndex].Adverts.length() - 1) {
                        GR_Main::RaiseWideMessage(pas::concat_wide({u"PlanetAdvtInit. Not found: ", Name}));
                    }
                    PlanetAdvertDefinitions[GroupIndex].Lists[BlockIndex].Indices[AdvertIndex] = FoundIndex;
                }
            }
        }
    }

    void InitializeGlobalUiRuntime() {
        aGalaxyStruct::TOwnerId Race{};
        std::int32_t Index{};
        std::int32_t TemplateIndex{};
        std::int32_t Count{};
        TSputnikTempl* SatelliteTemplate{};
        EC_BlockPar::TBlockParEC* Section{};
        aGalaxyStruct::TKlingType KlingKind{};
        aGalaxyStruct::THullType Kind{};
        TScriptTemplUnit* ScriptTemplate{};
        pas::WideString Text{};
        pas::WideString WarningText{};
        EC_BlockPar::TBlockParEC* ShipBlock{};
        EC_Expression::TVarEC* Variable{};
        EC_Expression::TVarEC* Other{};
        CacheLoader = pas::construct_call<fLoad::TCacheLoader>(EC_Thread::TThreadEC_Create);
        aSaveLoad::InitializeSaveWriter();
        GlobalsV::RangerFontName = u"Font.2Ranger"_w;
        GlobalsV::MiniFontName = u"Font.2Mini"_w;
        GlobalsV::SmallFontName = u"Font.2Small"_w;
        GlobalsV::SmallBoldFontName = u"Font.2SmallBold"_w;
        GlobalsV::NormalFontName = u"Font.2Normal"_w;
        GlobalsV::NormalBoldFontName = u"Font.2NormalBold"_w;
        GlobalsV::BigFontName = u"Font.2Big"_w;
        GlobalsV::HugeFontName = u"Font.2Huge"_w;
        GlobalsV::IntroFontName = u"Font.2Intro"_w;
        GlobalsV::AuthorsFontName = u"Font.2Authors"_w;
        GlobalsV::SmoothSmallFontName = u"Font.Verdana8"_w;
        GlobalsV::SmoothSmallBoldFontName = u"Font.Verdana8bold"_w;
        GlobalsV::SmoothNormalFontName = u"Font.Verdana9"_w;
        GlobalsV::SmoothNormalBoldFontName = u"Font.Verdana9bold"_w;
        GlobalsV::SmoothBigFontName = u"Font.Verdana11"_w;
        GlobalsV::SmoothHugeFontName = u"Font.Verdana12"_w;
        GlobalsV::SmoothIntroFontName = u"Font.Verdana13"_w;
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ChangeAutoPilot"_wref.get()) > 0) {
            GlobalsV::ChangeAutoPilot = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ChangeAutoPilot"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParams(u"AltResolutionSwitch"_wref.get()) > 0) {
            GR_Main::AltResolutionSwitch = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AltResolutionSwitch"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DisableAutoPilot"_wref.get()) > 0) {
            GlobalsV::DisableAutoPilot = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DisableAutoPilot"_wref.get())));
        }
        GlobalsV::AwardDialogsEnabled = true;
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PQuestStyle"_wref.get()) > 0) {
            GlobalsV::QuestStyleIndex = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PQuestStyle"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PQuestAnim"_wref.get()) > 0) {
            GlobalsV::QuestPageAnimationEnabled = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PQuestAnim"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DefaultOrder"_wref.get()) > 0) {
            GlobalsV::DefaultOrder = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DefaultOrder"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RightClickOnShip"_wref.get()) > 0) {
            GlobalsV::RightClickOnShip = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RightClickOnShip"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DoNotChangeMusicInBattle"_wref.get()) > 0) {
            GlobalsV::DoNotChangeMusicInBattle = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DoNotChangeMusicInBattle"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ViewFollowShip"_wref.get()) > 0) {
            GlobalsV::ViewFollowShip = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ViewFollowShip"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ViewPathLength"_wref.get()) > 0) {
            GlobalsV::ViewPathLength = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ViewPathLength"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"TurnSaveStep"_wref.get()) > 0) {
            GlobalsV::TurnSaveStep = std::min<std::int32_t>(aGalaxyStruct::TurnsPerYear, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"TurnSaveStep"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"QuickSaveExtraSlots"_wref.get()) > 0) {
            GlobalsV::QuickSaveExtraSlots = std::min<std::int32_t>(9, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"QuickSaveExtraSlots"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"MaxPlayerNews"_wref.get()) > 0) {
            GlobalsV::MaxPlayerNews = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"MaxPlayerNews"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ForsageDeactivatePercent"_wref.get()) > 0) {
            GlobalsV::AfterburnerStopCondition = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ForsageDeactivatePercent"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"MaxSearchResult"_wref.get()) > 0) {
            GlobalsV::MaxSearchResult = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"MaxSearchResult"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ClickAutoCloseForm"_wref.get()) > 0) {
            GlobalsV::ClickAutoCloseForm = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ClickAutoCloseForm"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ActionDoubleClick"_wref.get()) > 0) {
            GlobalsV::ActionDoubleClick = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ActionDoubleClick"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipGiper"_wref.get()) > 0) {
            GlobalsV::SkipGiper = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipGiper"_wref.get())));
        }
        Text = u"e"_w;
        Text = pas::concat_wide({Text, u"s"});
        Text = pas::concat_wide({Text, u"t"});
        if (GR_Main::UserSettingsConfig->CountParamsByPath(Text) > 0) {
            GlobalsV::EstOptionEnabled = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(Text)));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SendRecordOff"_wref.get()) > 0) {
            GlobalsV::SendRecordOff = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SendRecordOff"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Wind"_wref.get()) > 0) {
            GlobalsV::Wind = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Wind"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Skip1C"_wref.get()) > 0) {
            Skip1C = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Skip1C"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipVideo"_wref.get()) > 0) {
            SkipVideo = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipVideo"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipIntro"_wref.get()) > 0) {
            SkipIntro = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipIntro"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ShipTail"_wref.get()) > 0) {
            GlobalsV::ShipTail = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ShipTail"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimCaptain"_wref.get()) > 0) {
            GlobalsV::AnimCaptain = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimCaptain"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimItem"_wref.get()) > 0) {
            GlobalsV::AnimItem = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimItem"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BGImage"_wref.get()) > 0) {
            GlobalsV::BGImage = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BGImage"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Comet"_wref.get()) > 0) {
            GlobalsV::Comet = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Comet"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimShipFull"_wref.get()) > 0) {
            GlobalsV::AnimShipFull = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimShipFull"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimCity"_wref.get()) > 0) {
            GlobalsV::AnimCity = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimCity"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimGov"_wref.get()) > 0) {
            GlobalsV::AnimGov = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimGov"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimMenuShip"_wref.get()) > 0) {
            GlobalsV::AnimMenuShip = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimMenuShip"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimStar"_wref.get()) > 0) {
            GlobalsV::AnimStar = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimStar"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimHangar"_wref.get()) > 0) {
            GlobalsV::AnimHangar = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimHangar"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"CircleAction"_wref.get()) > 0) {
            GlobalsV::CircleAction = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"CircleAction"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"StaticBackground"_wref.get()) > 0) {
            GlobalsV::StaticBackground = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"StaticBackground"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScrollTime"_wref.get()) > 0) {
            GlobalsV::ScrollTime = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScrollTime"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScrollStep"_wref.get()) > 0) {
            GlobalsV::ScrollStep = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScrollStep"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScrollSense"_wref.get()) > 0) {
            GlobalsV::ScrollSense = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScrollSense"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FilmSpeed"_wref.get()) > 0) {
            GlobalsV::FilmSpeed = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FilmSpeed"_wref.get())));
        }
        GR_Main::ScrollInteriorRect = ClassesImports::Rect(GlobalsV::ScrollSense, GlobalsV::ScrollSense, GR_Main::GameScreenWidth - GlobalsV::ScrollSense, GR_Main::GameScreenHeight - GlobalsV::ScrollSense);
        if (!GR_Main::IsInstallFeatureEnabled(u"AnimGov"_wref.get())) {
            GlobalsV::AnimGov = 0;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"AnimCaptain"_wref.get())) {
            GlobalsV::AnimCaptain = false;
        }
        if (!GR_Main::IsInstallFeatureEnabled(u"Video"_wref.get())) {
            SkipVideo = true;
            SkipIntro = true;
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BGOCount"_wref.get()) > 0) {
            GlobalsV::BGOCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BGOCount"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BGOTime"_wref.get()) > 0) {
            GlobalsV::BGOTime = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BGOTime"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"MaxFilmStepSkip"_wref.get()) > 0) {
            GlobalsV::MaxFilmStepSkip = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"MaxFilmStepSkip"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"CountFilmSave"_wref.get()) > 0) {
            GlobalsV::FilmHistoryLimit = std::max<std::int32_t>(1, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"CountFilmSave"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SputnikShow"_wref.get()) > 0) {
            GlobalsV::SputnikShow = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SputnikShow"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SpaceImage"_wref.get()) > 0) {
            GlobalsV::SpaceImage = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SpaceImage"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ShowFPS"_wref.get()) > 0) {
            GR_Main::ShowFrameRate = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ShowFPS"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontGalaxy"_wref.get()) > 0) {
            GlobalsV::GalaxyMapFontChoice = static_cast<GlobalsV::TGalaxyMapFontChoice>(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontGalaxy"_wref.get()))));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontDialog"_wref.get()) > 0) {
            GlobalsV::FontDialog = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontDialog"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontQuest"_wref.get()) > 0) {
            GlobalsV::FontQuest = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontQuest"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontSmooth"_wref.get()) > 0) {
            GlobalsV::FontSmoothingEnabled = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontSmooth"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScreenShotType"_wref.get()) > 0) {
            GlobalsV::ScreenshotFormat = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScreenShotType"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScreenShotQuality"_wref.get()) > 0) {
            GlobalsV::ScreenshotJpegQuality = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScreenShotQuality"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DynamicTipsPos"_wref.get()) > 0) {
            GlobalsV::DynamicTipsPos = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DynamicTipsPos"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundShade"_wref.get()) > 0) {
            GlobalsV::BackgroundShade = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundShade"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundBlur"_wref.get()) > 0) {
            GlobalsV::BackgroundBlur = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundBlur"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundGrayscale"_wref.get()) > 0) {
            GlobalsV::BackgroundGrayscale = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundGrayscale"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PlanetClouds"_wref.get()) > 0) {
            GlobalsV::PlanetClouds = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PlanetClouds"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PlanetAtm"_wref.get()) > 0) {
            GlobalsV::PlanetAtm = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PlanetAtm"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimChangeForm"_wref.get()) > 0) {
            GlobalsV::AnimChangeForm = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimChangeForm"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimMainFon"_wref.get()) > 0) {
            GlobalsV::AnimMainFon = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimMainFon"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BeginCalcNextTurn"_wref.get()) > 0) {
            GlobalsV::BeginCalcNextTurn = pas::real_divide(EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BeginCalcNextTurn"_wref.get()))), 1.0E+2L);
        }
        if (GlobalsV::BeginCalcNextTurn < 0.0L) {
            GlobalsV::BeginCalcNextTurn = 0.0f;
        } else if (GlobalsV::BeginCalcNextTurn > 1.0L) {
            GlobalsV::BeginCalcNextTurn = 1.0f;
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"HalfGovAnim"_wref.get()) > 0) {
            GlobalsV::HalfGovAnim = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"HalfGovAnim"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"UseTablesForGov"_wref.get()) > 0) {
            GlobalsV::UseTablesForGov = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"UseTablesForGov"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotShowStencilShadows"_wref.get()) > 0) {
            Robot::RobotSettings.ShowStencilShadows = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotShowStencilShadows"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotShowProjShadows"_wref.get()) > 0) {
            Robot::RobotSettings.ShowProjShadows = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotShowProjShadows"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSelectEx"_wref.get()) > 0) {
            Robot::RobotSettings.SelectEx = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSelectEx"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotLandTexturesGloss"_wref.get()) > 0) {
            Robot::RobotSettings.LandTexturesGloss = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotLandTexturesGloss"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotObjTexturesGloss"_wref.get()) > 0) {
            Robot::RobotSettings.ObjTexturesGloss = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotObjTexturesGloss"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSoftwareCursor"_wref.get()) > 0) {
            Robot::RobotSettings.SoftwareCursor = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSoftwareCursor"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSky"_wref.get()) > 0) {
            Robot::RobotSettings.Sky = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSky"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotRobotShadow"_wref.get()) > 0) {
            Robot::RobotSettings.RobotShadow = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotRobotShadow"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSound"_wref.get()) > 0) {
            Robot::RobotSound = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSound"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotMusic"_wref.get()) > 0) {
            Robot::RobotMusic = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotMusic"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotVSync"_wref.get()) > 0) {
            Robot::RobotVSync = GI_Main::ParseEnabledNameGI(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotVSync"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotFSAASamples"_wref.get()) > 0) {
            Robot::RobotFSAASamples = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotFSAASamples"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotAnisotropy"_wref.get()) > 0) {
            Robot::RobotAnisotropy = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotAnisotropy"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotMaxDistance"_wref.get()) > 0) {
            Robot::RobotMaxDistance = EC_Str::ExtractDigitsToIntW(pas::view(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotMaxDistance"_wref.get())));
        }
        if (ReloadScriptTemplates) {
            Section = GR_Main::GameDataConfig->GetBlockByPath(u"Script"_wref.get());
            Count = Section->GetParamCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                if (Globals::FindScriptTemplateIndex(pas::view(Section->GetParamName(Index))) >= 0) {
                    pas::raise(pas::make_exception<pas::Exception>("Script name not unique"_a));
                }
                ScriptTemplate = pas::construct_call<TScriptTemplUnit>(TScriptTemplUnit_Create);
                ScriptTemplate->Name = Section->GetParamName(Index);
                Text = Section->GetParamValue(Index);
                ScriptTemplate->ClassId = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                ScriptTemplate->FileName = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv);
                pas::list_add(ScriptTemplates, reinterpret_cast<void*>(ScriptTemplate));
                aScript::CompileScriptTemplateCondition(pas::list_count(ScriptTemplates) - 1);
            }
            for (auto cpp_range_2 = pas::for_downto<std::int32_t>(GlobalScriptVariables->Count - 2, 0); cpp_range_2.next(Index); ) {
                Variable = EC_Expression::TVarArrayEC_GetItemByNameOrder(GlobalScriptVariables, Index);
                Other = EC_Expression::TVarArrayEC_GetItemByNameOrder(GlobalScriptVariables, Index + 1);
                if (Variable->Name == Other->Name) {
                    if (EC_Expression::TVarEC_RealVType(Variable) != EC_Expression::TVarEC_RealVType(Other)) {
                        WarningText = pas::concat_wide({u"Warning! Mismatching global variables with same name <", Variable->Name, u"> found! Types are ", ScriptVariableTypeNames[EC_Expression::TVarEC_RealVType(Variable)], u" and ", ScriptVariableTypeNames[EC_Expression::TVarEC_RealVType(Other)]});
                        if (EC_Expression::TVarEC_RealVType(Variable) == EC_Expression::vkEmpty) {
                            WarningText = pas::concat_wide({WarningText, u", ", ScriptVariableTypeNames[EC_Expression::TVarEC_RealVType(Variable)], u" will be discarded"});
                            GlobalScriptVariables->Remove(Variable);
                        } else {
                            WarningText = pas::concat_wide({WarningText, u", ", ScriptVariableTypeNames[EC_Expression::TVarEC_RealVType(Other)], u" will be discarded"});
                            GlobalScriptVariables->Remove(Other);
                        }
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(WarningText));
                    } else if (EC_Expression::TVarEC_EqualsValue(Variable, Other) || static_cast<std::uint8_t>(pas::is_one_of<EC_Expression::vkInt, EC_Expression::vkDword, EC_Expression::vkFloat, EC_Expression::vkString>(EC_Expression::TVarEC_RealVType(Variable)) ^ 1)) {
                        GlobalScriptVariables->Remove(Other);
                    } else {
                        WarningText = pas::concat_wide({u"Warning! Mismatching global variables with same name <", Variable->Name, u"> found! Initial values are ", Variable->GetString(), u" and ", Other->GetString(), u". Value ", Variable->GetString(), u" will be used"});
                        GlobalScriptVariables->Remove(Other);
                    }
                }
            }
        }
        if (ReloadScriptTemplates) {
            for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_3.next(Race); ) {
                ShipBlock = ([&] {
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Ship"_wref.get());
                    const pas::WideString& internalName = aConst::OwnerInfo[Race].InternalName;
                    return blockByPath->FindBlock(pas::view(internalName));
                }());
                for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::THullType>(aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat); cpp_range_4.next(Kind); ) {
                    RaceShipTemplates[Race][Kind] = nullptr;
                }
                PirateClanShipTemplates[Race] = nullptr;
                if (ShipBlock != nullptr) {
                    if (ShipBlock->CountBlocks(u"Ranger"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Ranger"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htRanger]);
                        SE_Space::RetainSpaceObject(cpp_arg, createSpaceObjectByName);
                    }
                    if (ShipBlock->CountBlocks(u"Warrior"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Warrior"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_2 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htWarrior]);
                        SE_Space::RetainSpaceObject(cpp_arg_2, createSpaceObjectByName_2);
                    }
                    if (ShipBlock->CountBlocks(u"Pirate"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_3 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Pirate"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_3 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htPirate]);
                        SE_Space::RetainSpaceObject(cpp_arg_3, createSpaceObjectByName_3);
                    }
                    if (ShipBlock->CountBlocks(u"Transport"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_4 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Transport"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_4 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htTransport]);
                        SE_Space::RetainSpaceObject(cpp_arg_4, createSpaceObjectByName_4);
                    }
                    if (ShipBlock->CountBlocks(u"Liner"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_5 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Liner"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_5 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htLiner]);
                        SE_Space::RetainSpaceObject(cpp_arg_5, createSpaceObjectByName_5);
                    }
                    if (ShipBlock->CountBlocks(u"Diplomat"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_6 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Diplomat"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_6 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][aGalaxyStruct::htDiplomat]);
                        SE_Space::RetainSpaceObject(cpp_arg_6, createSpaceObjectByName_6);
                    }
                    if (ShipBlock->CountBlocks(u"PirateClan"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_7 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".PirateClan"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_7 = pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]);
                        SE_Space::RetainSpaceObject(cpp_arg_7, createSpaceObjectByName_7);
                    }
                }
            }
            Index = 1;
            for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_5.next(KlingKind); ) {
                if (KlingKind != aGalaxyStruct::ktBoss) {
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_8 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, static_cast<pas::WideString>(pas::concat_ansi({"Ship.Blazer.B", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_8 = pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[KlingKind]);
                        SE_Space::RetainSpaceObject(cpp_arg_8, createSpaceObjectByName_8);
                    }
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_9 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, static_cast<pas::WideString>(pas::concat_ansi({"Ship.Keller.K", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_9 = pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[KlingKind]);
                        SE_Space::RetainSpaceObject(cpp_arg_9, createSpaceObjectByName_9);
                    }
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_10 = SE_Process::CreateSpaceObjectByName(u"Ship2"sv, static_cast<pas::WideString>(pas::concat_ansi({"Ship.Terron.T", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_10 = pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[KlingKind]);
                        SE_Space::RetainSpaceObject(cpp_arg_10, createSpaceObjectByName_10);
                    }
                    ++Index;
                }
            }
        }
        Count = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get())->GetBlockCount();
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(Index); ) {
            Section = ([&] {
                EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get());
                std::int32_t index = Index;
                return blockByPath_2->GetBlockByIndex(index);
            }());
            if (Section->CountParams(u"Image"_wref.get()) <= 0) {
                --Count;
            }
        }
        PlanetSpaceTemplates.set_length(Count);
        Count = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get())->GetBlockCount();
        TemplateIndex = 0;
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(Index); ) {
            Section = ([&] {
                EC_BlockPar::TBlockParEC* blockByPath_3 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get());
                std::int32_t index_2 = Index;
                return blockByPath_3->GetBlockByIndex(index_2);
            }());
            if (Section->CountParams(u"Image"_wref.get()) > 0) {
                {
                    SE_Space::TObjectSE* createSpaceObjectByName_11 = SE_Process::CreateSpaceObjectByName(u"Planet"sv, pas::concat_wide({u"Planet.", ([&] {
                        EC_BlockPar::TBlockParEC* blockByPath_4 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get());
                        std::int32_t index_3 = Index;
                        return blockByPath_4->GetBlockNameByIndex(index_3);
                    }())}), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> spaceObject = pas::Var<SE_Space::TObjectSE*>(&PlanetSpaceTemplates[TemplateIndex].SpaceObject);
                    SE_Space::RetainSpaceObject(spaceObject, createSpaceObjectByName_11);
                }
                PlanetSpaceTemplates[TemplateIndex].Radius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Section->GetParam(u"Radius"sv)));
                PlanetSpaceTemplates[TemplateIndex].Style = 0;
                PlanetSpaceTemplates[TemplateIndex].StyleVariant = 0;
                if (Section->CountParams(u"Style"_wref.get()) > 0) {
                    Text = Section->GetParam(u"Style"sv);
                    PlanetSpaceTemplates[TemplateIndex].Style = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                    if (EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) >= 2) {
                        PlanetSpaceTemplates[TemplateIndex].StyleVariant = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv)));
                    }
                }
                ++TemplateIndex;
            }
        }
        PopUp::PopupController = pas::construct_call<PopUp::TfPopUpController>(PopUp::TfPopUpController_Create);
        LoadScreen = pas::construct_call<fLoad::TfLoad>(fLoad::TfLoad_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenLoad] = LoadScreen;
        LoadScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Load"_wref.get(), true);
        LoadScreen->InitializeLayout();
        MainMenuScreen = pas::construct_call<fMainForm::TfMainForm>(fMainForm::TfMainForm_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenMainMenu] = MainMenuScreen;
        MainMenuScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"MainForm"_wref.get(), true);
        PlanetQuestScreen = pas::construct_call<fPlanetQuest::TfPlanetQuest>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenPlanetQuest] = PlanetQuestScreen;
        PlanetQuestScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"PlanetQuest"_wref.get(), true);
        GameLoadScreen = pas::construct_call<fGameLoad::TfGameLoad>(fGameLoad::TfGameLoad_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGameLoad] = GameLoadScreen;
        GameLoadScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"GameLoad"_wref.get(), true);
        NewGameScreen = pas::construct_call<fGameSettings2::TfGameSettings2>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenNewGame] = NewGameScreen;
        NewGameScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"GameSettings"_wref.get(), true);
        IntroductionScreen = pas::construct_call<fIntroduction::TfIntroduction>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenIntroduction] = IntroductionScreen;
        IntroductionScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Introduction"_wref.get(), true);
        HangarScreen = pas::construct_call<fHangar::TfHangar>(fHangar::TfHangar_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenHangar] = HangarScreen;
        HangarScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Hangar"_wref.get(), true);
        PlanetScreen = pas::construct_call<fPlanet::TfPlanet>(fPlanet::TfPlanet_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenPlanet] = PlanetScreen;
        PlanetScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Planet"_wref.get(), true);
        UninhabitedPlanetScreen = pas::construct_call<fPlanetNO::TfPlanetNO>(fPlanetNO::TfPlanetNO_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenPlanetNO] = UninhabitedPlanetScreen;
        UninhabitedPlanetScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"PlanetNO"_wref.get(), true);
        RuinsTalkScreen = pas::construct_call<fRuinsTalk::TfRuinsTalk>(fRuinsTalk::TfRuinsTalk_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenRuinsTalk] = RuinsTalkScreen;
        RuinsTalkScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"RuinsTalk"_wref.get(), true);
        ArcadeBattleScreen = pas::construct_call<ab_MainForm::TfAB>(ab_MainForm::TfAB_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenArcadeBattle] = ArcadeBattleScreen;
        ArcadeBattleScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"AB"_wref.get(), true);
        GovernmentScreen = pas::construct_call<fGov::TfGov>(fGov::TfGov_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGovernment] = GovernmentScreen;
        GovernmentScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Gov"_wref.get(), true);
        InfoScreen = pas::construct_call<fInfo::TfInfo>(fInfo::TfInfo_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenInfo] = InfoScreen;
        InfoScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Info"_wref.get(), true);
        RangerRatingScreen = pas::construct_call<fRating2::TfRating2>(fRating2::TfRating2_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenRating] = RangerRatingScreen;
        RangerRatingScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Rating"_wref.get(), true);
        RewardsScreen = pas::construct_call<fRewards::TfRewards>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenRewards] = RewardsScreen;
        RewardsScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Rewards"_wref.get(), true);
        ShipScreen = pas::construct_call<fShip2::TfShip2>(fShip2::TfShip2_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenShip] = ShipScreen;
        ShipScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Ship"_wref.get(), true);
        TalkScreen = pas::construct_call<fTalk::TfTalk>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenTalk] = TalkScreen;
        TalkScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Talk"_wref.get(), true);
        ScannerScreen = pas::construct_call<fScaner::TfScaner>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenScanner] = ScannerScreen;
        ScannerScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Scaner"_wref.get(), true);
        StarMapScreen = pas::construct_call<fStarMap::TfStarMap>(fStarMap::TfStarMap_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenStarMap] = StarMapScreen;
        StarMapScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"StarMap"_wref.get(), true);
        FilmScreen = pas::construct_call<fFilm::TfFilm>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenFilm] = FilmScreen;
        FilmScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Film"_wref.get(), true);
        GalaxyScreen = pas::construct_call<fGalaxy2::TfGalaxy2>(fGalaxy2::TfGalaxy2_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGalaxy] = GalaxyScreen;
        GalaxyScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Galaxy"_wref.get(), true);
        JumpScreen = pas::construct_call<fJump::TfJump>(fJump::TfJump_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenJump] = JumpScreen;
        JumpScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Jump"_wref.get(), true);
        EquipmentShopScreen = pas::construct_call<fEquipmentShop::TfEquipmentShop>(fEquipmentShop::TfEquipmentShop_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenEquipmentShop] = EquipmentShopScreen;
        EquipmentShopScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"EquipmentShop"_wref.get(), true);
        GoodsShopScreen = pas::construct_call<fGoodsShop2::TfGoodsShop2>(fGoodsShop2::TfGoodsShop2_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGoodsShop] = GoodsShopScreen;
        GoodsShopScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"GoodsShop"_wref.get(), true);
        SaveManagerScreen = pas::construct_call<fSaveManager::TfSaveManager>(fSaveManager::TfSaveManager_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenSaveManager] = SaveManagerScreen;
        SaveManagerScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"SaveManager"_wref.get(), true);
        GameMenuScreen = pas::construct_call<fGameMenu::TfGameMenu>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGameMenu] = GameMenuScreen;
        GameMenuScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"GameMenu"_wref.get(), true);
        SettingsScreen = pas::construct_call<fCfgSettings::TfCfgSettings>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenSettings] = SettingsScreen;
        SettingsScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"CfgSettings"_wref.get(), true);
        GameEndScreen = pas::construct_call<fGameEnd::TfGameEnd>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenGameEnd] = GameEndScreen;
        GameEndScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"GameEnd"_wref.get(), true);
        AboutScreen = pas::construct_call<fAbout::TfAbout>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenAbout] = AboutScreen;
        AboutScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"About"_wref.get(), true);
        ScoreScreen = pas::construct_call<fScore::TfScore>(fScore::TfScore_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenScores] = ScoreScreen;
        ScoreScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Score"_wref.get(), true);
        SelectFaceScreen = pas::construct_call<fSelectFace::TfSelectFace>(GI_MessageLoop::TMessageLoopGI_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenSelectFace] = SelectFaceScreen;
        SelectFaceScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"SelectFace"_wref.get(), true);
        JournalScreen = pas::construct_call<fJournal::TfJournal>(fJournal::TfJournal_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenJournal] = JournalScreen;
        JournalScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Journal"_wref.get(), true);
        LoadRobotScreen = pas::construct_call<fLoadRobot::TfLoadRobot>(fLoadRobot::TfLoadRobot_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenLoadRobot] = LoadRobotScreen;
        LoadRobotScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"LoadRobot"_wref.get(), true);
        LoadQuestScreen = pas::construct_call<fLoadQuest::TfLoadQuest>(fLoadQuest::TfLoadQuest_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenLoadQuest] = LoadQuestScreen;
        LoadQuestScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"LoadQuest"_wref.get(), true);
        LoadArcadeScreen = pas::construct_call<fLoadAB::TfLoadAB>(fLoadAB::TfLoadAB_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenLoadArcade] = LoadArcadeScreen;
        LoadArcadeScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"LoadAB"_wref.get(), true);
        AchievementsScreen = pas::construct_call<fAchievements::TfAchievements>(fAchievements::TfAchievements_Create);
        GlobalsV::RegisteredScreens[GlobalsV::screenAchievements] = AchievementsScreen;
        AchievementsScreen->InitializeFromConfig(GR_Main::UiStyleConfig, u"Achievements"_wref.get(), true);
        SpaceObjectUiLoop = pas::construct_call<GI_MessageLoop::TMessageLoopGI>(GI_MessageLoop::TMessageLoopGI_Create);
        SpaceObjectUiLoop->InitializeDefaults();
        SpaceObjectUiLoop->ViewportRect = ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height);
        SpaceObjectUiLoop->UpdateRectsEnabled = false;
        SpaceObjectUiLoop->ContentPanel->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
        SpaceObjectUiLoop->ContentPanel->SetPosition(ClassesImports::Point(pas::shr(GR_Main::RenderScratchBuffer->Width, 1), pas::shr(GR_Main::RenderScratchBuffer->Height, 1)));
        PrimaryFilm = pas::construct_call<aEFilm::TEFilm>(aEFilm::TEFilm_Create);
        SecondaryFilm = pas::construct_call<aEFilm::TEFilm>(aEFilm::TEFilm_Create);
        Globals::RecreateSpaceProcess(u"Process.Normal"_wref.get());
        PlanetRenderTemplates = pas::make_object<pas::List>();
        TPlanetTempl* PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 33;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask052"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light052"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask066"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light066"_w;
        PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 60;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask094"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light094"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask120"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light120"_w;
        PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 70;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask108"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light108"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask140"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light140"_w;
        PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 80;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask124"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light124"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask160"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light160"_w;
        PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 90;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask142"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light142"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask180"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light180"_w;
        PlanetTemplate = pas::make_object<TPlanetTempl>();
        pas::list_add(PlanetRenderTemplates, reinterpret_cast<void*>(PlanetTemplate));
        PlanetTemplate->Radius = 100;
        PlanetTemplate->SmallMaskName = u"Bm.Planet.S.Mask156"_w;
        PlanetTemplate->SmallLightName = u"Bm.Planet.S.Light156"_w;
        PlanetTemplate->MaskName = u"Bm.Planet.S.Mask200"_w;
        PlanetTemplate->LightName = u"Bm.Planet.S.Light200"_w;
        GlobalsV::SatelliteRenderTemplates = pas::make_object<pas::List>();
        Index = GlobalsV::MinimumSatelliteTemplateRadius;
        while (!(Index > GlobalsV::MaximumSatelliteTemplateRadius)) {
            SatelliteTemplate = pas::make_object<TSputnikTempl>();
            pas::list_add(GlobalsV::SatelliteRenderTemplates, reinterpret_cast<void*>(SatelliteTemplate));
            SatelliteTemplate->MaskName = static_cast<pas::WideString>(pas::concat_ansi({"Bm.Planet.S.Mask0", SysUtils::IntToStr(Index), "?", SysUtils::IntToStr(GlobalsV::SatelliteTemplateParameter1), ",", SysUtils::IntToStr(GlobalsV::SatelliteTemplateParameter2)}));
            SatelliteTemplate->Radius = Index;
            ++Index;
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"SpaceImg"sv);
        Count = Section->GetParamCount();
        GlobalsV::SpaceImageTemplates.set_length(Count);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(Index); ) {
            Text = Section->GetParamValue(Index);
            if (EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) < 2) {
                pas::raise(pas::make_exception<pas::Exception>("Error in GlobalsInit"_a));
            }
            GlobalsV::SpaceImageTemplates[Index].Kind = EC_Str::ExtractDigitsToIntW(pas::view(Section->GetParamName(Index)));
            GlobalsV::SpaceImageTemplates[Index].Weight = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
            GlobalsV::SpaceImageTemplates[Index].CacheControl = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
            GlobalsV::SpaceImageTemplates[Index].CachedData = nullptr;
            EC_Cache::TCacheEC::ResetControl(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl));
            reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl)->SetCacheKey(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv));
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"StarFieldImg"sv);
        Count = Section->GetParamCount();
        GlobalsV::StarFieldImageTemplates.set_length(Count);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(Index); ) {
            GlobalsV::StarFieldImageTemplates[Index].Weight = EC_Str::ExtractDigitsToIntW(pas::view(Section->GetParamName(Index)));
            GlobalsV::StarFieldImageTemplates[Index].CacheControl = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
            GlobalsV::StarFieldImageTemplates[Index].CachedData = nullptr;
            EC_Cache::TCacheEC::ResetControl(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Index].CacheControl));
            reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Index].CacheControl)->SetCacheKey(Section->GetParamValue(Index));
        }
        ReloadScriptTemplates = false;
        aConst::InitializeGameplayConfig();
        Globals::InitializeShipGreetingDefinitions();
        Globals::InitializeGovernmentGreetingDefinitions();
        Globals::InitializePlanetAdvertDefinitions();
        Globals::InitializeRobotMapDefinitions();
        UselessItemRemainsCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"UselessItems.CntRemains"_wref.get())));
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"sv)->GetBlock(u"Explosion"sv);
        Count = Section->GetParamCount();
        ArcadeExplosionSounds.set_length(Count);
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(Index); ) {
            ArcadeExplosionSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"sv)->GetBlock(u"Hit"sv);
        Count = Section->GetParamCount();
        ArcadeHitSounds.set_length(Count);
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(Index); ) {
            ArcadeHitSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"sv)->GetBlock(u"Item"sv);
        Count = Section->GetParamCount();
        ArcadeItemSounds.set_length(Count);
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_12.next(Index); ) {
            ArcadeItemSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"sv)->GetBlock(u"WeaponFirst"sv);
        for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, 17); cpp_range_13.next(Index); ) {
            if (Section->CountParams(pas::wide_int_to_str(Index)) <= 0) {
                ArcadeWeaponFirstSounds[Index] = pas::WideString();
            } else {
                ArcadeWeaponFirstSounds[Index] = Section->GetParam(pas::view(pas::wide_int_to_str(Index)));
            }
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"sv)->GetBlock(u"WeaponLoop"sv);
        for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, 17); cpp_range_14.next(Index); ) {
            if (Section->CountParams(pas::wide_int_to_str(Index)) <= 0) {
                ArcadeWeaponLoopSounds[Index] = pas::WideString();
                ArcadeWeaponLoopTicks[Index] = -1;
            } else {
                Text = Section->GetParam(pas::view(pas::wide_int_to_str(Index)));
                ArcadeWeaponLoopTicks[Index] = pas::idiv(EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv))), ab_Global::ArcadeTickMs);
                ArcadeWeaponLoopSounds[Index] = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv);
            }
        }
    }

    void FinalizeGlobalUiRuntime() {
        std::int32_t Index{};
        TSputnikTempl* SatelliteTemplate{};
        TPlanetTempl* PlanetTemplate{};
        GlobalsV::TGameScreenId ScreenIndex{};
        fEquipmentShop::TShopSlot* Slot{};
        ArcadeHitSounds = nullptr;
        ArcadeExplosionSounds = nullptr;
        ArcadeItemSounds = nullptr;
        std::int32_t Count = GlobalsV::SpaceImageTemplates.length() - 1 + 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl) != nullptr) {
                pas::free(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl));
                GlobalsV::SpaceImageTemplates[Index].CacheControl = nullptr;
            }
        }
        GlobalsV::SpaceImageTemplates = nullptr;
        Count = GlobalsV::StarFieldImageTemplates.length() - 1 + 1;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            if (reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Index].CacheControl) != nullptr) {
                pas::free(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::StarFieldImageTemplates[Index].CacheControl));
                GlobalsV::StarFieldImageTemplates[Index].CacheControl = nullptr;
            }
        }
        GlobalsV::StarFieldImageTemplates = nullptr;
        if (PlanetRenderTemplates != nullptr) {
            Count = pas::list_count(PlanetRenderTemplates);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
                PlanetTemplate = pas::list_at<TPlanetTempl>(PlanetRenderTemplates, Index);
                pas::free(PlanetTemplate);
            }
            pas::free(PlanetRenderTemplates);
            PlanetRenderTemplates = nullptr;
        }
        if (GlobalsV::SatelliteRenderTemplates != nullptr) {
            Count = pas::list_count(GlobalsV::SatelliteRenderTemplates);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(Index); ) {
                SatelliteTemplate = pas::list_at<TSputnikTempl>(GlobalsV::SatelliteRenderTemplates, Index);
                pas::free(SatelliteTemplate);
            }
            pas::free(GlobalsV::SatelliteRenderTemplates);
            GlobalsV::SatelliteRenderTemplates = nullptr;
        }
        if (SpaceProcess != nullptr) {
            pas::free(SpaceProcess);
            SpaceProcess = nullptr;
        }
        if (aScript::GameplayUiScriptCache != nullptr) {
            pas::free(aScript::GameplayUiScriptCache);
            aScript::GameplayUiScriptCache = nullptr;
        }
        if (fEquipmentShop::TemporaryShopSlots != nullptr) {
            Count = pas::list_count(fEquipmentShop::TemporaryShopSlots);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(Index); ) {
                Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, Index);
                if (Slot->SlotImage != nullptr) {
                    pas::free(Slot->SlotImage);
                    Slot->SlotImage = nullptr;
                }
                if (Slot->BorderImage != nullptr) {
                    pas::free(Slot->BorderImage);
                    Slot->BorderImage = nullptr;
                }
                if (Slot->TypeOverlayImage != nullptr) {
                    pas::free(Slot->TypeOverlayImage);
                    Slot->TypeOverlayImage = nullptr;
                }
                if (Slot->ItemIconImage != nullptr) {
                    pas::free(Slot->ItemIconImage);
                    Slot->ItemIconImage = nullptr;
                }
                if (Slot->ItemAnimation != nullptr) {
                    pas::free(Slot->ItemAnimation);
                    Slot->ItemAnimation = nullptr;
                }
                if (Slot->MicroModuleImage != nullptr) {
                    pas::free(Slot->MicroModuleImage);
                    Slot->MicroModuleImage = nullptr;
                }
            }
        }
        if (MainMenuScreen != nullptr) {
            pas::free(MainMenuScreen);
            MainMenuScreen = nullptr;
        }
        if (NewGameScreen != nullptr) {
            pas::free(NewGameScreen);
            NewGameScreen = nullptr;
        }
        if (IntroductionScreen != nullptr) {
            pas::free(IntroductionScreen);
            IntroductionScreen = nullptr;
        }
        if (HangarScreen != nullptr) {
            pas::free(HangarScreen);
            HangarScreen = nullptr;
        }
        if (PlanetScreen != nullptr) {
            pas::free(PlanetScreen);
            PlanetScreen = nullptr;
        }
        if (UninhabitedPlanetScreen != nullptr) {
            pas::free(UninhabitedPlanetScreen);
            UninhabitedPlanetScreen = nullptr;
        }
        if (PlanetQuestScreen != nullptr) {
            pas::free(PlanetQuestScreen);
            PlanetQuestScreen = nullptr;
        }
        if (RuinsTalkScreen != nullptr) {
            pas::free(RuinsTalkScreen);
            RuinsTalkScreen = nullptr;
        }
        if (ArcadeBattleScreen != nullptr) {
            pas::free(ArcadeBattleScreen);
            ArcadeBattleScreen = nullptr;
        }
        if (EquipmentShopScreen != nullptr) {
            pas::free(EquipmentShopScreen);
            EquipmentShopScreen = nullptr;
        }
        if (GoodsShopScreen != nullptr) {
            pas::free(GoodsShopScreen);
            GoodsShopScreen = nullptr;
        }
        if (GovernmentScreen != nullptr) {
            pas::free(GovernmentScreen);
            GovernmentScreen = nullptr;
        }
        if (InfoScreen != nullptr) {
            pas::free(InfoScreen);
            InfoScreen = nullptr;
        }
        if (RangerRatingScreen != nullptr) {
            pas::free(RangerRatingScreen);
            RangerRatingScreen = nullptr;
        }
        if (RewardsScreen != nullptr) {
            pas::free(RewardsScreen);
            RewardsScreen = nullptr;
        }
        if (ShipScreen != nullptr) {
            pas::free(ShipScreen);
            ShipScreen = nullptr;
        }
        if (TalkScreen != nullptr) {
            pas::free(TalkScreen);
            TalkScreen = nullptr;
        }
        if (ScannerScreen != nullptr) {
            pas::free(ScannerScreen);
            ScannerScreen = nullptr;
        }
        if (StarMapScreen != nullptr) {
            pas::free(StarMapScreen);
            StarMapScreen = nullptr;
        }
        if (FilmScreen != nullptr) {
            pas::free(FilmScreen);
            FilmScreen = nullptr;
        }
        if (GalaxyScreen != nullptr) {
            pas::free(GalaxyScreen);
            GalaxyScreen = nullptr;
        }
        if (JumpScreen != nullptr) {
            pas::free(JumpScreen);
            JumpScreen = nullptr;
        }
        if (LoadScreen != nullptr) {
            pas::free(LoadScreen);
            LoadScreen = nullptr;
        }
        if (SaveManagerScreen != nullptr) {
            pas::free(SaveManagerScreen);
            SaveManagerScreen = nullptr;
        }
        if (GameLoadScreen != nullptr) {
            pas::free(GameLoadScreen);
            GameLoadScreen = nullptr;
        }
        if (GameMenuScreen != nullptr) {
            pas::free(GameMenuScreen);
            GameMenuScreen = nullptr;
        }
        if (SettingsScreen != nullptr) {
            pas::free(SettingsScreen);
            SettingsScreen = nullptr;
        }
        if (GameEndScreen != nullptr) {
            pas::free(GameEndScreen);
            GameEndScreen = nullptr;
        }
        if (AboutScreen != nullptr) {
            pas::free(AboutScreen);
            AboutScreen = nullptr;
        }
        if (ScoreScreen != nullptr) {
            pas::free(ScoreScreen);
            ScoreScreen = nullptr;
        }
        if (SelectFaceScreen != nullptr) {
            pas::free(SelectFaceScreen);
            SelectFaceScreen = nullptr;
        }
        if (JournalScreen != nullptr) {
            pas::free(JournalScreen);
            JournalScreen = nullptr;
        }
        if (SpaceObjectUiLoop != nullptr) {
            pas::free(SpaceObjectUiLoop);
            SpaceObjectUiLoop = nullptr;
        }
        if (LoadRobotScreen != nullptr) {
            pas::free(LoadRobotScreen);
            LoadRobotScreen = nullptr;
        }
        if (LoadQuestScreen != nullptr) {
            pas::free(LoadQuestScreen);
            LoadQuestScreen = nullptr;
        }
        if (LoadArcadeScreen != nullptr) {
            pas::free(LoadArcadeScreen);
            LoadArcadeScreen = nullptr;
        }
        // Native code omits AchievementsScreen from this cleanup list.
        for (auto cpp_range_6 = pas::for_to<GlobalsV::TGameScreenId>(GlobalsV::screenNone, GlobalsV::screenAchievements); cpp_range_6.next(ScreenIndex); ) {
            GlobalsV::RegisteredScreens[ScreenIndex] = nullptr;
        }
        if (PopUp::PopupController != nullptr) {
            pas::free(PopUp::PopupController);
            PopUp::PopupController = nullptr;
        }
        if (SecondaryFilm != nullptr) {
            pas::free(SecondaryFilm);
            SecondaryFilm = nullptr;
        }
        if (PrimaryFilm != nullptr) {
            pas::free(PrimaryFilm);
            PrimaryFilm = nullptr;
        }
        if (PlanetSpaceTemplates != nullptr) {
            Count = PlanetSpaceTemplates.length() - 1;
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count); cpp_range_7.next(Index); ) {
                if (PlanetSpaceTemplates[Index].SpaceObject != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PlanetSpaceTemplates[Index].SpaceObject));
                }
            }
        }
        PlanetSpaceTemplates = nullptr;
        aSaveLoad::FinalizeSaveWriter();
        if (CacheLoader != nullptr) {
            pas::free(CacheLoader);
            CacheLoader = nullptr;
        }
    }

    void ResetScriptHostRuntimeState() {
        aGalaxyStruct::TOwnerId Race{};
        aGalaxyStruct::THullType Kind{};
        aGalaxyStruct::TKlingType KlingKind{};
        std::int32_t Index{};
        if (ScriptTemplates != nullptr) {
            const std::int32_t cpp_first = pas::list_count(ScriptTemplates) - 1;
            if (cpp_first >= 0) {
                for (Index = cpp_first; Index >= 0; --Index) {
                    pas::free(pas::list_at<pas::Object>(ScriptTemplates, Index));
                    pas::list_delete(ScriptTemplates, Index);
                }
            }
        }
        if (GlobalScriptVariables != nullptr) {
            GlobalScriptVariables->Clear();
            GlobalScriptVariables->Add(u"GRunFrom"_wref.get(), EC_Expression::vkInt)->SetInt(0);
            GlobalScriptVariables->Add(u"GRunStar"_wref.get(), EC_Expression::vkDword)->SetInt(0);
        }
        if (aScript::ArtefactScriptCache != nullptr) {
            pas::free(aScript::ArtefactScriptCache);
            aScript::ArtefactScriptCache = nullptr;
        }
        if (aScript::ArtefactKindScriptCache != nullptr) {
            pas::free(aScript::ArtefactKindScriptCache);
            aScript::ArtefactKindScriptCache = nullptr;
        }
        if (aScript::UselessItemScriptCache != nullptr) {
            pas::free(aScript::UselessItemScriptCache);
            aScript::UselessItemScriptCache = nullptr;
        }
        if (aScript::CustomShipInfoScriptCache != nullptr) {
            pas::free(aScript::CustomShipInfoScriptCache);
            aScript::CustomShipInfoScriptCache = nullptr;
        }
        if (aScript::ScriptLibraryCache != nullptr) {
            pas::free(aScript::ScriptLibraryCache);
            aScript::ScriptLibraryCache = nullptr;
        }
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range.next(Race); ) {
            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::THullType>(aGalaxyStruct::htRanger, aGalaxyStruct::htDiplomat); cpp_range_2.next(Kind); ) {
                if (RaceShipTemplates[Race][Kind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][Kind]));
                }
            }
            if (PirateClanShipTemplates[Race] != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]));
            }
        }
        for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_3.next(KlingKind); ) {
            if (KlingKind != aGalaxyStruct::ktBoss) {
                if (BlazerShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[KlingKind]));
                }
                if (KellerShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[KlingKind]));
                }
                if (TerronShipTemplates[KlingKind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[KlingKind]));
                }
            }
        }
        ReloadScriptTemplates = true;
    }

    // Case-sensitive; returns nil when absent.
    GI_MessageLoop::TMessageLoopGI* FindMessageLoop(const std::u16string_view& Name) {
        GlobalsV::TGameScreenId Index{};
        GI_MessageLoop::TMessageLoopGI* Result = nullptr;
        for (auto cpp_range = pas::for_to<GlobalsV::TGameScreenId>(GlobalsV::screenNone, GlobalsV::screenAchievements); cpp_range.next(Index); ) {
            if (reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]) != nullptr && pas::class_cast_if<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]) != nullptr && pas::view(pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index])->RegisteredLoopName) == Name) {
                Result = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]);
                break;
            }
        }
        return Result;
    }

    // Consumes RequestedScreenId before each run; zero ends dispatch. StarMap, Film and arcade battle use RunContinuous.
    void RunMainScreenStateLoop() {
        while (true) {
            if (GR_Main::ExitScreenLoop) {
                break;
            }
            if (GlobalsV::RequestedScreenId == GlobalsV::screenStarMap || GlobalsV::RequestedScreenId == GlobalsV::screenFilm || GlobalsV::RequestedScreenId == GlobalsV::screenArcadeBattle) {
                GlobalsV::CurrentScreenId = GlobalsV::RequestedScreenId;
                GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RunContinuous();
                GlobalsV::PreviousScreenId = GlobalsV::CurrentScreenId;
                GlobalsV::CurrentScreenId = GlobalsV::screenNone;
            } else {
                if (GlobalsV::RequestedScreenId == GlobalsV::screenNone) {
                    return;
                }
                GlobalsV::CurrentScreenId = GlobalsV::RequestedScreenId;
                GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->Run();
                GlobalsV::PreviousScreenId = GlobalsV::CurrentScreenId;
                GlobalsV::CurrentScreenId = GlobalsV::screenNone;
            }
        }
    }

    // Exchanges the producer and playback films.
    void SwapTurnFilms() {
        aEFilm::TEFilm* Film = PrimaryFilm;
        PrimaryFilm = SecondaryFilm;
        SecondaryFilm = Film;
    }

    // Follows ChildLoop from the current registered screen.
    GI_MessageLoop::TMessageLoopGI* GetInnermostScreenLoop() {
        GI_MessageLoop::TMessageLoopGI* Result = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
        while (Result->ChildLoop != nullptr) {
            Result = Result->ChildLoop;
        }
        return Result;
    }

    // Key is an optional substring; returned queue node is borrowed.
    TMessagePlayer* FindPlayerMessageExceptKinds(const pas::WideString& Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock) {
        TMessagePlayer* MessageEntry{};
        TMessagePlayer* Result = nullptr;
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            MessageEntry = FirstPersistentPlayerMessage;
            while (MessageEntry != nullptr) {
                if (static_cast<std::uint8_t>(pas::contains(ExcludedKinds, MessageEntry->Kind) ^ 1) && (Key.length() == 0 || pas::pos(Key, MessageEntry->Key) > 0)) {
                    Result = MessageEntry;
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                MessageEntry = MessageEntry->Next;
            }
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Result;
    }

    // Returns whether any queued message was removed.
    std::uint8_t RemovePlayerMessagesExceptKinds(pas::WideString Key, TPlayerMessageKindSet ExcludedKinds, std::uint8_t SkipLock) {
        TMessagePlayer* MessageEntry{};
        std::uint8_t Result = false;
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            std::exception_ptr cpp_error{};
            try {
                do {
                    MessageEntry = Globals::FindPlayerMessageExceptKinds(Key, ExcludedKinds, true);
                    if (MessageEntry != nullptr) {
                        Globals::RemovePersistentPlayerMessage(MessageEntry, true);
                        Result = true;
                    }
                } while (!(MessageEntry == nullptr));
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    void ClearPersistentPlayerMessages() {
        TMessagePlayer* Next{};
        TMessagePlayer* Entry{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            Next = FirstPersistentPlayerMessage;
            while (Next != nullptr) {
                Entry = Next;
                Next = Next->Next;
                pas::free(Entry);
            }
            FirstPersistentPlayerMessage = nullptr;
            LastPersistentPlayerMessage = nullptr;
        }
    }

    std::int32_t CountPersistentPlayerMessages() {
        std::int32_t Result{};
        TMessagePlayer* Entry{};
        std::int32_t Count{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            Count = 0;
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                ++Count;
                Entry = Entry->Next;
            }
            Result = Count;
            pas::critical_leave(PersistentPlayerMessageLock);
        }
        return Result;
    }

    std::uint8_t IsPersistentPlayerMessageQueued(TMessagePlayer* MessageEntry, std::uint8_t SkipLock) {
        std::uint8_t Result{};
        TMessagePlayer* Entry{};
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                if (Entry == MessageEntry) {
                    Result = true;
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Entry = Entry->Next;
            }
            Result = false;
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Result;
    }

    // Requires a queued node; unlinks and frees it.
    void RemovePersistentPlayerMessage(TMessagePlayer* MessageEntry, std::uint8_t SkipLock) {
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            std::exception_ptr cpp_error{};
            try {
                if (MessageEntry->Prev != nullptr) {
                    MessageEntry->Prev->Next = MessageEntry->Next;
                }
                if (MessageEntry->Next != nullptr) {
                    MessageEntry->Next->Prev = MessageEntry->Prev;
                }
                if (LastPersistentPlayerMessage == MessageEntry) {
                    LastPersistentPlayerMessage = MessageEntry->Prev;
                }
                if (FirstPersistentPlayerMessage == MessageEntry) {
                    FirstPersistentPlayerMessage = MessageEntry->Next;
                }
                pas::free(MessageEntry);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    // Returns a borrowed queue node or nil.
    TMessagePlayer* FindPlayerBubbleByText(const pas::WideString& Text, std::uint8_t SkipLock) {
        TMessagePlayer* Entry{};
        TMessagePlayer* Result = nullptr;
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                if (Entry->Text == Text) {
                    Result = Entry;
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Entry = Entry->Next;
            }
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Result;
    }

    // Returns a borrowed queue node or nil.
    TMessagePlayer* FindPlayerBubbleByKey(const pas::WideString& Key, std::uint8_t SkipLock) {
        TMessagePlayer* Entry{};
        TMessagePlayer* Result = nullptr;
        if (!SkipLock) {
            pas::critical_enter(PersistentPlayerMessageLock);
        }
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                if (Entry->Key == Key) {
                    Result = Entry;
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Entry = Entry->Next;
            }
            cpp_cleanup:;
            if (!SkipLock) {
                pas::critical_leave(PersistentPlayerMessageLock);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Result;
    }

    // Removes matching prefix keys whose integer suffix is at least FirstPage; leaves unnumbered keys alone.
    void RemovePlayerBubblePages(const pas::WideString& Prefix, std::int32_t FirstPage) {
        TMessagePlayer* Entry{};
        TMessagePlayer* Next{};
        std::int32_t PrefixLength{};
        pas::WideString Suffix{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                Next = Entry->Next;
                PrefixLength = pas::pos(Prefix, Entry->Key);
                if (PrefixLength == 1) {
                    PrefixLength = Prefix.length();
                    Suffix = EC_Str::CopyWideStringUnchecked(Entry->Key, PrefixLength + 1, Entry->Key.length() - PrefixLength);
                    if (EC_Str::IsIntegerTextW(pas::view(Suffix)) && EC_Str::ExtractDigitsToIntW(pas::view(Suffix)) >= FirstPage) {
                        if (Entry->Prev != nullptr) {
                            Entry->Prev->Next = Entry->Next;
                        }
                        if (Entry->Next != nullptr) {
                            Entry->Next->Prev = Entry->Prev;
                        }
                        if (LastPersistentPlayerMessage == Entry) {
                            LastPersistentPlayerMessage = Entry->Prev;
                        }
                        if (FirstPersistentPlayerMessage == Entry) {
                            FirstPersistentPlayerMessage = Entry->Next;
                        }
                        pas::free(Entry);
                    }
                }
                Entry = Next;
            }
        }
    }

    // Removes only the first exact match.
    void RemovePlayerBubbleByKey(const pas::WideString& Key) {
        TMessagePlayer* Entry{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            Entry = FirstPersistentPlayerMessage;
            while (Entry != nullptr) {
                if (Entry->Key == Key) {
                    if (Entry->Prev != nullptr) {
                        Entry->Prev->Next = Entry->Next;
                    }
                    if (Entry->Next != nullptr) {
                        Entry->Next->Prev = Entry->Prev;
                    }
                    if (LastPersistentPlayerMessage == Entry) {
                        LastPersistentPlayerMessage = Entry->Prev;
                    }
                    if (FirstPersistentPlayerMessage == Entry) {
                        FirstPersistentPlayerMessage = Entry->Next;
                    }
                    pas::free(Entry);
                    return;
                }
                Entry = Entry->Next;
            }
        }
    }

    // Appends a new node owned by the global message queue.
    TMessagePlayer* CreatePersistentPlayerMessage() {
        TMessagePlayer* Entry{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            Entry = pas::construct_call<TMessagePlayer>(TMessagePlayer_Create);
            if (LastPersistentPlayerMessage != nullptr) {
                LastPersistentPlayerMessage->Next = Entry;
            }
            Entry->Prev = LastPersistentPlayerMessage;
            Entry->Next = nullptr;
            LastPersistentPlayerMessage = Entry;
            if (FirstPersistentPlayerMessage == nullptr) {
                FirstPersistentPlayerMessage = Entry;
            }
        }
        return Entry;
    }

    // Returns a borrowed queue node. An existing key updates kind/turn and nonempty text; otherwise an exact text match is returned unchanged.
    TMessagePlayer* AddOrUpdatePlayerBubble(TPlayerMessageKind Kind, std::int32_t Turn, const pas::WideString& Text, const pas::WideString& Key) {
        TMessagePlayer* Result{};
        TMessagePlayer* Entry{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            if (Key != u"") {
                Entry = Globals::FindPlayerBubbleByKey(Key, true);
                if (Entry != nullptr) {
                    if (Entry->Kind != Kind) {
                        Entry->WasRead = false;
                        Entry->NotificationSoundPlayed = false;
                    }
                    Entry->Kind = Kind;
                    Entry->Turn = Turn;
                    if (Text != u"") {
                        Entry->Text = Text;
                    }
                    Result = Entry;
                    return Result;
                }
            }
            Entry = Globals::FindPlayerBubbleByText(Text, true);
            if (Entry != nullptr) {
                Result = Entry;
                return Result;
            }
            Entry = pas::construct_call<TMessagePlayer>(TMessagePlayer_Create);
            if (LastPersistentPlayerMessage != nullptr) {
                LastPersistentPlayerMessage->Next = Entry;
            }
            Entry->Prev = LastPersistentPlayerMessage;
            Entry->Next = nullptr;
            LastPersistentPlayerMessage = Entry;
            if (FirstPersistentPlayerMessage == nullptr) {
                FirstPersistentPlayerMessage = Entry;
            }
            Entry->Key = Key;
            Entry->Kind = Kind;
            Entry->Turn = Turn;
            Entry->Text = Text;
            Entry->WasRead = false;
            Entry->NotificationSoundPlayed = false;
            Result = Entry;
        }
        return Result;
    }

    void PruneExpiredPersistentPlayerMessages() {
        TMessagePlayer* Next{};
        TMessagePlayer* Entry{};
        pas::critical_enter(PersistentPlayerMessageLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(PersistentPlayerMessageLock);
            };
            Next = FirstPersistentPlayerMessage;
            while (Next != nullptr) {
                Entry = Next;
                Next = Next->Next;
                if (Entry->WasRead && pas::in_range(Entry->Kind, static_cast<std::int32_t>(pmTip), static_cast<std::int32_t>(pmTip)) && aGalaxy::Galaxy->CurrentTurn - Entry->Turn >= 7) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (aGalaxy::Galaxy->CurrentTurn - Entry->Turn >= PlayerMessagePresentations[Entry->Kind].LifetimeTurns) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (Entry->WasRead && pas::in_set<pmGalaxyNews, pmShipPositive, pmQuestSucceeded, pmQuestCancelled, pmShipNegative, pmShipNegative>(Entry->Kind)) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1) && Entry->Kind == pmRadio) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                }
            }
        }
    }

    // Targets are serialized as all three ship IDs, then all three planet IDs.
    // Prev, Next and Button are not serialized; both flags are persistent.
    // Does not link the object into the global message queue.
    void TMessagePlayer_Create(TMessagePlayer* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Button = nullptr;
        Self->ImageNameOverride = pas::WideString();
    }

    void TMessagePlayer::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(Key);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Kind));
        Buffer->AddIntegerValue(NotificationSoundKind);
        Buffer->AddIntegerValue(Turn);
        Buffer->AddWideStringZ(Text);
        Buffer->AddBoolean(false);
        Buffer->AddDWord(Targets[0].ShipId);
        Buffer->AddDWord(Targets[1].ShipId);
        Buffer->AddDWord(Targets[2].ShipId);
        Buffer->AddDWord(Targets[0].PlanetId);
        Buffer->AddDWord(Targets[1].PlanetId);
        Buffer->AddDWord(Targets[2].PlanetId);
        Buffer->AddBoolean(WasRead);
        Buffer->AddBoolean(NotificationSoundPlayed);
        Buffer->AddWideStringZ(ImageNameOverride);
    }

    // Leaves linkage and Button untouched. ImageNameOverride is present only from save version 109 onward.
    void TMessagePlayer::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        Key = Buffer->ReadWideString();
        Kind = static_cast<TPlayerMessageKind>(EC_Buf::TBufEC_GetByte(Buffer));
        NotificationSoundKind = EC_Buf::TBufEC_GetInt32(Buffer);
        Turn = EC_Buf::TBufEC_GetInt32(Buffer);
        Text = Buffer->ReadWideString();
        EC_Buf::TBufEC_GetBoolean(Buffer);
        Targets[0].ShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
        Targets[1].ShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
        Targets[2].ShipId = EC_Buf::TBufEC_GetUInt32(Buffer);
        Targets[0].PlanetId = EC_Buf::TBufEC_GetUInt32(Buffer);
        Targets[1].PlanetId = EC_Buf::TBufEC_GetUInt32(Buffer);
        Targets[2].PlanetId = EC_Buf::TBufEC_GetUInt32(Buffer);
        WasRead = EC_Buf::TBufEC_GetBoolean(Buffer);
        NotificationSoundPlayed = EC_Buf::TBufEC_GetBoolean(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 109) {
            ImageNameOverride = Buffer->ReadWideString();
        }
    }

    pas::WideString TMessagePlayer::GetNormalImageName() {
        if (ImageNameOverride == u"") {
            return PlayerMessagePresentations[Kind].NormalImage;
        }
        return pas::concat_wide({ImageNameOverride, u"N"});
    }

    pas::WideString TMessagePlayer::GetActiveImageName() {
        if (ImageNameOverride == u"") {
            return PlayerMessagePresentations[Kind].ActiveImage;
        }
        return pas::concat_wide({ImageNameOverride, u"A"});
    }

    pas::WideString TMessagePlayer::GetPressedImageName() {
        if (ImageNameOverride == u"") {
            return PlayerMessagePresentations[Kind].PressedImage;
        }
        return pas::concat_wide({ImageNameOverride, u"D"});
    }

    void TScriptTemplUnit_Create(TScriptTemplUnit* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->ConditionCode = pas::construct_call<EC_Expression::TCodeEC>(EC_Expression::TCodeEC_Create);
        Self->ActiveScriptIndex = -1;
    }

    void TScriptTemplUnit_Destroy(TScriptTemplUnit* Self) {
        pas::free(Self->ConditionCode);
        Self->ConditionCode = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TScriptTemplUnit::p_destroy() {
        Globals::TScriptTemplUnit_Destroy(this);
    }

} // namespace Globals
