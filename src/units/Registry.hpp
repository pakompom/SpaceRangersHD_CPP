#pragma once
#include "types/Registry.hpp"

namespace Registry {
    std::uint8_t IsRelative(const pas::AnsiString& Value);

    void TRegistry_Create(TRegistry* Self);

    void TRegistry_Destroy(TRegistry* Self);

} // namespace Registry
