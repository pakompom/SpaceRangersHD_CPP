#pragma once
#include "types/GR_Music.hpp"

namespace GR_Music {
    pas::WideString ChooseMusicFile(const pas::WideString& Category, const pas::WideString& CurrentFile);

    void TMusicUnit_Create(TMusicUnit* Self, char16_t* LibraryName);

    void TMusicUnit_Destroy(TMusicUnit* Self);

    void TMusicUnit_Execute(TMusicUnit* Self);

    void TMusicControl_Create(TMusicControl* Self);

    void TMusicControl_Destroy(TMusicControl* Self);

    void TMusicControl_Execute(TMusicControl* Self);

} // namespace GR_Music
