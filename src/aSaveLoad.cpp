#include "layout/aSaveLoad.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/WindowsImports.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aShip.hpp"
#include "types/ab_MainForm.hpp"
#include "types/fChameleon.hpp"
#include "types/fCount2.hpp"
#include "types/fFilmFile.hpp"
#include "types/fGalaxy2.hpp"
#include "types/fGameLoad.hpp"
#include "types/fGameMenu.hpp"
#include "types/fGoodsShop2.hpp"
#include "types/fGov.hpp"
#include "types/fHangar.hpp"
#include "types/fInfo.hpp"
#include "types/fJournal.hpp"
#include "types/fJump.hpp"
#include "types/fLoad.hpp"
#include "types/fPlanet.hpp"
#include "types/fPlanetNO.hpp"
#include "types/fPlanetQuest.hpp"
#include "types/fRating2.hpp"
#include "types/fRewards.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fSaveManager.hpp"
#include "types/fScaner.hpp"
#include "types/fSelectFace.hpp"
#include "types/fStarMap.hpp"
#include "types/fTalk.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fShip2.hpp"

namespace aSaveLoad {
    EC_Buf::TBufEC* MemorySnapshotBuffer = nullptr;

    pas::CriticalSection* SaveLoadLock = nullptr;

    aSaveLoad::TSaver* SaveWriter = nullptr;

    std::int32_t MemorySnapshotXorSeed{};

    // Detached galaxy address plus 0x17557455, modulo 2^32.
    std::uint32_t MemorySnapshotGalaxyToken{};

