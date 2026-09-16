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
    pas::WideString ReadMapText(const pas::WideString& Path, EC_BlockPar::TBlockParEC*& Block);

    pas::WideString ReadShipGreetingField(const pas::WideString& FieldName, EC_BlockPar::TBlockParEC*& Block);

    pas::WideString ReadGovernmentGreetingField(pas::WideString FieldName, EC_BlockPar::TBlockParEC*& Block);

    pas::Array<Globals::TMessagePlayerTypeGraph, 0, 10> PlayerMessagePresentations = pas::Array<Globals::TMessagePlayerTypeGraph, 0, 10>{{
        {.NormalImage = u"GalaxyN"_w, .ActiveImage = u"GalaxyA"_w, .PressedImage = u"GalaxyD"_w, .LifetimeTurns = 10},
        {.NormalImage = u"EtherN"_w, .ActiveImage = u"EtherA"_w, .PressedImage = u"EtherD"_w},
        {.NormalImage = u"ShipPlusN"_w, .ActiveImage = u"ShipPlusA"_w, .PressedImage = u"ShipPlusD"_w, .LifetimeTurns = 5},
        {.NormalImage = u"QuestNormalN"_w, .ActiveImage = u"QuestNormalA"_w, .PressedImage = u"QuestNormalD"_w, .LifetimeTurns = 1000000},
        {.NormalImage = u"QuestOkN"_w, .ActiveImage = u"QuestOkA"_w, .PressedImage = u"QuestOkD"_w, .LifetimeTurns = 1000000},
        {.NormalImage = u"QuestCancelN"_w, .ActiveImage = u"QuestCancelA"_w, .PressedImage = u"QuestCancelD"_w, .LifetimeTurns = 1000000},
        {.NormalImage = u"TipsN"_w, .ActiveImage = u"TipsA"_w, .PressedImage = u"TipsD"_w, .LifetimeTurns = 182},
        {.NormalImage = u"UserN"_w, .ActiveImage = u"UserA"_w, .PressedImage = u"UserD"_w, .LifetimeTurns = 1000000},
        {.NormalImage = u"ShipMinusN"_w, .ActiveImage = u"ShipMinusA"_w, .PressedImage = u"ShipMinusD"_w, .LifetimeTurns = 5},
        {.NormalImage = u"StorageN"_w, .ActiveImage = u"StorageA"_w, .PressedImage = u"StorageD"_w, .LifetimeTurns = 1000000},
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

    GI_MessageLoop::TMessageLoopGI* SpaceObjectUiLoop{};

    fJournal::TfJournal* JournalScreen{};

    fLoadRobot::TfLoadRobot* LoadRobotScreen{};

    fLoadQuest::TfLoadQuest* LoadQuestScreen{};

    fLoadAB::TfLoadAB* LoadArcadeScreen{};

    fAchievements::TfAchievements* AchievementsScreen{};

    EC_Struct::TPointF SpaceViewPosition{};

    std::uint8_t FilmCameraFollow{};

    aShip::TShip* TalkShip{};

    aPlanet::TPlanet* TalkPlanet{};

    std::uint8_t TalkScripted{};

    std::uint8_t TalkType{};

    std::int32_t TalkAmount{};

    std::uint8_t TalkResponse{};

    pas::WideString TalkText{};

    aItem::TItem* ScriptUseItem{};

    pas::List* ScriptItemContextStack{};

    pas::List* ScriptItemInfoContextStack{};

    pas::List* ScriptActionShipStack{};

    pas::List* ScriptActionObject1Stack{};

    pas::List* ScriptActionObject2Stack{};

    pas::List* ScriptActionParamStack{};

    pas::List* ScriptActionTypeStack{};

    std::uint8_t ScreenLoadMode{};

    pas::List* ScriptTemplates{};

    EC_Expression::TVarArrayEC* SharedScriptVariables{};

    EC_Expression::TVarArrayEC* GlobalScriptVariables{};

    std::uint8_t ScriptTemplateStartRequested{};

    pas::WideString LastLoadedPlayerName{};

    std::uint8_t ReloadScriptTemplates = true;

    std::uint8_t ReloadModsRequested = false;

    std::uint8_t StandaloneQuestMode = false;

    pas::Object* ScannerTarget = nullptr;

    std::int32_t ScriptDialogIndex = -1;

    pas::Object* AwardSubject = nullptr;

    std::uint8_t PlayerStarDayPrepared = false;

    std::uint32_t PreviousFilmActivity = 0u;

    std::uint8_t FilmSoundEffectsEnabled = true;

    ThreadCalc::TThreadCalc* TurnCalculationThread = nullptr;

    fFilmFile::TFilmFile* FilmHistory = nullptr;

    fLoad::TCacheLoader* CacheLoader = nullptr;

    fSaveManager::TSaveManagerMode SaveManagerMode = fSaveManager::smmLoad;

    std::uint32_t TalkRequestEvent = 0u;

    std::uint32_t TalkCompletedEvent = 0u;

    std::uint32_t ScriptUiRequestEvent = 0u;

    std::uint32_t ScriptUiAbortEvent = 0u;

    pas::List* PlanetRenderTemplates = nullptr;

    std::int32_t MinimapFrameCounter = 0;

    std::uint8_t Skip1C = false;

    std::uint8_t SkipVideo = false;

    std::uint8_t SkipIntro = false;

    fGameSettings::TThreadCreateNewGame* NewGameGenerationThread{};

    std::uint32_t ShownPlayerTips{};

    std::uint8_t StarMapWeaponPanelOpen{};

    aEFilm::TEFilm* PrimaryFilm{};

    aEFilm::TEFilm* SecondaryFilm{};

    aEFilmEnd::TEFilmEnd* TrailingFilmEffects{};

    SE_Process::TProcessSE* SpaceProcess{};

    EC_Buf::TBufEC* ActiveLoadBuffer{};

    pas::CriticalSection* PersistentPlayerMessageLock{};

    Globals::TMessagePlayer* FirstPersistentPlayerMessage{};

    Globals::TMessagePlayer* LastPersistentPlayerMessage{};

    pas::DynArray<pas::WideString> ArcadeExplosionSounds{};

    pas::DynArray<pas::WideString> ArcadeItemSounds{};

    pas::DynArray<pas::WideString> ArcadeHitSounds{};

    pas::Array<pas::WideString, 0, 17> ArcadeWeaponFirstSounds{};

    pas::Array<pas::WideString, 0, 17> ArcadeWeaponLoopSounds{};

    pas::Array<std::int32_t, 0, 17> ArcadeWeaponLoopTicks{};

    pas::Array<pas::Array<SE_Space::TObjectSE*, 0, 5>, 0, 7> RaceShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> BlazerShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> KellerShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> TerronShipTemplates{};

    pas::Array<SE_Space::TObjectSE*, 0, 7> PirateClanShipTemplates{};

    pas::DynArray<Globals::TPlanetSpaceTemplate> PlanetSpaceTemplates{};

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

    std::int32_t FindScriptTemplateIndex(const pas::WideString& Name) {
        TScriptTemplUnit* Item{};
        std::int32_t Index{};
        std::int32_t Count = pas::list_count(ScriptTemplates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<TScriptTemplUnit>(ScriptTemplates, Index);
            if (Item->Name == Name) {
                return Index;
            }
        }
        return -1;
    }

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

    std::uint8_t ShowPlayerTipOnce(std::int32_t Index) {
        std::uint8_t Result = false;
        if ((pas::shr(ShownPlayerTips, Index) & 1) == 0) {
            ShownPlayerTips |= pas::shl(1, Index);
            if (Index < 10) {
                const pas::WideString& localizedColorText = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Tips.0", SysUtils::IntToStr(Index)})));
                std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(6, currentTurn, localizedColorText, u""_wref.get());
            } else {
                const pas::WideString& localizedColorText_2 = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Tips.", SysUtils::IntToStr(Index)})));
                std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                Globals::AddOrUpdatePlayerBubble(6, currentTurn_2, localizedColorText_2, u""_wref.get());
            }
            return true;
        }
        return Result;
    }

    std::uint8_t HasShownPlayerTip(std::int32_t Index) {
        return (pas::shr(ShownPlayerTips, Index) & 1) != 0;
    }

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

    void HandleRuntimeExitCheck1() {
        static_cast<void>(GR_Main::ExitScreenLoop);
    }

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
        std::uint8_t Race{};
        std::uint8_t Kind{};
        std::uint8_t Series{};
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
        for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_2.next(Race); ) {
            for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(5)); cpp_range_3.next(Kind); ) {
                if (RaceShipTemplates[Race][Kind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][Kind]));
                }
            }
            if (PirateClanShipTemplates[Race] != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]));
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_4.next(Series); ) {
            if (Series != 0) {
                if (BlazerShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[Series]));
                }
                if (KellerShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[Series]));
                }
                if (TerronShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[Series]));
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
                pas::include_at(&Result, 0);
            }
            if (pas::pos("Peleng", Names) > 0) {
                pas::include_at(&Result, 1);
            }
            if (pas::pos("People", Names) > 0) {
                pas::include_at(&Result, 2);
            }
            if (pas::pos("Fei", Names) > 0) {
                pas::include_at(&Result, 3);
            }
            if (pas::pos("Gaal", Names) > 0) {
                pas::include_at(&Result, 4);
            }
        }
        return Result;
    }

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
        Root = GR_Main::LanguageDataConfig->GetBlock(u"RobotsMap"_wref.get());
        std::int32_t Count = Root->GetBlockCount();
        RobotMapDefinitions.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            RobotMapDefinitions[Index].Id = EC_Str::ExtractDigitsToIntW(Root->GetBlockNameByIndex(Index));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Index - 1); cpp_range_2.next(Previous); ) {
                if (RobotMapDefinitions[Index].Id == RobotMapDefinitions[Previous].Id) {
                    GR_Main::RaiseWideMessage(u"RobotMap.Id"_wref.get());
                }
            }
            Block = Root->GetBlockByIndex(Index);
            RobotMapDefinitions[Index].Name = Globals::ReadMapText(u"Name"_wref.get(), Block);
            RobotMapDefinitions[Index].Map = Globals::ReadMapText(u"Map"_wref.get(), Block);
            Text = Globals::ReadMapText(u"Group"_wref.get(), Block);
            if (Text != u"") {
                RobotMapDefinitions[Index].Group = EC_Str::ExtractSignedDigitsToIntW(Globals::ReadMapText(u"Group"_wref.get(), Block));
            } else {
                RobotMapDefinitions[Index].Group = -1;
            }
            RobotMapDefinitions[Index].Access = EC_Str::ExtractSignedDigitsToIntW(Globals::ReadMapText(u"Access"_wref.get(), Block));
            Text = Globals::ReadMapText(u"Side"_wref.get(), Block);
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
            RobotMapDefinitions[Index].Length = EC_Str::ExtractSignedDigitsToIntW(Globals::ReadMapText(u"Length"_wref.get(), Block));
            Text = Globals::ReadMapText(u"PlanetRace"_wref.get(), Block);
            RobotMapDefinitions[Index].PlanetRace = Globals::ParseRobotMapRaceMask(Text);
            Text = Globals::ReadMapText(u"PlayerRace"_wref.get(), Block);
            RobotMapDefinitions[Index].PlayerRace = Globals::ParseRobotMapRaceMask(Text);
            Text = Globals::ReadMapText(u"PlayerStatus"_wref.get(), Block);
            RobotMapDefinitions[Index].PlayerStatus = pas::constant_set<TRobotMapPlayerStatuses>({});
            if (Text != u"" && Text != u"Any") {
                if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, 0);
                }
                if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, 1);
                }
                if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                    pas::include_at(&RobotMapDefinitions[Index].PlayerStatus, 2);
                }
            }
            RobotMapDefinitions[Index].MinWins = EC_Str::ExtractSignedDigitsToIntW(Globals::ReadMapText(u"MinWins"_wref.get(), Block));
            RobotMapDefinitions[Index].MaxWins = EC_Str::ExtractSignedDigitsToIntW(Globals::ReadMapText(u"MaxWins"_wref.get(), Block));
            RobotMapDefinitions[Index].Reiteration = EC_Str::ExtractDigitsToIntW(Globals::ReadMapText(u"Reiteration"_wref.get(), Block));
            RobotMapDefinitions[Index].ReinforcementsDisabled = GI_Main::ParseEnabledNameGI(Globals::ReadMapText(u"ReinforcementsDisabled"_wref.get(), Block));
            RobotMapDefinitions[Index].Terron = GI_Main::ParseEnabledNameGI(Globals::ReadMapText(u"Terron"_wref.get(), Block));
            RobotMapDefinitions[Index].Demo = GI_Main::ParseEnabledNameGI(Globals::ReadMapText(u"Demo"_wref.get(), Block));
            RobotMapDefinitions[Index].AfterLiberation = GI_Main::ParseEnabledNameGI(Globals::ReadMapText(u"AfterLiberation"_wref.get(), Block));
            RobotMapDefinitions[Index].GovTextStart = Globals::ReadMapText(u"GovTextStart"_wref.get(), Block);
            RobotMapDefinitions[Index].GovTextWin = Globals::ReadMapText(u"GovTextWin"_wref.get(), Block);
            RobotMapDefinitions[Index].GovTextLoss = Globals::ReadMapText(u"GovTextLoss"_wref.get(), Block);
            RobotMapDefinitions[Index].RobotsStart = Globals::ReadMapText(u"RobotsStart"_wref.get(), Block);
            RobotMapDefinitions[Index].RobotsWin = Globals::ReadMapText(u"RobotsWin"_wref.get(), Block);
            RobotMapDefinitions[Index].RobotsLoss = Globals::ReadMapText(u"RobotsLoss"_wref.get(), Block);
            RobotMapDefinitions[Index].FromAuthor = Globals::ReadMapText(u"FromAuthor"_wref.get(), Block);
        }
    }

    void InitializeShipGreetingDefinitions() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        std::int32_t EntryIndex{};
        std::int32_t Item{};
        pas::WideString Text{};
        ShipGreetingCount = 0;
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"ShipGreetings.CountShipGreetings"_wref.get())));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"ShipGreetings"_wref.get());
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
                EC_BlockPar::TBlockParEC* block_2 = GR_Main::LanguageDataConfig->GetBlock(u"ShipGreetings"_wref.get());
                return block_2->CountBlocks(intToStr_2);
            }()) != 0) {
                ++ShipGreetingCount;
                EntryIndex = ShipGreetingCount - 1;
                Block = GR_Main::LanguageDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"ShipGreetings.", SysUtils::IntToStr(Index)})));
                {
                    TShipGreetingsInfo& cpp_with = ShipGreetingDefinitions[EntryIndex];
                    cpp_with.Name = pas::wide_int_to_str(Index);
                    Text = Globals::ReadShipGreetingField(u"Priority"_wref.get(), Block);
                    if (Text == u"") {
                        cpp_with.Priority = 10;
                    } else {
                        cpp_with.Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
                    }
                    Text = Globals::ReadShipGreetingField(u"AutoTalk"_wref.get(), Block);
                    if (Text == u"" || Text == u"No") {
                        cpp_with.AutoTalk = 1;
                    } else if (Text == u"Any") {
                        cpp_with.AutoTalk = 2;
                    } else {
                        cpp_with.AutoTalk = 0;
                    }
                    Text = Globals::ReadShipGreetingField(u"FlyType"_wref.get(), Block);
                    if (Text == u"Any" || Text == u"") {
                        cpp_with.FlyType = 0;
                    } else if (Text == u"ToPlanet") {
                        cpp_with.FlyType = 1;
                    } else if (Text == u"ToStar") {
                        cpp_with.FlyType = 2;
                    } else if (Text == u"ToItem") {
                        cpp_with.FlyType = 3;
                    } else if (Text == u"ToShip") {
                        cpp_with.FlyType = 4;
                    } else {
                        GR_Main::RaiseWideMessage(Text);
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipType"_wref.get(), Block);
                    cpp_with.ShipType = pas::constant_set<TGreetingMask>({});
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
                    Text = Globals::ReadShipGreetingField(u"Relations"_wref.get(), Block);
                    cpp_with.Relations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.Relations, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipRace"_wref.get(), Block);
                    cpp_with.ShipRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadShipGreetingField(u"PlayerRace"_wref.get(), Block);
                    cpp_with.PlayerRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadShipGreetingField(u"ShipRaceIsPlayerRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipRaceIsPlayerRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ShipRaceIsPlayerRace = 1;
                    } else {
                        cpp_with.ShipRaceIsPlayerRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerAttackGoodShip"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.PlayerAttackGoodShip = 0;
                    } else if (Text == u"No") {
                        cpp_with.PlayerAttackGoodShip = 1;
                    } else {
                        cpp_with.PlayerAttackGoodShip = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"InFear"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.InFear = 0;
                    } else if (Text == u"Any") {
                        cpp_with.InFear = 2;
                    } else {
                        cpp_with.InFear = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipBadFlyToShip"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipBadFlyToShip = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ShipBadFlyToShip = 2;
                    } else {
                        cpp_with.ShipBadFlyToShip = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipBadType"_wref.get(), Block);
                    cpp_with.ShipBadType = pas::constant_set<TGreetingMask>({});
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
                    Text = Globals::ReadShipGreetingField(u"ShipBadRace"_wref.get(), Block);
                    cpp_with.ShipBadRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadShipGreetingField(u"ShipFlyToPlayer"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipFlyToPlayer = 0;
                    } else if (Text == u"No") {
                        cpp_with.ShipFlyToPlayer = 1;
                    } else {
                        cpp_with.ShipFlyToPlayer = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerFlyToShip"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.PlayerFlyToShip = 0;
                    } else if (Text == u"No") {
                        cpp_with.PlayerFlyToShip = 1;
                    } else {
                        cpp_with.PlayerFlyToShip = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerIsShipBad"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.PlayerIsShipBad = 0;
                    } else if (Text == u"No") {
                        cpp_with.PlayerIsShipBad = 1;
                    } else {
                        cpp_with.PlayerIsShipBad = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipTurnBeforeEndOrder"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerTurnBeforeEndOrder"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipBadTurnBeforeEndOrder"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipStatus"_wref.get(), Block);
                    cpp_with.ShipStatus = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, 0);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, 1);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStatus, 2);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerStatus"_wref.get(), Block);
                    cpp_with.PlayerStatus = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 0);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 1);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 2);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipStrength"_wref.get(), Block);
                    cpp_with.ShipStrength = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Mini", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ShipStrength, 1);
                        }
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
                    Text = Globals::ReadShipGreetingField(u"PlayerStrength"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipStructure"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerStructure"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipRating"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerRating"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipRank"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerRank"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"RatingShipWithPlayer"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"RankShipWithPlayer"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"StrengthShipWithPlayer"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"Goods"_wref.get(), Block);
                    if (Text == u"") {
                        cpp_with.Goods = 42;
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
                        cpp_with.Goods = 42;
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipGoodsCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerGoodsCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipHaveGoods"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipHaveGoods = 0;
                    } else if (Text == u"No") {
                        cpp_with.ShipHaveGoods = 1;
                    } else {
                        cpp_with.ShipHaveGoods = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerHaveGoods"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.PlayerHaveGoods = 0;
                    } else if (Text == u"No") {
                        cpp_with.PlayerHaveGoods = 1;
                    } else {
                        cpp_with.PlayerHaveGoods = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipGoodsTypeCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PlayerGoodsTypeCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ShipMayScanPlayer"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipMayScanPlayer = 0;
                    } else if (Text == u"No") {
                        cpp_with.ShipMayScanPlayer = 1;
                    } else {
                        cpp_with.ShipMayScanPlayer = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"RangerInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PirateInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"KlingInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"WarriorInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"TransportInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"LastPlanetRace"_wref.get(), Block);
                    if (Text == u"Any") {
                        cpp_with.LastPlanetRace = pas::constant_set<aGalaxyStruct::TOwnerMask>({{0, 4}});
                    } else {
                        cpp_with.LastPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetRelations"_wref.get(), Block);
                    cpp_with.LastPlanetRelations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetRelations, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetGoodsCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"LastPlanetGoodsSale"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"LastPlanetGoodsBuy"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"LastPlanetIsHomePlanet"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetIsHomePlanet = 0;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetIsHomePlanet = 1;
                    } else {
                        cpp_with.LastPlanetIsHomePlanet = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetRaceIsShipRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetRaceIsShipRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetRaceIsShipRace = 1;
                    } else {
                        cpp_with.LastPlanetRaceIsShipRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetRaceIsPlayerRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetRaceIsPlayerRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetRaceIsPlayerRace = 1;
                    } else {
                        cpp_with.LastPlanetRaceIsPlayerRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetEconomy"_wref.get(), Block);
                    cpp_with.LastPlanetEconomy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, 0);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, 1);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetEconomy, 2);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetGoverment"_wref.get(), Block);
                    cpp_with.LastPlanetGovernment = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, 0);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, 1);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, 2);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, 3);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.LastPlanetGovernment, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetInCurStar"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.LastPlanetInCurStar = 0;
                    } else if (Text == u"No") {
                        cpp_with.LastPlanetInCurStar = 1;
                    } else {
                        cpp_with.LastPlanetInCurStar = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"LastPlanetDistToShipInTurn"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"RangerInLastPlanetStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PirateInLastPlanetStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"KlingInLastPlanetStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"WarriorInLastPlanetStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"TransportInLastPlanetStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ToPlanetRace"_wref.get(), Block);
                    cpp_with.ToPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadShipGreetingField(u"ToPlanetRelations"_wref.get(), Block);
                    cpp_with.ToPlanetRelations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetGoodsCnt"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ToPlanetGoodsSale"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ToPlanetGoodsBuy"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ToPlanetIsHomePlanet"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetIsHomePlanet = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetIsHomePlanet = 1;
                    } else {
                        cpp_with.ToPlanetIsHomePlanet = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetRaceIsShipRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsShipRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsShipRace = 1;
                    } else {
                        cpp_with.ToPlanetRaceIsShipRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetRaceIsPlayerRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsPlayerRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsPlayerRace = 1;
                    } else {
                        cpp_with.ToPlanetRaceIsPlayerRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetEconomy"_wref.get(), Block);
                    cpp_with.ToPlanetEconomy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 0);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 1);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 2);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetGoverment"_wref.get(), Block);
                    cpp_with.ToPlanetGovernment = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 0);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 1);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 2);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 3);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetIsLastPlanet"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetIsLastPlanet = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToPlanetIsLastPlanet = 2;
                    } else {
                        cpp_with.ToPlanetIsLastPlanet = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToPlanetRaceIsLastPlanetRace"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = 1;
                    } else {
                        cpp_with.ToPlanetRaceIsLastPlanetRace = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"HomePlanetInToStar"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.HomePlanetInToStar = 0;
                    } else if (Text == u"No") {
                        cpp_with.HomePlanetInToStar = 1;
                    } else {
                        cpp_with.HomePlanetInToStar = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"HomePlanetInCurStar"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.HomePlanetInCurStar = 0;
                    } else if (Text == u"No") {
                        cpp_with.HomePlanetInCurStar = 1;
                    } else {
                        cpp_with.HomePlanetInCurStar = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToStarControlByKling"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByKling = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByKling = 2;
                    } else {
                        cpp_with.ToStarControlByKling = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToStarInBattle"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarInBattle = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarInBattle = 2;
                    } else {
                        cpp_with.ToStarInBattle = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"RangerInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PirateInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"KlingInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"WarriorInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"TransportInToStar"_wref.get(), Block);
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
                    cpp_with.ItemType = Globals::ReadShipGreetingField(u"ItemType"_wref.get(), Block);
                    Text = Globals::ReadShipGreetingField(u"ToPlanetGoverment"_wref.get(), Block);
                    cpp_with.ToPlanetGovernment = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 0);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 1);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 2);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 3);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"ShipNeedInItem"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ShipNeedInItem = 0;
                    } else if (Text == u"No") {
                        cpp_with.ShipNeedInItem = 1;
                    } else {
                        cpp_with.ShipNeedInItem = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToShipType"_wref.get(), Block);
                    cpp_with.ToShipType = pas::constant_set<TGreetingMask>({});
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
                    Text = Globals::ReadShipGreetingField(u"ToShipRace"_wref.get(), Block);
                    cpp_with.ToShipRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadShipGreetingField(u"ToShipInPlanet"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToShipInPlanet = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToShipInPlanet = 1;
                    } else {
                        cpp_with.ToShipInPlanet = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToShipBad"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToShipBad = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToShipBad = 1;
                    } else {
                        cpp_with.ToShipBad = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"ToShipRelations"_wref.get(), Block);
                    cpp_with.ToShipRelations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToShipRelations, 4);
                        }
                    }
                    Text = Globals::ReadShipGreetingField(u"PlayerPirateRank"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"RankShipWithPlayer"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"Female"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.Female = 0;
                    } else {
                        cpp_with.Female = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"PirateClanInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"ToStarControlByPirates"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByPirates = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByPirates = 2;
                    } else {
                        cpp_with.ToStarControlByPirates = 1;
                    }
                    Text = Globals::ReadShipGreetingField(u"PirateClanInCurStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"PirateInToStar"_wref.get(), Block);
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
                    Text = Globals::ReadShipGreetingField(u"CoalitionAlreadyDefeated"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.CoalitionAlreadyDefeated = 0;
                    } else if (Text == u"No") {
                        cpp_with.CoalitionAlreadyDefeated = 1;
                    } else {
                        cpp_with.CoalitionAlreadyDefeated = 2;
                    }
                    Text = Globals::ReadShipGreetingField(u"DominatorsAlreadyDefeated"_wref.get(), Block);
                    if (Text == u"Yes") {
                        cpp_with.DominatorsAlreadyDefeated = 0;
                    } else if (Text == u"No") {
                        cpp_with.DominatorsAlreadyDefeated = 1;
                    } else {
                        cpp_with.DominatorsAlreadyDefeated = 2;
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
        GovernmentGreetingCount = 0;
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"GovGreetings.CountGovGreetings"_wref.get())));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(Index);
                EC_BlockPar::TBlockParEC* block = GR_Main::LanguageDataConfig->GetBlock(u"GovGreetings"_wref.get());
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
                EC_BlockPar::TBlockParEC* block_2 = GR_Main::LanguageDataConfig->GetBlock(u"GovGreetings"_wref.get());
                return block_2->CountBlocks(intToStr_2);
            }()) != 0) {
                ++GovernmentGreetingCount;
                EntryIndex = GovernmentGreetingCount - 1;
                Block = GR_Main::LanguageDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"GovGreetings.", SysUtils::IntToStr(Index)})));
                {
                    TGovGreetingsInfo& cpp_with = GovernmentGreetingDefinitions[EntryIndex];
                    cpp_with.Name = pas::wide_int_to_str(Index);
                    Text = Globals::ReadGovernmentGreetingField(u"Priority"_w, Block);
                    if (Text == u"") {
                        cpp_with.Priority = 10;
                    } else {
                        cpp_with.Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(Text));
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"PlayerRace"_w, Block);
                    cpp_with.PlayerRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadGovernmentGreetingField(u"PlayerStatus"_w, Block);
                    cpp_with.PlayerStatus = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Trader", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 0);
                        }
                        if (pas::pos("Pirate", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 1);
                        }
                        if (pas::pos("Warrior", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.PlayerStatus, 2);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"PlayerRating"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"PlayerRank"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"Goods"_w, Block);
                    if (Text == u"") {
                        cpp_with.Goods = 42;
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
                        cpp_with.Goods = 42;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetRace"_w, Block);
                    cpp_with.CurPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetRaceIsPlayerRace"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetRaceIsPlayerRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetRaceIsPlayerRace = 1;
                    } else {
                        cpp_with.CurPlanetRaceIsPlayerRace = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetRelations"_w, Block);
                    cpp_with.CurPlanetRelations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetRelations, 4);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetGoodsPermit"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetGoodsPermit = 0;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetGoodsPermit = 1;
                    } else {
                        cpp_with.CurPlanetGoodsPermit = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetGoodsCnt"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetGoodsSale"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetGoodsBuy"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetEconomy"_w, Block);
                    cpp_with.CurPlanetEconomy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, 0);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, 1);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetEconomy, 2);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetGoverment"_w, Block);
                    cpp_with.CurPlanetGovernment = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, 0);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, 1);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, 2);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, 3);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.CurPlanetGovernment, 4);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"RangerInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"PirateInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"KlingInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"WarriorInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"TransportInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"CurStarInBattle"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CurStarInBattle = 0;
                    } else if (Text == u"Any") {
                        cpp_with.CurStarInBattle = 2;
                    } else {
                        cpp_with.CurStarInBattle = 1;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetRace"_w, Block);
                    if (Text == u"Any") {
                        cpp_with.ToPlanetRace = pas::constant_set<aGalaxyStruct::TOwnerMask>({{0, 4}});
                    } else {
                        cpp_with.ToPlanetRace = Globals::ParseRobotMapRaceMask(Text);
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetRaceIsPlayerRace"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsPlayerRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsPlayerRace = 1;
                    } else {
                        cpp_with.ToPlanetRaceIsPlayerRace = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetRaceIsCurPlanetRace"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = 1;
                    } else {
                        cpp_with.ToPlanetRaceIsCurPlanetRace = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetRelations"_w, Block);
                    cpp_with.ToPlanetRelations = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("War", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 0);
                        }
                        if (pas::pos("Bad", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 1);
                        }
                        if (pas::pos("Normal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 2);
                        }
                        if (pas::pos("Good", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 3);
                        }
                        if (pas::pos("Best", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetRelations, 4);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetGoodsPermit"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToPlanetGoodsPermit = 0;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetGoodsPermit = 1;
                    } else {
                        cpp_with.ToPlanetGoodsPermit = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetGoodsCnt"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetGoodsSale"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetGoodsBuy"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetEconomy"_w, Block);
                    cpp_with.ToPlanetEconomy = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Agriculture", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 0);
                        }
                        if (pas::pos("Mixed", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 1);
                        }
                        if (pas::pos("Industrial", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetEconomy, 2);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetGoverment"_w, Block);
                    cpp_with.ToPlanetGovernment = pas::constant_set<TGreetingMask>({});
                    if (Text != u"" && Text != u"Any") {
                        if (pas::pos("Anarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 0);
                        }
                        if (pas::pos("Dictatorship", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 1);
                        }
                        if (pas::pos("Monarchy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 2);
                        }
                        if (pas::pos("Republic", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 3);
                        }
                        if (pas::pos("Democracy", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&cpp_with.ToPlanetGovernment, 4);
                        }
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToPlanetInCurStar"_w, Block);
                    if (Text == u"Any") {
                        cpp_with.ToPlanetInCurStar = 2;
                    } else if (Text == u"No") {
                        cpp_with.ToPlanetInCurStar = 1;
                    } else {
                        cpp_with.ToPlanetInCurStar = 0;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"RangerInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"PirateInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"KlingInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"WarriorInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"TransportInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"ToStarControlByKling"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByKling = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByKling = 2;
                    } else {
                        cpp_with.ToStarControlByKling = 1;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"ToStarInBattle"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarInBattle = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarInBattle = 2;
                    } else {
                        cpp_with.ToStarInBattle = 1;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurPlanetPirateClan"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CurPlanetPirateClan = 0;
                    } else if (Text == u"No") {
                        cpp_with.CurPlanetPirateClan = 1;
                    } else {
                        cpp_with.CurPlanetPirateClan = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CurStarInBattlePirates"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CurStarInBattlePirates = 0;
                    } else if (Text == u"Any") {
                        cpp_with.CurStarInBattlePirates = 2;
                    } else {
                        cpp_with.CurStarInBattlePirates = 1;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"PirateClanInCurStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"PirateClanInToStar"_w, Block);
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
                    Text = Globals::ReadGovernmentGreetingField(u"ToStarControlByPirates"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.ToStarControlByPirates = 0;
                    } else if (Text == u"Any") {
                        cpp_with.ToStarControlByPirates = 2;
                    } else {
                        cpp_with.ToStarControlByPirates = 1;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"CoalitionAlreadyDefeated"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.CoalitionAlreadyDefeated = 0;
                    } else if (Text == u"No") {
                        cpp_with.CoalitionAlreadyDefeated = 1;
                    } else {
                        cpp_with.CoalitionAlreadyDefeated = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"DominatorsAlreadyDefeated"_w, Block);
                    if (Text == u"Yes") {
                        cpp_with.DominatorsAlreadyDefeated = 0;
                    } else if (Text == u"No") {
                        cpp_with.DominatorsAlreadyDefeated = 1;
                    } else {
                        cpp_with.DominatorsAlreadyDefeated = 2;
                    }
                    Text = Globals::ReadGovernmentGreetingField(u"PlayerPirateRank"_w, Block);
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
            PlanetAdvertDefinitions[GroupIndex].Position = GI_Main::GetPointGI(GroupBlock->GetParamByPathOrMarker(u"Info.Pos"_wref.get()));
            if (GroupBlock->GetBlock(u"Info"_wref.get())->CountParams(u"Image1"_wref.get()) > 0) {
                PlanetAdvertDefinitions[GroupIndex].Image1 = GroupBlock->GetParamByPathOrMarker(u"Info.Image1"_wref.get());
            }
            if (GroupBlock->GetBlock(u"Info"_wref.get())->CountParams(u"Image2"_wref.get()) > 0) {
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
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Image1 = Block->GetParam(u"Image1"_wref.get());
                    }
                    if (Block->CountParams(u"Image2"_wref.get()) > 0) {
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Image2 = Block->GetParam(u"Image2"_wref.get());
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = 0;
                    if (Block->CountParams(u"War"_wref.get()) > 0) {
                        PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = EC_Str::ExtractSignedDigitsToIntW(Block->GetParam(u"War"_wref.get()));
                        if (PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War < -1) {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = -1;
                        } else if (PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War > 1) {
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].War = 1;
                        }
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 42;
                    if (Block->CountParams(u"Goods"_wref.get()) > 0) {
                        Text = Block->GetParam(u"Goods"_wref.get());
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
                            PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Goods = 42;
                        }
                    }
                    PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
                    if (Block->CountParams(u"Owner"_wref.get()) > 0) {
                        Text = Block->GetParam(u"Owner"_wref.get());
                        if (pas::pos("Maloc", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, 0);
                        }
                        if (pas::pos("Peleng", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, 1);
                        }
                        if (pas::pos("People", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, 2);
                        }
                        if (pas::pos("Fei", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, 3);
                        }
                        if (pas::pos("Gaal", static_cast<pas::AnsiString>(Text)) > 0) {
                            pas::include_at(&PlanetAdvertDefinitions[GroupIndex].Adverts[AdvertIndex].Owner, 4);
                        }
                    }
                    ++AdvertIndex;
                    if (AdvertIndex >= Count - 1) {
                        break;
                    }
                }
            }
            Block = GroupBlock->GetBlock(u"List"_wref.get());
            PlanetAdvertDefinitions[GroupIndex].Lists.set_length(Block->GetParamCount());
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, PlanetAdvertDefinitions[GroupIndex].Lists.length() - 1); cpp_range_3.next(BlockIndex); ) {
                PlanetAdvertDefinitions[GroupIndex].Lists[BlockIndex].Key = EC_Str::ExtractDigitsToIntW(Block->GetParamName(BlockIndex));
                Text = Block->GetParamValue(BlockIndex);
                Count = EC_Str::CountDelimitedPartsW(Text, u","_wref.get());
                PlanetAdvertDefinitions[GroupIndex].Lists[BlockIndex].Indices.set_length(Count);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(AdvertIndex); ) {
                    Name = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(Text, AdvertIndex, u","_wref.get()));
                    FoundIndex = 0;
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
        std::uint8_t Race{};
        std::int32_t Index{};
        std::int32_t TemplateIndex{};
        std::int32_t Count{};
        TSputnikTempl* SatelliteTemplate{};
        EC_BlockPar::TBlockParEC* Section{};
        std::uint8_t Series{};
        std::uint8_t Kind{};
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
            GlobalsV::ChangeAutoPilot = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ChangeAutoPilot"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParams(u"AltResolutionSwitch"_wref.get()) > 0) {
            GR_Main::AltResolutionSwitch = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AltResolutionSwitch"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DisableAutoPilot"_wref.get()) > 0) {
            GlobalsV::DisableAutoPilot = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DisableAutoPilot"_wref.get()));
        }
        GlobalsV::UiRuntimeFlag = true;
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PQuestStyle"_wref.get()) > 0) {
            GlobalsV::QuestStyleIndex = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PQuestStyle"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PQuestAnim"_wref.get()) > 0) {
            GlobalsV::QuestPageAnimationEnabled = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PQuestAnim"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DefaultOrder"_wref.get()) > 0) {
            GlobalsV::DefaultOrder = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DefaultOrder"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RightClickOnShip"_wref.get()) > 0) {
            GlobalsV::RightClickOnShip = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RightClickOnShip"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DoNotChangeMusicInBattle"_wref.get()) > 0) {
            GlobalsV::DoNotChangeMusicInBattle = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DoNotChangeMusicInBattle"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ViewFollowShip"_wref.get()) > 0) {
            GlobalsV::ViewFollowShip = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ViewFollowShip"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ViewPathLength"_wref.get()) > 0) {
            GlobalsV::ViewPathLength = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ViewPathLength"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"TurnSaveStep"_wref.get()) > 0) {
            GlobalsV::TurnSaveStep = std::min<std::int32_t>(365, EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"TurnSaveStep"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"QuickSaveExtraSlots"_wref.get()) > 0) {
            GlobalsV::QuickSaveExtraSlots = std::min<std::int32_t>(9, EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"QuickSaveExtraSlots"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"MaxPlayerNews"_wref.get()) > 0) {
            GlobalsV::MaxPlayerNews = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"MaxPlayerNews"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ForsageDeactivatePercent"_wref.get()) > 0) {
            GlobalsV::AfterburnerStopCondition = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ForsageDeactivatePercent"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"MaxSearchResult"_wref.get()) > 0) {
            GlobalsV::MaxSearchResult = std::min<std::int32_t>(100, EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"MaxSearchResult"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ClickAutoCloseForm"_wref.get()) > 0) {
            GlobalsV::ClickAutoCloseForm = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ClickAutoCloseForm"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ActionDoubleClick"_wref.get()) > 0) {
            GlobalsV::ActionDoubleClick = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ActionDoubleClick"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipGiper"_wref.get()) > 0) {
            GlobalsV::SkipGiper = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipGiper"_wref.get()));
        }
        Text = u"e"_w;
        Text = pas::concat_wide({Text, u"s"});
        Text = pas::concat_wide({Text, u"t"});
        if (GR_Main::UserSettingsConfig->CountParamsByPath(Text) > 0) {
            GlobalsV::EstOptionEnabled = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(Text));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SendRecordOff"_wref.get()) > 0) {
            GlobalsV::SendRecordOff = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SendRecordOff"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Wind"_wref.get()) > 0) {
            GlobalsV::Wind = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Wind"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Skip1C"_wref.get()) > 0) {
            Skip1C = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Skip1C"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipVideo"_wref.get()) > 0) {
            SkipVideo = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipVideo"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SkipIntro"_wref.get()) > 0) {
            SkipIntro = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SkipIntro"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ShipTail"_wref.get()) > 0) {
            GlobalsV::ShipTail = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ShipTail"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimCaptain"_wref.get()) > 0) {
            GlobalsV::AnimCaptain = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimCaptain"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimItem"_wref.get()) > 0) {
            GlobalsV::AnimItem = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimItem"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BGImage"_wref.get()) > 0) {
            GlobalsV::BGImage = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BGImage"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"Comet"_wref.get()) > 0) {
            GlobalsV::Comet = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"Comet"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimShipFull"_wref.get()) > 0) {
            GlobalsV::AnimShipFull = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimShipFull"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimCity"_wref.get()) > 0) {
            GlobalsV::AnimCity = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimCity"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimGov"_wref.get()) > 0) {
            GlobalsV::AnimGov = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimGov"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimMenuShip"_wref.get()) > 0) {
            GlobalsV::AnimMenuShip = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimMenuShip"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimStar"_wref.get()) > 0) {
            GlobalsV::AnimStar = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimStar"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimHangar"_wref.get()) > 0) {
            GlobalsV::AnimHangar = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimHangar"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"CircleAction"_wref.get()) > 0) {
            GlobalsV::CircleAction = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"CircleAction"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"StaticBackground"_wref.get()) > 0) {
            GlobalsV::StaticBackground = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"StaticBackground"_wref.get()));
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
            GlobalsV::SputnikShow = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SputnikShow"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"SpaceImage"_wref.get()) > 0) {
            GlobalsV::SpaceImage = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"SpaceImage"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ShowFPS"_wref.get()) > 0) {
            GR_Main::ShowFrameRate = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ShowFPS"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontGalaxy"_wref.get()) > 0) {
            GlobalsV::GalaxyMapFontChoice = static_cast<GlobalsV::TGalaxyMapFontChoice>(EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontGalaxy"_wref.get())));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontDialog"_wref.get()) > 0) {
            GlobalsV::FontDialog = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontDialog"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontQuest"_wref.get()) > 0) {
            GlobalsV::FontQuest = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontQuest"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"FontSmooth"_wref.get()) > 0) {
            GlobalsV::FontSmoothingEnabled = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"FontSmooth"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScreenShotType"_wref.get()) > 0) {
            GlobalsV::ScreenshotFormat = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScreenShotType"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"ScreenShotQuality"_wref.get()) > 0) {
            GlobalsV::ScreenshotJpegQuality = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"ScreenShotQuality"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"DynamicTipsPos"_wref.get()) > 0) {
            GlobalsV::DynamicTipsPos = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"DynamicTipsPos"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundShade"_wref.get()) > 0) {
            GlobalsV::BackgroundShade = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundShade"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundBlur"_wref.get()) > 0) {
            GlobalsV::BackgroundBlur = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundBlur"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BackgroundGrayscale"_wref.get()) > 0) {
            GlobalsV::BackgroundGrayscale = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BackgroundGrayscale"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PlanetClouds"_wref.get()) > 0) {
            GlobalsV::PlanetClouds = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PlanetClouds"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"PlanetAtm"_wref.get()) > 0) {
            GlobalsV::PlanetAtm = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"PlanetAtm"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimChangeForm"_wref.get()) > 0) {
            GlobalsV::AnimChangeForm = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimChangeForm"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"AnimMainFon"_wref.get()) > 0) {
            GlobalsV::AnimMainFon = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"AnimMainFon"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"BeginCalcNextTurn"_wref.get()) > 0) {
            GlobalsV::BeginCalcNextTurn = pas::real_divide(EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"BeginCalcNextTurn"_wref.get())), 1.0E+2L);
        }
        if (GlobalsV::BeginCalcNextTurn < 0.0L) {
            GlobalsV::BeginCalcNextTurn = 0.0f;
        } else if (GlobalsV::BeginCalcNextTurn > 1.0L) {
            GlobalsV::BeginCalcNextTurn = 1.0f;
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"HalfGovAnim"_wref.get()) > 0) {
            GlobalsV::HalfGovAnim = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"HalfGovAnim"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"UseTablesForGov"_wref.get()) > 0) {
            GlobalsV::UseTablesForGov = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"UseTablesForGov"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotShowStencilShadows"_wref.get()) > 0) {
            Robot::RobotSettings.ShowStencilShadows = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotShowStencilShadows"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotShowProjShadows"_wref.get()) > 0) {
            Robot::RobotSettings.ShowProjShadows = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotShowProjShadows"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSelectEx"_wref.get()) > 0) {
            Robot::RobotSettings.SelectEx = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSelectEx"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotLandTexturesGloss"_wref.get()) > 0) {
            Robot::RobotSettings.LandTexturesGloss = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotLandTexturesGloss"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotObjTexturesGloss"_wref.get()) > 0) {
            Robot::RobotSettings.ObjTexturesGloss = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotObjTexturesGloss"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSoftwareCursor"_wref.get()) > 0) {
            Robot::RobotSettings.SoftwareCursor = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSoftwareCursor"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSky"_wref.get()) > 0) {
            Robot::RobotSettings.Sky = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSky"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotRobotShadow"_wref.get()) > 0) {
            Robot::RobotSettings.RobotShadow = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotRobotShadow"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotSound"_wref.get()) > 0) {
            Robot::RobotSound = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotSound"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotMusic"_wref.get()) > 0) {
            Robot::RobotMusic = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotMusic"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotVSync"_wref.get()) > 0) {
            Robot::RobotVSync = GI_Main::ParseEnabledNameGI(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotVSync"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotFSAASamples"_wref.get()) > 0) {
            Robot::RobotFSAASamples = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotFSAASamples"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotAnisotropy"_wref.get()) > 0) {
            Robot::RobotAnisotropy = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotAnisotropy"_wref.get()));
        }
        if (GR_Main::UserSettingsConfig->CountParamsByPath(u"RobotMaxDistance"_wref.get()) > 0) {
            Robot::RobotMaxDistance = EC_Str::ExtractDigitsToIntW(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"RobotMaxDistance"_wref.get()));
        }
        if (ReloadScriptTemplates) {
            Section = GR_Main::GameDataConfig->GetBlockByPath(u"Script"_wref.get());
            Count = Section->GetParamCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                if (Globals::FindScriptTemplateIndex(Section->GetParamName(Index)) >= 0) {
                    pas::raise(pas::make_exception<pas::Exception>("Script name not unique"_a));
                }
                ScriptTemplate = pas::construct_call<TScriptTemplUnit>(TScriptTemplUnit_Create);
                ScriptTemplate->Name = Section->GetParamName(Index);
                Text = Section->GetParamValue(Index);
                ScriptTemplate->ConfigValue = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                ScriptTemplate->FileName = EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get());
                pas::list_add(ScriptTemplates, reinterpret_cast<void*>(ScriptTemplate));
                aScript::CompileScriptTemplateCondition(pas::list_count(ScriptTemplates) - 1);
            }
            for (auto cpp_range_2 = pas::for_downto<std::int32_t>(GlobalScriptVariables->Count - 2, 0); cpp_range_2.next(Index); ) {
                Variable = EC_Expression::TVarArrayEC_GetItemByNameOrder(GlobalScriptVariables, Index);
                Other = EC_Expression::TVarArrayEC_GetItemByNameOrder(GlobalScriptVariables, Index + 1);
                if (Variable->Name == Other->Name) {
                    if (Variable->RealVType() != Other->RealVType()) {
                        WarningText = pas::concat_wide({u"Warning! Mismatching global variables with same name <", Variable->Name, u"> found! Types are ", ScriptVariableTypeNames[static_cast<std::int32_t>(Variable->RealVType()) & 0x0000007f], u" and ", ScriptVariableTypeNames[static_cast<std::int32_t>(Other->RealVType()) & 0x0000007f]});
                        if (Variable->RealVType() == EC_Expression::vkEmpty) {
                            WarningText = pas::concat_wide({WarningText, u", ", ScriptVariableTypeNames[static_cast<std::int32_t>(Variable->RealVType()) & 0x0000007f], u" will be discarded"});
                            GlobalScriptVariables->Remove(Variable);
                        } else {
                            WarningText = pas::concat_wide({WarningText, u", ", ScriptVariableTypeNames[static_cast<std::int32_t>(Other->RealVType()) & 0x0000007f], u" will be discarded"});
                            GlobalScriptVariables->Remove(Other);
                        }
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(WarningText));
                    } else if (Variable->EqualsValue(Other) || static_cast<std::uint8_t>(pas::in_set<1, 4>(Variable->RealVType()) ^ 1)) {
                        GlobalScriptVariables->Remove(Other);
                    } else {
                        WarningText = pas::concat_wide({u"Warning! Mismatching global variables with same name <", Variable->Name, u"> found! Initial values are ", Variable->GetString(), u" and ", Other->GetString(), u". Value ", Variable->GetString(), u" will be used"});
                        GlobalScriptVariables->Remove(Other);
                    }
                }
            }
        }
        if (ReloadScriptTemplates) {
            for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_3.next(Race); ) {
                ShipBlock = ([&] {
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Ship"_wref.get());
                    const pas::WideString& internalName = aConst::OwnerInfo[Race].InternalName;
                    return blockByPath->FindBlock(internalName);
                }());
                for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(5); ++Kind) {
                    RaceShipTemplates[Race][Kind] = nullptr;
                }
                PirateClanShipTemplates[Race] = nullptr;
                if (ShipBlock != nullptr) {
                    if (ShipBlock->CountBlocks(u"Ranger"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Ranger"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][0]);
                        SE_Space::RetainSpaceObject(cpp_arg, createSpaceObjectByName);
                    }
                    if (ShipBlock->CountBlocks(u"Warrior"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Warrior"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_2 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][1]);
                        SE_Space::RetainSpaceObject(cpp_arg_2, createSpaceObjectByName_2);
                    }
                    if (ShipBlock->CountBlocks(u"Pirate"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_3 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Pirate"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_3 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][2]);
                        SE_Space::RetainSpaceObject(cpp_arg_3, createSpaceObjectByName_3);
                    }
                    if (ShipBlock->CountBlocks(u"Transport"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_4 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Transport"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_4 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][3]);
                        SE_Space::RetainSpaceObject(cpp_arg_4, createSpaceObjectByName_4);
                    }
                    if (ShipBlock->CountBlocks(u"Liner"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_5 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Liner"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_5 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][4]);
                        SE_Space::RetainSpaceObject(cpp_arg_5, createSpaceObjectByName_5);
                    }
                    if (ShipBlock->CountBlocks(u"Diplomat"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_6 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".Diplomat"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_6 = pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][5]);
                        SE_Space::RetainSpaceObject(cpp_arg_6, createSpaceObjectByName_6);
                    }
                    if (ShipBlock->CountBlocks(u"PirateClan"_wref.get()) > 0) {
                        SE_Space::TObjectSE* createSpaceObjectByName_7 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), pas::concat_wide({u"Ship.", aConst::OwnerInfo[Race].InternalName, u".PirateClan"}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_7 = pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]);
                        SE_Space::RetainSpaceObject(cpp_arg_7, createSpaceObjectByName_7);
                    }
                }
            }
            Index = 1;
            for (auto cpp_range_4 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_4.next(Series); ) {
                if (Series != 0) {
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_8 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), static_cast<pas::WideString>(pas::concat_ansi({"Ship.Blazer.B", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_8 = pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[Series]);
                        SE_Space::RetainSpaceObject(cpp_arg_8, createSpaceObjectByName_8);
                    }
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_9 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), static_cast<pas::WideString>(pas::concat_ansi({"Ship.Keller.K", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_9 = pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[Series]);
                        SE_Space::RetainSpaceObject(cpp_arg_9, createSpaceObjectByName_9);
                    }
                    {
                        SE_Space::TObjectSE* createSpaceObjectByName_10 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), static_cast<pas::WideString>(pas::concat_ansi({"Ship.Terron.T", SysUtils::IntToStr(Index)})), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> cpp_arg_10 = pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[Series]);
                        SE_Space::RetainSpaceObject(cpp_arg_10, createSpaceObjectByName_10);
                    }
                    ++Index;
                }
            }
        }
        Count = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get())->GetBlockCount();
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(Index); ) {
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
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(Index); ) {
            Section = ([&] {
                EC_BlockPar::TBlockParEC* blockByPath_3 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get());
                std::int32_t index_2 = Index;
                return blockByPath_3->GetBlockByIndex(index_2);
            }());
            if (Section->CountParams(u"Image"_wref.get()) > 0) {
                {
                    SE_Space::TObjectSE* createSpaceObjectByName_11 = SE_Process::CreateSpaceObjectByName(u"Planet"_wref.get(), pas::concat_wide({u"Planet.", ([&] {
                        EC_BlockPar::TBlockParEC* blockByPath_4 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Planet"_wref.get());
                        std::int32_t index_3 = Index;
                        return blockByPath_4->GetBlockNameByIndex(index_3);
                    }())}), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> spaceObject = pas::Var<SE_Space::TObjectSE*>(&PlanetSpaceTemplates[TemplateIndex].SpaceObject);
                    SE_Space::RetainSpaceObject(spaceObject, createSpaceObjectByName_11);
                }
                PlanetSpaceTemplates[TemplateIndex].Radius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Section->GetParam(u"Radius"_wref.get())));
                PlanetSpaceTemplates[TemplateIndex].Style = 0;
                PlanetSpaceTemplates[TemplateIndex].StyleVariant = 0;
                if (Section->CountParams(u"Style"_wref.get()) > 0) {
                    Text = Section->GetParam(u"Style"_wref.get());
                    PlanetSpaceTemplates[TemplateIndex].Style = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                    if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) >= 2) {
                        PlanetSpaceTemplates[TemplateIndex].StyleVariant = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
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
        Section = GR_Main::GameDataConfig->GetBlock(u"SpaceImg"_wref.get());
        Count = Section->GetParamCount();
        GlobalsV::SpaceImageTemplates.set_length(Count);
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_7.next(Index); ) {
            Text = Section->GetParamValue(Index);
            if (EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) < 2) {
                pas::raise(pas::make_exception<pas::Exception>("Error in GlobalsInit"_a));
            }
            GlobalsV::SpaceImageTemplates[Index].Kind = EC_Str::ExtractDigitsToIntW(Section->GetParamName(Index));
            GlobalsV::SpaceImageTemplates[Index].Weight = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
            GlobalsV::SpaceImageTemplates[Index].CacheControl = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
            GlobalsV::SpaceImageTemplates[Index].CachedData = nullptr;
            EC_Cache::TCacheEC::ResetControl(reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl));
            reinterpret_cast<EC_CacheGAI::TCGaiControlEC*>(GlobalsV::SpaceImageTemplates[Index].CacheControl)->SetCacheKey(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"StarFieldImg"_wref.get());
        Count = Section->GetParamCount();
        GlobalsV::StarFieldImageTemplates.set_length(Count);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_8.next(Index); ) {
            GlobalsV::StarFieldImageTemplates[Index].Weight = EC_Str::ExtractDigitsToIntW(Section->GetParamName(Index));
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
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"_wref.get())->GetBlock(u"Explosion"_wref.get());
        Count = Section->GetParamCount();
        ArcadeExplosionSounds.set_length(Count);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(Index); ) {
            ArcadeExplosionSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"_wref.get())->GetBlock(u"Hit"_wref.get());
        Count = Section->GetParamCount();
        ArcadeHitSounds.set_length(Count);
        for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_10.next(Index); ) {
            ArcadeHitSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"_wref.get())->GetBlock(u"Item"_wref.get());
        Count = Section->GetParamCount();
        ArcadeItemSounds.set_length(Count);
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_11.next(Index); ) {
            ArcadeItemSounds[Index] = Section->GetParamValue(Index);
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"_wref.get())->GetBlock(u"WeaponFirst"_wref.get());
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, 17); cpp_range_12.next(Index); ) {
            if (Section->CountParams(pas::wide_int_to_str(Index)) <= 0) {
                ArcadeWeaponFirstSounds[Index] = pas::WideString();
            } else {
                ArcadeWeaponFirstSounds[Index] = Section->GetParam(pas::wide_int_to_str(Index));
            }
        }
        Section = GR_Main::GameDataConfig->GetBlock(u"ABSound"_wref.get())->GetBlock(u"WeaponLoop"_wref.get());
        for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, 17); cpp_range_13.next(Index); ) {
            if (Section->CountParams(pas::wide_int_to_str(Index)) <= 0) {
                ArcadeWeaponLoopSounds[Index] = pas::WideString();
                ArcadeWeaponLoopTicks[Index] = -1;
            } else {
                Text = Section->GetParam(pas::wide_int_to_str(Index));
                ArcadeWeaponLoopTicks[Index] = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get())) / 20;
                ArcadeWeaponLoopSounds[Index] = EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get());
            }
        }
    }

    void FinalizeGlobalUiRuntime() {
        std::int32_t Index{};
        TSputnikTempl* SatelliteTemplate{};
        TPlanetTempl* PlanetTemplate{};
        std::uint8_t ScreenIndex{};
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
        for (ScreenIndex = static_cast<std::uint8_t>(0); ScreenIndex <= static_cast<std::uint8_t>(41); ++ScreenIndex) {
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
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count); cpp_range_6.next(Index); ) {
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
        std::uint8_t Race{};
        std::uint8_t Kind{};
        std::uint8_t Series{};
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
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range.next(Race); ) {
            for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(5)); cpp_range_2.next(Kind); ) {
                if (RaceShipTemplates[Race][Kind] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&RaceShipTemplates[Race][Kind]));
                }
            }
            if (PirateClanShipTemplates[Race] != nullptr) {
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PirateClanShipTemplates[Race]));
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range_3.next(Series); ) {
            if (Series != 0) {
                if (BlazerShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&BlazerShipTemplates[Series]));
                }
                if (KellerShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&KellerShipTemplates[Series]));
                }
                if (TerronShipTemplates[Series] != nullptr) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&TerronShipTemplates[Series]));
                }
            }
        }
        ReloadScriptTemplates = true;
    }

    GI_MessageLoop::TMessageLoopGI* FindMessageLoop(pas::WideString Name) {
        std::uint8_t Index{};
        GI_MessageLoop::TMessageLoopGI* Result = nullptr;
        for (Index = static_cast<std::uint8_t>(0); Index <= static_cast<std::uint8_t>(41); ++Index) {
            if (reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]) != nullptr && pas::class_cast_if<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]) != nullptr && pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index])->RegisteredLoopName == Name) {
                Result = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[Index]);
                break;
            }
        }
        return Result;
    }

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

    void SwapTurnFilms() {
        aEFilm::TEFilm* Film = PrimaryFilm;
        PrimaryFilm = SecondaryFilm;
        SecondaryFilm = Film;
    }

    GI_MessageLoop::TMessageLoopGI* GetInnermostScreenLoop() {
        GI_MessageLoop::TMessageLoopGI* Result = reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
        while (Result->ChildLoop != nullptr) {
            Result = Result->ChildLoop;
        }
        return Result;
    }

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
                    if (EC_Str::IsIntegerTextW(Suffix) && EC_Str::ExtractDigitsToIntW(Suffix) >= FirstPage) {
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

    TMessagePlayer* AddOrUpdatePlayerBubble(std::uint8_t Kind, std::int32_t Turn, const pas::WideString& Text, const pas::WideString& Key) {
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
                if (Entry->WasRead && pas::in_range(Entry->Kind, 6, 6) && aGalaxy::Galaxy->CurrentTurn - Entry->Turn >= 7) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (aGalaxy::Galaxy->CurrentTurn - Entry->Turn >= PlayerMessagePresentations[Entry->Kind].LifetimeTurns) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (Entry->WasRead && pas::in_set<0, 2, 4, 5, 8, 8>(Entry->Kind)) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                } else if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1) && Entry->Kind == 1) {
                    Globals::RemovePersistentPlayerMessage(Entry, true);
                }
            }
        }
    }

    void TMessagePlayer_Create(TMessagePlayer* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Button = nullptr;
        Self->ImageNameOverride = pas::WideString();
    }

    void TMessagePlayer::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddWideStringZ(Key);
        Buffer->AddAnsiChar(Kind);
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

    void TMessagePlayer::LoadFromBuffer(EC_Buf::TBufEC* Buffer) {
        Key = Buffer->ReadWideString();
        Kind = EC_Buf::TBufEC_GetByte(Buffer);
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

    pas::WideString ReadMapText(const pas::WideString& Path, EC_BlockPar::TBlockParEC*& Block) {
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
    }

    pas::WideString ReadShipGreetingField(const pas::WideString& FieldName, EC_BlockPar::TBlockParEC*& Block) {
        if (Block->CountParams(FieldName) > 0) {
            return Block->GetParam(FieldName);
        }
        return pas::WideString();
    }

    pas::WideString ReadGovernmentGreetingField(pas::WideString FieldName, EC_BlockPar::TBlockParEC*& Block) {
        if (Block->CountParams(FieldName) > 0) {
            return Block->GetParam(FieldName);
        }
        return pas::WideString();
    }

    void TScriptTemplUnit::p_destroy() {
        Globals::TScriptTemplUnit_Destroy(this);
    }

} // namespace Globals
