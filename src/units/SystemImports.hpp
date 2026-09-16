#pragma once
#include "types/System.hpp"
#include "types/SystemImports.hpp"

namespace SystemImports {
    extern pas::Proc<void()> InitProc;

    pas::AnsiString StringOfChar(std::uint8_t Value, std::int32_t Count);

    void Randomize();

    std::uint8_t* GetParamStr(std::uint8_t* P, pas::AnsiString& Param);

    std::int32_t ParamCount();

    pas::AnsiString ParamStr(std::int32_t Index);

    std::int32_t BeginThread(void* SecurityAttributes, std::uint32_t StackSize, System::TThreadFunc ThreadFunc, void* Parameter, std::uint32_t CreationFlags, std::uint32_t& ThreadId);

    std::int32_t PAS_STDCALL RunThread(void* Parameter);

} // namespace SystemImports