    // Disk header: eight NUL-terminated UTF-16 strings: RSG, v<version>,
    // description, turn, money, pilot name, race/emblem name, EZ.
    // Each preview has a four-byte byte count. The trailing film block runs to EOF.
    // Queues the write; true does not mean the background writer has finished.
    std::uint8_t SaveGameToFile(pas::WideString FileName, pas::WideString Description) {
        EC_Buf::TBufEC* Header{};
        EC_Buf::TBufEC* Preview{};
        EC_Buf::TBufEC* SecondaryPreview{};
        EC_Buf::TBufEC* GameState{};
        EC_Buf::TBufEC* Films{};
        EC_Buf::TBufEC* FilmEntry{};
        std::int32_t Size{};
        std::int32_t I{};
        std::int32_t Count{};
        Globals::TMessagePlayer* Message{};
        fShip2::TPlayerHoldUnit* Entry{};
        pas::WideString AutoName{};
        pas::WideString TurnName{};
        pas::WideString QuickName{};
        std::uint8_t Result = false;
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr) {
            FilmEntry = nullptr;
            AutoName = fSaveManager::TfSaveManager::GetAutoSavePath();
            TurnName = fSaveManager::TfSaveManager::GetTurnSavePath();
            QuickName = fSaveManager::TfSaveManager::GetQuickSavePath(1);
            if (FileName == AutoName) {
                Description = fSaveManager::TfSaveManager::BuildCurrentSaveDescription();
            }
            try {
                Header = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Header->AddWideStringZ(u"RSG"_wref.get());
                Header->AddWideStringZ(static_cast<pas::WideString>(pas::concat_ansi({"v", SysUtils::IntToStr(aConst::CurrentSaveVersion)})));
                Header->AddWideStringZ(Description);
                Header->AddWideStringZ(pas::wide_int_to_str(aGalaxy::Galaxy->CurrentTurn));
                Header->AddWideStringZ(pas::wide_int_to_str(aPlayer::GetPlayer()->Money));
                Header->AddWideStringZ(aPlayer::GetPlayer()->Name);
                if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                    Header->AddWideStringZ(pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)].InternalName}));
                } else {
                    Header->AddWideStringZ(aConst::OwnerInfo[aPlayer::GetPlayer()->OwnerId].InternalName);
                }
                Header->AddWideStringZ(u"EZ"_wref.get());
                Preview = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                if (GR_Main::SavePreviewGraph != nullptr) {
                    GR_Main::SavePreviewGraph->SaveToBuffer(Preview);
                }
                SecondaryPreview = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                if (GR_Main::SecondarySavePreviewGraph != nullptr) {
                    GR_Main::SecondarySavePreviewGraph->SaveToBuffer(SecondaryPreview);
                }
                GameState = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                GameState->AddIntegerValue(GlobalsV::SaveManagerReturnScreenId);
                GameState->AddIntegerValue(Globals::StarMapScreen->GetMapCenter().X);
                GameState->AddIntegerValue(Globals::StarMapScreen->GetMapCenter().Y);
                GameState->AddBoolean(Globals::StarMapWeaponPanelOpen);
                GameState->AddAnsiChar('\000');
                GameState->AddBoolean(Globals::FilmCameraFollow);
                GameState->AddAnsiChar('\000');
                GameState->AddAnsiChar('\000');
                GameState->AddAnsiChar('\000');
                GameState->AddBoolean(false);
                GameState->AddBoolean(Globals::PlayerStarDayPrepared);
                GameState->AddDWord(Globals::ShownPlayerTips);
                GameState->AddIntegerValue(0);
                Count = Globals::CountPersistentPlayerMessages();
                GameState->AddIntegerValue(Count);
                Message = Globals::FirstPersistentPlayerMessage;
                while (Message != nullptr) {
                    Message->SaveToBuffer(GameState);
                    Message = Message->Next;
                }
                fShip2::PlayerHoldShip = aPlayer::GetPlayer();
                fShip2::RefreshPlayerHoldView(false);
                Count = pas::list_count(fShip2::PlayerHoldEntries);
                GameState->AddWideChar(Count);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                    Entry = pas::list_at<fShip2::TPlayerHoldUnit>(fShip2::PlayerHoldEntries, I);
                    GameState->AddAnsiChar(static_cast<std::uint8_t>(Entry->Kind));
                    GameState->AddAnsiChar(Entry->GoodsIndex);
                    GameState->AddDWord(Entry->ItemId);
                }
                aGalaxy::Galaxy->SaveToBuffer(GameState);
                Films = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                FilmEntry = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                Count = 0;
                Count = Globals::FilmHistory->GetCount();
                Films->AddBytes(&Count, 4);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                    {
                        fFilmFile::PFilmHistoryEntry entry = Globals::FilmHistory->GetEntry(I);
                        fFilmFile::TFilmFile* filmHistory = Globals::FilmHistory;
                        filmHistory->SaveEntryToBuffer(entry, FilmEntry);
                    }
                    Size = FilmEntry->DataSize;
                    Films->AddBytes(&Size, 4);
                    if (Size > 0) {
                        Films->AddBytes(FilmEntry->Data, Size);
                    }
                }
                SaveWriter->QueueSave(FileName, Header, Preview, SecondaryPreview, GameState, Films);
                if (aGalaxy::Galaxy->CampaignFlag183 != 0) {
                    GR_Main::EditableSaveFileName = fSaveManager::TfSaveManager::GetSaveConfigPath(FileName);
                    if (GR_Main::UserSettingsConfig->CountParams(u"UnicodeDump"_wref.get()) > 0 && GI_Main::ParseEnabledNameGI(pas::view(EC_Str::TrimWideString(GR_Main::UserSettingsConfig->GetParamByPathOrMarker(u"UnicodeDump"_wref.get()))))) {
                        GR_Main::EditableSaveBlock->SaveTextFile(GR_Main::EditableSaveFileName.pchar(), false, false);
                    } else {
                        GR_Main::EditableSaveBlock->SaveTextFile(GR_Main::EditableSaveFileName.pchar(), true, false);
                    }
                    GR_Main::EditableSaveBlock->Clear();
                    aGalaxy::Galaxy->CampaignFlag183 = 0;
                }
                Result = true;
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(E->message);
                    if (FileName == AutoName) {
                        aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.AutoSaveFailed"_w);
                    }
                    if (FileName == TurnName) {
                        aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.TurnSaveFailed"_w);
                    }
                    if (FileName == QuickName) {
                        aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.QuickSaveFailed"_w);
                    }
                } else {
                    throw;
                }
            }
            if (FilmEntry != nullptr) {
                pas::free(FilmEntry);
            }
            if (Result == true) {
                GR_Main::FreeSavePreviewBuffers();
            }
        }
        return Result;
    }

    // Replaces the current galaxy.
    std::uint8_t LoadGameFromFile(pas::WideString FileName) {
        EC_File::TFileEC* F{};
        EC_Buf::TBufEC* Films{};
        Types::TPoint Center{};
        std::int32_t Size{};
        std::int32_t I{};
        std::int32_t Count{};
        std::int32_t Seed{};
        std::uint32_t Crc{};
        Globals::TMessagePlayer* Message{};
        std::uint8_t Result = false;
        GR_Main::LoadedSaveModSet = GR_Main::SelectedMods;
        pas::critical_enter(SaveLoadLock);
        F = nullptr;
        EC_Buf::TBufEC* Buffer = nullptr;
        try {
            if (MemorySnapshotBuffer != nullptr) {
                pas::free(MemorySnapshotBuffer);
            }
            MemorySnapshotBuffer = nullptr;
            GlobalsV::MemorySnapshotActive = false;
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
                pas::free(aGalaxy::Galaxy);
            }
            aGalaxy::Galaxy = nullptr;
            F = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
            F->SetFileName(FileName);
            if (!F->TryAcquireReadHandle(false)) {
                pas::raise(pas::make_exception<pas::Abort>(static_cast<pas::AnsiString>(pas::concat_wide({u"Cannot open file ", FileName}))));
            }
            if (F->ReadWideString() != u"RSG") {
                pas::raise(pas::make_exception<pas::Abort>(static_cast<pas::AnsiString>(pas::concat_wide({u"Bad pre-signature of file", FileName}))));
            }
            GlobalsV::LoadedSaveVersion = EC_Str::ExtractDigitsToIntW(pas::view(F->ReadWideString()));
            F->ReadWideString();
            SysUtils::StrToInt(static_cast<pas::AnsiString>(F->ReadWideString()));
            SysUtils::StrToInt(static_cast<pas::AnsiString>(F->ReadWideString()));
            F->ReadWideString();
            F->ReadWideString();
            if (F->ReadWideString() != u"EZ") {
                pas::raise(pas::make_exception<pas::Abort>(static_cast<pas::AnsiString>(pas::concat_wide({u"Bad post-signature of file", FileName}))));
            }
            Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            F->ReadBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                F->SetPointer(Size, WindowsImports::FILE_CURRENT);
            }
            F->ReadBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                F->SetPointer(Size, WindowsImports::FILE_CURRENT);
            }
            F->ReadBuffer(&Crc, static_cast<std::int32_t>(sizeof(std::uint32_t)));
            F->ReadBuffer(&Seed, static_cast<std::int32_t>(sizeof(std::int32_t)));
            F->ReadBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                Buffer->SetSize(Size);
                try {
                    F->ReadBuffer(Buffer->Data, Size);
                } catch (...) {
                    GI_MessageBox::ShowMessageBoxGI(nullptr, u"Compressed galaxy read fail"_wref.get(), GI_MessageBox::mbgOK, 0, 0, 0);
                }
            }
            Buffer->ApplyDatXorCipher(Seed);
            if (Buffer->ComputeCrc32() != Crc) {
                pas::raise(pas::make_exception<pas::Abort>("Integrity check fail"_a));
            }
            Buffer->ExpandZlibPayloadInPlace();
            Globals::ActiveLoadBuffer = Buffer;
            GlobalsV::RequestedScreenId = static_cast<GlobalsV::TGameScreenId>(EC_Buf::TBufEC_GetInt32(Buffer));
            aGalaxy::Galaxy = pas::construct_call<aGalaxy::TGalaxy>(aGalaxy::TGalaxy_Create);
            Center.X = EC_Buf::TBufEC_GetInt32(Buffer);
            Center.Y = EC_Buf::TBufEC_GetInt32(Buffer);
            Globals::SpaceViewPosition = EC_Struct::PointToPointF(Center);
            Globals::StarMapScreen->SetMapCenterManually(Center);
            Globals::StarMapWeaponPanelOpen = EC_Buf::TBufEC_GetBoolean(Buffer);
            EC_Buf::TBufEC_GetByte(Buffer);
            Globals::FilmCameraFollow = EC_Buf::TBufEC_GetBoolean(Buffer);
            EC_Buf::TBufEC_GetByte(Buffer);
            EC_Buf::TBufEC_GetByte(Buffer);
            EC_Buf::TBufEC_GetByte(Buffer);
            EC_Buf::TBufEC_GetBoolean(Buffer);
            Globals::PlayerStarDayPrepared = EC_Buf::TBufEC_GetBoolean(Buffer);
            static_cast<void>(Globals::PlayerStarDayPrepared);
            Globals::ShownPlayerTips = EC_Buf::TBufEC_GetUInt32(Buffer);
            EC_Buf::TBufEC_GetInt32(Buffer);
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                Message = Globals::CreatePersistentPlayerMessage();
                Message->LoadFromBuffer(Buffer);
            }
            aSaveLoad::LoadGameFromSaveBuffer(Buffer);
            Films = nullptr;
            {
                std::exception_ptr cpp_error{};
                try {
                    Films = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
                    Films->Clear();
                    {
                        std::uint32_t cpp_left = F->GetSize();
                        Size = cpp_left - F->GetPointer();
                    }
                    if (Size > 0) {
                        Films->SetSize(Size);
                        F->ReadBuffer(Films->Data, Size);
                        Films->ExpandZlibPayloadInPlace();
                        Globals::FilmHistory->Clear();
                        Films->ReadBytes(&Count, 4);
                        GlobalsV::LoadingFilmCount = Count;
                        GlobalsV::LoadedFilmCount = 0;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                            Films->ReadBytes(&Size, 4);
                            if (Size > 0) {
                                Buffer->SetSize(Size);
                                Films->ReadBytes(Buffer->Data, Size);
                                Buffer->SetPosition(0);
                                Globals::FilmHistory->LoadEntryFromBuffer(Buffer);
                            }
                            ++GlobalsV::LoadedFilmCount;
                        }
                        GlobalsV::LoadingFilmCount = -1;
                    }
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                if (Films != nullptr) {
                    pas::free(Films);
                }
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
            }
            GlobalsV::LoadingFilmCount = -1;
            Globals::PreviousFilmActivity = 0u;
            Result = true;
        } catch (...) {
            if (MemorySnapshotBuffer != nullptr) {
                pas::free(MemorySnapshotBuffer);
            }
            MemorySnapshotBuffer = nullptr;
            GlobalsV::MemorySnapshotActive = false;
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
                pas::ScopeExit cpp_cleanup_2 = [&]() noexcept {
                    aGalaxy::Galaxy = nullptr;
                };
                pas::free(aGalaxy::Galaxy);
            }
            if (GR_Main::EditableSaveBlock != nullptr) {
                GR_Main::EditableSaveBlock->Clear();
            }
        }
        GlobalsV::LoadingFilmCount = -1;
        Globals::ActiveLoadBuffer = nullptr;
        if (Buffer != nullptr) {
            pas::free(Buffer);
        }
        if (F != nullptr) {
            pas::free(F);
        }
        pas::critical_leave(SaveLoadLock);
        return Result;
    }

    // Requires an existing galaxy object and a decoded buffer positioned at the player-hold section.
    void LoadGameFromSaveBuffer(EC_Buf::TBufEC* Buffer) {
        std::int32_t I{};
        fShip2::TPlayerHoldUnit* Entry{};
        fShip2::InitializePlayerHoldView();
        std::int32_t Count = EC_Buf::TBufEC_GetWord(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Entry = pas::make_object<fShip2::TPlayerHoldUnit>();
            pas::list_add(fShip2::PlayerHoldEntries, reinterpret_cast<void*>(Entry));
            Entry->Kind = static_cast<fShip2::TPlayerHoldKind>(EC_Buf::TBufEC_GetByte(Buffer));
            Entry->GoodsIndex = EC_Buf::TBufEC_GetByte(Buffer);
            Entry->ItemId = EC_Buf::TBufEC_GetUInt32(Buffer);
        }
        aGalaxy::TGalaxy* LoadingGalaxy = aGalaxy::Galaxy;
        aGalaxy::Galaxy = nullptr;
        aGalaxy::TGalaxy_LoadFromBuffer(LoadingGalaxy, Buffer);
        if (GR_Main::ApplyEditableSaveOnLoad) {
            aGalaxy::Galaxy->ApplyEditableState();
            GR_Main::ApplyEditableSaveOnLoad = false;
            aGalaxy::Galaxy->PrimeIntegrityChecksum(101);
        }
        aGalaxy::TGalaxy::RunConfigOnLoadHandlers();
    }

    // Requires a live galaxy/player and no outstanding snapshot. Obfuscates and detaches the live galaxy until restoration; does not increment the persistent save count.
    void SaveGameToMemorySnapshot() {
        std::int32_t I{};
        fShip2::TPlayerHoldUnit* Entry{};
        GI_MessageLoop::TMessageLoopGI* Loop{};
        MemorySnapshotBuffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        aGalaxy::TGalaxy::ClearIntegrityStatus();
        if (GlobalsV::CurrentScreenId != GlobalsV::screenPlanetQuest && (GlobalsV::CurrentScreenId != GlobalsV::screenGovernment || Globals::GovernmentScreen->PendingTransition == 0) && (GlobalsV::CurrentScreenId != GlobalsV::screenStarMap || Globals::StarMapScreen->PlanetBattleState == 0)) {
            pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->OnClose();
        }
        {
            const std::int32_t cpp_first = pas::list_count(GI_MessageLoop::MessageLoopStack) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Loop = pas::list_at<GI_MessageLoop::TMessageLoopGI>(GI_MessageLoop::MessageLoopStack, I);
                    if (fShip2::TfShip2* fShip2_2 = pas::class_cast_if<fShip2::TfShip2*>(Loop)) {
                        fShip2_2->ReturnSelectedHoldEntry();
                    }
                }
            }
        }
        aGalaxy::TGalaxy::ClearIntegrityStatus();
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited || aPlayer::GetPlayer()->IsDockedToShip() && pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr) {
            fEquipmentShop::RestoreTemporaryShopStock();
        }
        std::int32_t Count = Globals::CountPersistentPlayerMessages();
        MemorySnapshotBuffer->AddIntegerValue(Count);
        Globals::TMessagePlayer* Message = Globals::FirstPersistentPlayerMessage;
        while (Message != nullptr) {
            Message->SaveToBuffer(MemorySnapshotBuffer);
            Message = Message->Next;
        }
        MemorySnapshotBuffer->AddDWord(Globals::ShownPlayerTips);
        fShip2::PlayerHoldShip = aPlayer::GetPlayer();
        fShip2::RefreshPlayerHoldView(false);
        Count = pas::list_count(fShip2::PlayerHoldEntries);
        MemorySnapshotBuffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<fShip2::TPlayerHoldUnit>(fShip2::PlayerHoldEntries, I);
            MemorySnapshotBuffer->AddAnsiChar(static_cast<std::uint8_t>(Entry->Kind));
            MemorySnapshotBuffer->AddAnsiChar(Entry->GoodsIndex);
            MemorySnapshotBuffer->AddDWord(Entry->ItemId);
        }
        --aGalaxy::Galaxy->SaveCount;
        aGalaxy::Galaxy->SaveToBuffer(MemorySnapshotBuffer);
        if (aGalaxy::Galaxy->ContainsShipReference(Globals::TalkShip)) {
            MemorySnapshotBuffer->AddDWord(Globals::TalkShip->Id);
        } else {
            MemorySnapshotBuffer->AddDWord(0u);
        }
        if (aGalaxy::Galaxy->ContainsPlanetReference(Globals::TalkPlanet)) {
            MemorySnapshotBuffer->AddDWord(Globals::TalkPlanet->Id);
        } else {
            MemorySnapshotBuffer->AddDWord(0u);
        }
        if (aGalaxy::Galaxy->ContainsShipReference(Globals::ShipScreen->ShipToInspect)) {
            MemorySnapshotBuffer->AddDWord(Globals::ShipScreen->ShipToInspect->Id);
        } else {
            MemorySnapshotBuffer->AddDWord(0u);
        }
        MemorySnapshotXorSeed = aMyFunction::RandomIntRange(0, 2000000000);
        MemorySnapshotBuffer->ApplyDatXorCipher(MemorySnapshotXorSeed);
        aPlayer::GetPlayer()->SetMoney(0);
        aGalaxy::Galaxy->ObfuscateProtectedState();
        MemorySnapshotGalaxyToken = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(aGalaxy::Galaxy)) + 0x17557455;
        aGalaxy::Galaxy = nullptr;
        aKling::BlazerShip = nullptr;
        aKling::KellerShip = nullptr;
        aKling::TerronShip = nullptr;
        GlobalsV::MemorySnapshotActive = true;
    }

    // Consumes the snapshot, destroys the detached galaxy and rebuilds it. Preserves the persistent load count and restores UI references by object ID.
    void RestoreGameFromMemorySnapshot() {
        std::int32_t I{};
        fShip2::TPlayerHoldUnit* Entry{};
        Globals::TMessagePlayer* Message{};
        GI_MessageLoop::TMessageLoopGI* Loop{};
        std::uint8_t ReopenScreen = true;
        aGalaxy::Galaxy = reinterpret_cast<aGalaxy::TGalaxy*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(MemorySnapshotGalaxyToken - 0x17557455)));
        aGalaxy::Galaxy->RestoreProtectedState();
        if (aKling::DominatorSpawnPlanet != nullptr) {
            pas::free(aKling::DominatorSpawnPlanet);
            aKling::DominatorSpawnPlanet = nullptr;
        }
        pas::free(aGalaxy::Galaxy);
        MemorySnapshotBuffer->SetPosition(0);
        MemorySnapshotBuffer->ApplyDatXorCipher(MemorySnapshotXorSeed);
        GlobalsV::MemorySnapshotActive = true;
        aGalaxy::Galaxy = pas::construct_call<aGalaxy::TGalaxy>(aGalaxy::TGalaxy_Create);
        GlobalsV::LoadedSaveVersion = aConst::CurrentSaveVersion;
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(MemorySnapshotBuffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Message = Globals::CreatePersistentPlayerMessage();
            Message->LoadFromBuffer(MemorySnapshotBuffer);
        }
        Globals::ShownPlayerTips = EC_Buf::TBufEC_GetUInt32(MemorySnapshotBuffer);
        fShip2::InitializePlayerHoldView();
        Count = EC_Buf::TBufEC_GetWord(MemorySnapshotBuffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
            Entry = pas::make_object<fShip2::TPlayerHoldUnit>();
            pas::list_add(fShip2::PlayerHoldEntries, reinterpret_cast<void*>(Entry));
            Entry->Kind = static_cast<fShip2::TPlayerHoldKind>(EC_Buf::TBufEC_GetByte(MemorySnapshotBuffer));
            Entry->GoodsIndex = EC_Buf::TBufEC_GetByte(MemorySnapshotBuffer);
            Entry->ItemId = EC_Buf::TBufEC_GetUInt32(MemorySnapshotBuffer);
        }
        aGalaxy::TGalaxy_LoadFromBuffer(aGalaxy::Galaxy, MemorySnapshotBuffer);
        --aGalaxy::Galaxy->LoadCount;
        {
            std::uint32_t uInt32 = EC_Buf::TBufEC_GetUInt32(MemorySnapshotBuffer);
            aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
            Globals::TalkShip = static_cast<aShip::TShip*>(galaxy->IdToShip(uInt32, true));
        }
        {
            std::uint32_t uInt32_2 = EC_Buf::TBufEC_GetUInt32(MemorySnapshotBuffer);
            aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
            Globals::TalkPlanet = static_cast<aPlanet::TPlanet*>(galaxy_2->IdToPlanet(uInt32_2, true));
        }
        {
            std::uint32_t uInt32_3 = EC_Buf::TBufEC_GetUInt32(MemorySnapshotBuffer);
            aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
            Globals::ShipScreen->ShipToInspect = static_cast<aShip::TShip*>(galaxy_3->IdToShip(uInt32_3, true));
        }
        {
            const std::int32_t cpp_first = pas::list_count(GI_MessageLoop::MessageLoopStack) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Loop = pas::list_at<GI_MessageLoop::TMessageLoopGI>(GI_MessageLoop::MessageLoopStack, I);
                    if (pas::class_cast_if<GI_MessageBox::TMessageBoxGI*>(Loop) != nullptr) {
                        Loop->RequestClose(254);
                    } else if (pas::class_cast_if<fCount2::TfCount2*>(Loop) != nullptr) {
                        Loop->RequestClose(254);
                    } else if (pas::class_cast_if<fStarMap::TfStarMap*>(Loop) != nullptr && Globals::StarMapScreen->PlanetBattleState != 0) {
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fRating2::TfRating2*>(Loop) != nullptr) {
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fJournal::TfJournal*>(Loop) != nullptr) {
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fSelectFace::TfSelectFace*>(Loop) != nullptr) {
                        Loop->RequestClose(254);
                    } else if (pas::class_cast_if<fScaner::TfScaner*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::ScannerReturnScreenId;
                        Loop->RequestClose(1);
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fShip2::TfShip2*>(Loop) != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fGalaxy2::TfGalaxy2*>(Loop) != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fShip2::TfShip2*>(Loop) != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
                        GlobalsV::RequestedScreenId = GlobalsV::ShipReturnScreenId;
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fGalaxy2::TfGalaxy2*>(Loop) != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
                        GlobalsV::RequestedScreenId = GlobalsV::GalaxyReturnScreenId;
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fGameMenu::TfGameMenu*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::GameMenuReturnScreenId;
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fSaveManager::TfSaveManager*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::SaveManagerReturnScreenId;
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fRewards::TfRewards*>(Loop) != nullptr) {
                        Loop->RequestClose(254);
                    } else if (pas::class_cast_if<fChameleon::TfChameleon*>(Loop) != nullptr) {
                        Loop->RequestClose(254);
                    } else if (pas::class_cast_if<fTalk::TfTalk*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::TalkReturnScreenId;
                        if (Globals::TalkScripted) {
                            GR_Main::RaiseWideMessage(u"gtalk AI"_wref.get());
                        }
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<ab_MainForm::TfAB*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
                        Loop->RequestClose(1);
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fLoad::TfLoad*>(Loop) != nullptr) {
                        if (aPlayer::GetPlayer()->InNormalSpace()) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
                            Globals::StarMapScreen->ResumeMode = fStarMap::smrOrders;
                        }
                        GlobalsV::PostLoadScreenId = GlobalsV::RequestedScreenId;
                        Loop->RequestClose(1);
                    } else if (pas::class_cast_if<fGameLoad::TfGameLoad*>(Loop) != nullptr) {
                        Loop->RequestClose(1);
                        ReopenScreen = false;
                    } else if (aPlayer::GetPlayer()->InNormalSpace() && (pas::class_cast_if<fHangar::TfHangar*>(Loop) != nullptr || pas::class_cast_if<fPlanet::TfPlanet*>(Loop) != nullptr || pas::class_cast_if<fPlanetNO::TfPlanetNO*>(Loop) != nullptr || pas::class_cast_if<fGov::TfGov*>(Loop) != nullptr || pas::class_cast_if<fPlanetQuest::TfPlanetQuest*>(Loop) != nullptr || pas::class_cast_if<fEquipmentShop::TfEquipmentShop*>(Loop) != nullptr || pas::class_cast_if<fGoodsShop2::TfGoodsShop2*>(Loop) != nullptr || pas::class_cast_if<fRuinsTalk::TfRuinsTalk*>(Loop) != nullptr || pas::class_cast_if<fInfo::TfInfo*>(Loop) != nullptr)) {
                        GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
                        Globals::StarMapScreen->ResumeMode = fStarMap::smrOrders;
                        if (pas::class_cast_if<fGoodsShop2::TfGoodsShop2*>(Loop) != nullptr) {
                            Loop->RequestClose(2);
                            Globals::TalkScreen->ModalTransition = fTalk::tmtNone;
                            Globals::GoodsShopScreen->ReopenRequested = false;
                            Globals::StarMapScreen->RequestClose(1);
                        } else {
                            Loop->RequestClose(1);
                        }
                        ReopenScreen = false;
                    } else if ((aPlayer::GetPlayer()->IsOnPlanet() || aPlayer::GetPlayer()->IsDockedToShip()) && pas::class_cast_if<fStarMap::TfStarMap*>(Loop) != nullptr) {
                        Loop->RequestClose(1);
                        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiUninhabited) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenPlanetNO;
                        } else if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
                        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
                        }
                    } else if (aPlayer::GetPlayer()->InHyperspace && aPlayer::GetPlayer()->Order == aShip::soJumpHole && (pas::class_cast_if<fStarMap::TfStarMap*>(Loop) != nullptr || pas::class_cast_if<ab_MainForm::TfAB*>(Loop) != nullptr)) {
                        GlobalsV::RequestedScreenId = GlobalsV::screenMainMenu;
                        Loop->RequestClose(1);
                        ReopenScreen = false;
                    } else if (aPlayer::GetPlayer()->InHyperspace && aPlayer::GetPlayer()->Order == aShip::soJump && pas::class_cast_if<fStarMap::TfStarMap*>(Loop) != nullptr) {
                        GlobalsV::RequestedScreenId = GlobalsV::screenJump;
                        Loop->RequestClose(1);
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fPlanetQuest::TfPlanetQuest*>(Loop) != nullptr) {
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fGov::TfGov*>(Loop) != nullptr && Globals::GovernmentScreen->PendingTransition != 0) {
                        ReopenScreen = false;
                    } else if (pas::class_cast_if<fJump::TfJump*>(Loop) != nullptr) {
                        Globals::JumpScreen->RestoreOrdersOnArrival = true;
                    }
                }
            }
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() && aPlayer::GetPlayer()->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited || aPlayer::GetPlayer()->IsDockedToShip() && pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
        }
        if (ReopenScreen) {
            pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->OnOpen();
        }
        aGalaxy::TGalaxy::ClearIntegrityStatus();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(1);
        pas::free(MemorySnapshotBuffer);
        MemorySnapshotBuffer = nullptr;
        GlobalsV::MemorySnapshotActive = false;
        aGalaxy::TGalaxy::RunConfigOnLoadHandlers();
    }

    void InitializeSaveWriter() {
        SaveWriter = pas::construct_call<TSaver>(EC_Thread::TThreadEC_Create);
    }

    // Waits for a pending write before freeing the worker.
    void FinalizeSaveWriter() {
        if (SaveWriter != nullptr) {
            if (SaveWriter->IsRunning()) {
                SaveWriter->WaitForIdle(WindowsSdk::INFINITE);
            }
            pas::free(SaveWriter);
            SaveWriter = nullptr;
        }
    }

    // Owns and frees all five buffers. Writes Save.tmp before replacing the destination; shares SaveLoadLock with the loader.
    void TSaver_Execute(TSaver* Self) {
        pas::AnsiString cpp_text{};
        pas::AnsiString cpp_text_2{};
        pas::AnsiString cpp_text_3{};
        pas::AnsiString cpp_text_4{};
        std::int32_t Size{};
        std::int32_t I{};
        std::int32_t Seed{};
        pas::WideString SourceName{};
        pas::WideString TargetName{};
        pas::WideString NewName{};
        pas::WideString OldName{};
        pas::WideString Prefix{};
        pas::WideString AutoName{};
        pas::WideString TurnName{};
        pas::WideString QuickName{};
        pas::WideString TempName{};
        pas::critical_enter(SaveLoadLock);
        AutoName = fSaveManager::TfSaveManager::GetAutoSavePath();
        TurnName = fSaveManager::TfSaveManager::GetTurnSavePath();
        QuickName = fSaveManager::TfSaveManager::GetQuickSavePath(1);
        SysUtilsImports::CreateDir(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Save"})));
        EC_File::TFileEC* F = nullptr;
        try {
            F = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
            TempName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"save\\save.tmp"});
            if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TempName))) {
                WindowsSdk::DeleteFile((cpp_text = static_cast<pas::AnsiString>(TempName), cpp_text.pchar()));
            }
            F->SetFileName(TempName);
            F->CreateNew();
            Self->HeaderBuffer->SaveToFile(F);
            if (Self->PreviewBuffer->DataSize > 0) {
                Self->PreviewBuffer->CompressZlibPayloadInPlace(false);
            }
            Size = Self->PreviewBuffer->DataSize;
            F->WriteBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                F->WriteBuffer(Self->PreviewBuffer->Data, Size);
            }
            if (Self->SecondaryPreviewBuffer->DataSize > 0) {
                Self->SecondaryPreviewBuffer->CompressZlibPayloadInPlace(false);
            }
            Size = Self->SecondaryPreviewBuffer->DataSize;
            F->WriteBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                F->WriteBuffer(Self->SecondaryPreviewBuffer->Data, Size);
            }
            Self->GameStateBuffer->CompressZlibPayloadInPlace(false);
            Size = Self->GameStateBuffer->ComputeCrc32();
            F->WriteBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            Seed = aMyFunction::RandomIntRange(0, 2000000000);
            Self->GameStateBuffer->ApplyDatXorCipher(Seed);
            F->WriteBuffer(&Seed, static_cast<std::int32_t>(sizeof(std::int32_t)));
            Size = Self->GameStateBuffer->DataSize;
            F->WriteBuffer(&Size, static_cast<std::int32_t>(sizeof(std::int32_t)));
            if (Size > 0) {
                F->WriteBuffer(Self->GameStateBuffer->Data, Size);
            }
            Self->FilmBuffer->CompressZlibPayloadInPlace(false);
            Size = Self->FilmBuffer->DataSize;
            F->WriteBuffer(Self->FilmBuffer->Data, Size);
            F->ReleaseHandle();
            SourceName = TempName;
            TargetName = Self->FileName;
            if (Self->FileName == QuickName && GlobalsV::QuickSaveExtraSlots > 0) {
                Prefix = pas::concat_wide({EC_Str::TrimWideString(EC_Str::ExtractFileDirW(pas::view(QuickName))), u"\\", EC_Str::TrimWideString(EC_Str::ExtractFileNameNoExtW(QuickName))});
                NewName = pas::concat_wide({Prefix, EC_Str::IntToWideString(GlobalsV::QuickSaveExtraSlots + 1), u".sav"});
                {
                    const std::int32_t cpp_first = GlobalsV::QuickSaveExtraSlots;
                    if (cpp_first >= 1) {
                        for (I = cpp_first; I >= 1; --I) {
                            if (I > 1) {
                                OldName = pas::concat_wide({Prefix, EC_Str::IntToWideString(I), u".sav"});
                            } else {
                                OldName = QuickName;
                            }
                            if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(NewName))) {
                                WindowsSdk::DeleteFile((cpp_text_2 = static_cast<pas::AnsiString>(NewName), cpp_text_2.pchar()));
                            }
                            WindowsSdk::MoveFileW(OldName.pchar(), NewName.pchar());
                            NewName = OldName;
                        }
                    }
                }
            }
            if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TargetName))) {
                WindowsSdk::DeleteFile((cpp_text_3 = static_cast<pas::AnsiString>(TargetName), cpp_text_3.pchar()));
            }
            WindowsSdk::MoveFileW(SourceName.pchar(), TargetName.pchar());
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(E->message);
                if (Self->FileName == AutoName) {
                    aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.AutoSaveFailed"_w);
                }
                if (Self->FileName == TurnName) {
                    aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.TurnSaveFailed"_w);
                }
                if (Self->FileName == QuickName) {
                    aGalaxy::TGalaxy::ShowLocalizedWarning(u"Warning.QuickSaveFailed"_w);
                }
            } else {
                throw;
            }
        }
        if (F != nullptr) {
            pas::free(F);
        }
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(TempName))) {
            WindowsSdk::DeleteFile((cpp_text_4 = static_cast<pas::AnsiString>(TempName), cpp_text_4.pchar()));
        }
        if (Self->HeaderBuffer != nullptr) {
            pas::free(Self->HeaderBuffer);
        }
        Self->HeaderBuffer = nullptr;
        if (Self->PreviewBuffer != nullptr) {
            pas::free(Self->PreviewBuffer);
        }
        Self->PreviewBuffer = nullptr;
        if (Self->SecondaryPreviewBuffer != nullptr) {
            pas::free(Self->SecondaryPreviewBuffer);
        }
        Self->SecondaryPreviewBuffer = nullptr;
        if (Self->GameStateBuffer != nullptr) {
            pas::free(Self->GameStateBuffer);
        }
        Self->GameStateBuffer = nullptr;
        if (Self->FilmBuffer != nullptr) {
            pas::free(Self->FilmBuffer);
        }
        Self->FilmBuffer = nullptr;
        pas::critical_leave(SaveLoadLock);
    }

    // Waits for the preceding job, then takes ownership of all five buffers.
    void TSaver::QueueSave(pas::WideString AFileName, EC_Buf::TBufEC* Header, EC_Buf::TBufEC* Preview, EC_Buf::TBufEC* SecondaryPreview, EC_Buf::TBufEC* GameState, EC_Buf::TBufEC* Films) {
        if (IsRunning()) {
            WaitForIdle(WindowsSdk::INFINITE);
        }
        FileName = std::move(AFileName);
        HeaderBuffer = Header;
        PreviewBuffer = Preview;
        SecondaryPreviewBuffer = SecondaryPreview;
        GameStateBuffer = GameState;
        FilmBuffer = Films;
        Start();
    }

    void TSaver::virtual_TThreadEC_Execute() {
        aSaveLoad::TSaver_Execute(this);
    }

} // namespace aSaveLoad
