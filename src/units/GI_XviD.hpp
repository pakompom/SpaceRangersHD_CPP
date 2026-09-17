#pragma once
#include "types/GI_XviD.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_XviD {
    extern std::uint32_t XvidLibrary;

    extern GI_XviD::TXvidFunction XvidGlobal;

    extern GI_XviD::TXvidFunction XvidDecore;

    void TxvidGI_Create(TxvidGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TxvidGI_Destroy(TxvidGI* Self);

    void TxvidGI_LoadFromConfigPath(TxvidGI* Self, const pas::WideString& Path);

} // namespace GI_XviD
