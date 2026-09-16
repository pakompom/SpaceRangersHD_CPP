#include "layout/SystemImports.hpp"
#include "units/System.hpp"
#include "units/SystemImports.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace SystemImports {
    pas::Proc<void()> InitProc{};

    pas::AnsiString StringOfChar(std::uint8_t Value, std::int32_t Count) {
        pas::AnsiString Result{};
        Result = pas::AnsiString();
        if (Count > 0) {
            Result.set_length(Count);
            pas::fill_memory(Result.pchar(), Count, Value);
        }
        return Result;
    }

    void Randomize() {
        std::int64_t Counter{};
        if (WindowsImports::QueryPerformanceCounter(Counter)) {
            System::RandSeed = Counter;
        } else {
            System::RandSeed = WindowsImports::GetTickCount();
        }
    }

    std::uint8_t* GetParamStr(std::uint8_t* P, pas::AnsiString& Param) {
        std::uint8_t* Q{};
        while (true) {
            while (P[0] != '\000' && P[0] <= ' ') {
                P = WindowsImports::CharNext(P);
            }
            if (P[0] == '\"' && P[1] == '\"') {
                P += 2;
            } else {
                break;
            }
        }
        std::int32_t Len = 0;
        std::uint8_t* Start = P;
        while (P[0] > ' ') {
            if (P[0] == '\"') {
                P = WindowsImports::CharNext(P);
                while (P[0] != '\000' && P[0] != '\"') {
                    Q = WindowsImports::CharNext(P);
                    Len += static_cast<std::int32_t>(Q - P);
                    P = Q;
                }
                if (P[0] != '\000') {
                    P = WindowsImports::CharNext(P);
                }
            } else {
                Q = WindowsImports::CharNext(P);
                Len += static_cast<std::int32_t>(Q - P);
                P = Q;
            }
        }
        Param.set_length(Len);
        P = Start;
        std::uint8_t* S = static_cast<std::uint8_t*>(static_cast<void*>(Param.data()));
        std::int32_t I = 0;
        while (P[0] > ' ') {
            if (P[0] == '\"') {
                P = WindowsImports::CharNext(P);
                while (P[0] != '\000' && P[0] != '\"') {
                    Q = WindowsImports::CharNext(P);
                    while (P < Q) {
                        S[I] = *P;
                        ++P;
                        ++I;
                    }
                }
                if (P[0] != '\000') {
                    P = WindowsImports::CharNext(P);
                }
            } else {
                Q = WindowsImports::CharNext(P);
                while (P < Q) {
                    S[I] = *P;
                    ++P;
                    ++I;
                }
            }
        }
        return P;
    }

    std::int32_t ParamCount() {
        pas::AnsiString S{};
        std::int32_t Result = 0;
        std::uint8_t* P = SystemImports::GetParamStr(WindowsImports::GetCommandLineA(), S);
        while (true) {
            P = SystemImports::GetParamStr(P, S);
            if (S == "") {
                break;
            }
            ++Result;
        }
        return Result;
    }

    pas::AnsiString ParamStr(std::int32_t Index) {
        pas::AnsiString Result{};
        std::uint8_t* P{};
        std::uint32_t Count{};
        Result = pas::AnsiString();
        if (Index == 0) {
            Result.set_length(261);
            Count = WindowsImports::GetModuleFileNameA(0u, Result.pchar(), 261u);
            Result.set_length(static_cast<std::int32_t>(Count));
        } else {
            P = WindowsImports::GetCommandLineA();
            while (true) {
                P = SystemImports::GetParamStr(P, Result);
                if (Index == 0 || Result == "") {
                    break;
                }
                --Index;
            }
        }
        return Result;
    }

    std::int32_t BeginThread(void* SecurityAttributes, std::uint32_t StackSize, System::TThreadFunc ThreadFunc, void* Parameter, std::uint32_t CreationFlags, std::uint32_t& ThreadId) {
        PThreadStart Start{};
        if (System::SystemThreadFuncProc != nullptr) {
            Start = static_cast<PThreadStart>(System::SystemThreadFuncProc(ThreadFunc, Parameter));
        } else {
            pas::new_value(Start);
            Start->Func = ThreadFunc;
            Start->Parameter = Parameter;
        }
        System::IsMultiThread = true;
        std::int32_t Result = WindowsSdk::CreateThread(SecurityAttributes, StackSize, reinterpret_cast<WindowsSdk::TFNThreadStartRoutine>(pas::callback_address(pas::StdcallProc<std::int32_t(void*)>(RunThread))), Start, CreationFlags, ThreadId);
        if (Result == 0) {
            pas::dispose(Start);
        }
        return Result;
    }

    std::int32_t PAS_STDCALL RunThread(void* Parameter) {
        TThreadStart Start{};
        Start = pas::load_unaligned<TThreadStart>(static_cast<PThreadStart>(Parameter));
        pas::dispose(static_cast<PThreadStart>(Parameter));
        System::Set8087CW(0x00001332);
        return Start.Func(Start.Parameter);
    }

} // namespace SystemImports
