#pragma once
#include "types/GI_Cursor.hpp"
#include "types/WindowsImports.hpp"
#include "types/Windows_group.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Cursor {
    void TCursorGI_Create(TCursorGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TCursorGI_Destroy(TCursorGI* Self);

    WindowsSdk::HBITMAP PAS_STDCALL CreateDIBSection(WindowsSdk::HDC DC, pas::ConstRef<WindowsSdk::TBitmapInfo> BitmapInfo, std::uint32_t Usage, void*& Bits, WindowsImports::THandle Section, std::uint32_t Offset);

} // namespace GI_Cursor
