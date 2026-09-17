#pragma once
#include "types/GI_SpaceImg.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_SpaceImg {
    void TSpaceImgGI_Create(TSpaceImgGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TSpaceImgGI_Destroy(TSpaceImgGI* Self);

    void TSpaceImgGI_LoadFromConfigPath(TSpaceImgGI* Self, const pas::WideString& Path);

} // namespace GI_SpaceImg
