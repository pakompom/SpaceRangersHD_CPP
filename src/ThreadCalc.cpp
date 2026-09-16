#include "layout/ThreadCalc.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SystemImports.hpp"
#include "types/aEFilm.hpp"
#include "types/aPlanet.hpp"
#include "types/aShip.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aGalaxy.hpp"
#include "units/aPlayer.hpp"

namespace ThreadCalc {
    // Smoothed film-progress threshold derived from measured galaxy-turn duration.
    float AdaptiveBeginCalcNextTurn = 0.5f;

    // Measured milliseconds; native smoothing uses signed arithmetic.
    std::int32_t LastGalaxyTurnDuration{};

    void StartGalaxyTurnCalculation() {
        Globals::TurnCalculationThread->Job = tcjGalaxy;
        Globals::TurnCalculationThread->Start();
    }

    void StartPlayerStarTurnCalculation() {
        Globals::TurnCalculationThread->Job = tcjPlayerStar;
        Globals::TurnCalculationThread->Start();
    }

    void StartPlayerStarPreparation() {
        Globals::TurnCalculationThread->Job = tcjPreparePlayerStar;
        Globals::TurnCalculationThread->Start();
    }

    std::uint8_t IsTurnCalculationRunning() {
        if (Globals::TurnCalculationThread == nullptr) {
            return false;
        }
        return Globals::TurnCalculationThread->IsRunning();
    }

    // Requires an initialized calculation thread.
    void WaitForTurnCalculation() {
        if (Globals::TurnCalculationThread->IsRunning()) {
            Globals::TurnCalculationThread->WaitForIdle(WindowsSdk::INFINITE);
        }
    }

    void ProcessPlayerStarTurn() {
        std::uint8_t RecordFilm{};
        std::int32_t Stage = 0;
        try {
            if (!Globals::PlayerStarDayPrepared) {
                Globals::PrimaryFilm->Clear();
            }
            Stage = 1;
            RecordFilm = aPlayer::GetPlayer()->InNormalSpace() || aPlayer::GetPlayer()->Order == aShip::soTakeoff && (aPlayer::GetPlayer()->CurrentPlanet != nullptr || aPlayer::GetPlayer()->DockedTo != nullptr) || aPlayer::GetPlayer()->InHyperspace && static_cast<std::uint32_t>(aPlayer::GetPlayer()->OrderStateData & 0x0000ffff) <= 1;
            aGalaxy::PlayerStar->NextDay(RecordFilm);
            Stage = 2;
            if (aGalaxy::Galaxy->StasisModEnabled != 1) {
                aGalaxy::Galaxy->CompleteDay(RecordFilm);
            }
            Stage = 3;
            aGalaxy::Galaxy->TransferShipsInTransit();
            Stage = 4;
            Globals::PlayerStarDayPrepared = false;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                reinterpret_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
                GR_Main::ExitScreenLoop = true;
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure ThCa label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TThreadCalc_Execute(TThreadCalc* Self) {
        std::uint32_t StartTick{};
        std::uint32_t EndTick{};
        std::int32_t FrameMs{};
        // Native handwritten x87 setup: each calculation thread establishes its own control word.
        std::uint16_t ControlWord = 0x0000103f;
        System::Set8087CW(ControlWord);
        if (Self->Job == tcjGalaxy) {
            aCalc::TurnCalculationPhase = tcpGalaxyRunning;
            if (aGalaxy::Galaxy->StasisModEnabled != 1) {
                try {
                    if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
                        StartTick = MMSystem::timeGetTime();
                        aGalaxy::Galaxy->NextDay();
                        EndTick = MMSystem::timeGetTime();
                        LastGalaxyTurnDuration = EndTick - StartTick;
                        if (GlobalsV::FilmSpeed == 0) {
                            FrameMs = 16;
                        } else if (GlobalsV::FilmSpeed == 1) {
                            FrameMs = 12;
                        } else {
                            FrameMs = 8;
                        }
                        AdaptiveBeginCalcNextTurn = pas::real_min<pas::Extended>(0.9L, pas::real_divide(AdaptiveBeginCalcNextTurn + 1.0L - pas::real_min<pas::Extended>(1.0L, pas::real_divide(LastGalaxyTurnDuration + 100, 200 * FrameMs)), 2.0L));
                    } else {
                        aGalaxy::Galaxy->NextDay();
                    }
                    aGalaxy::Galaxy->TransferShipsInTransit();
                } catch (...) {
                    auto cpp_exception = pas::caught_object();
                    if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                        GR_Main::AppendLogLineThreadSafe("ThreadCalc exception 1"_a);
                        if (aGalaxy::Galaxy->CurrentTurn < 300) {
                            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Galaxy create exception, seed = ", SysUtils::IntToStr(aGalaxy::Galaxy->GenerationSeed)}));
                        }
                        WindowsSdk::SetEvent(Self->IdleEvent);
                        throw;
                    } else {
                        throw;
                    }
                }
            }
            aCalc::TurnCalculationPhase = tcpGalaxyFinished;
        } else if (Self->Job == tcjPlayerStar) {
            aCalc::TurnCalculationPhase = tcpPlayerStarRunning;
            try {
                ThreadCalc::ProcessPlayerStarTurn();
            } catch (...) {
                auto cpp_exception_2 = pas::caught_object();
                if (pas::Exception* E_2 = pas::class_cast_if<pas::Exception*>(cpp_exception_2)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_2))), " ", E_2->message}));
                    GR_Main::AppendLogLineThreadSafe("ThreadCalc exception 2"_a);
                    if (aGalaxy::Galaxy->CurrentTurn < 300) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Galaxy create exception, seed = ", SysUtils::IntToStr(aGalaxy::Galaxy->GenerationSeed)}));
                    }
                    WindowsSdk::SetEvent(Self->IdleEvent);
                    throw;
                } else {
                    throw;
                }
            }
            aCalc::TurnCalculationPhase = tcpPlayerStarFinished;
        } else {
            aCalc::TurnCalculationPhase = tcpPlayerStarPreparationRunning;
            Globals::PlayerStarDayPrepared = true;
            try {
                Globals::PrimaryFilm->Clear();
                if (aGalaxy::Galaxy->StasisModEnabled != 1) {
                    aGalaxy::PlayerStar->PrepareNextDay();
                }
            } catch (...) {
                auto cpp_exception_3 = pas::caught_object();
                if (pas::Exception* E_3 = pas::class_cast_if<pas::Exception*>(cpp_exception_3)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E_3))), " ", E_3->message}));
                    GR_Main::AppendLogLineThreadSafe("ThreadCalc exception 3"_a);
                    if (aGalaxy::Galaxy->CurrentTurn < 300) {
                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Galaxy create exception, seed = ", SysUtils::IntToStr(aGalaxy::Galaxy->GenerationSeed)}));
                    }
                    WindowsSdk::SetEvent(Self->IdleEvent);
                    throw;
                } else {
                    throw;
                }
            }
            aCalc::TurnCalculationPhase = tcpPlayerStarPrepared;
        }
    }

    void TThreadCalc::virtual_TThreadEC_Execute() {
        ThreadCalc::TThreadCalc_Execute(this);
    }

} // namespace ThreadCalc
