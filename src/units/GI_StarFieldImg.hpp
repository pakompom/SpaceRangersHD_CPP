#pragma once
#include "types/GI_StarFieldImg.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_StarFieldImg {
    void TStarFieldImgGI_Create(TStarFieldImgGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TStarFieldImgGI_Destroy(TStarFieldImgGI* Self);

} // namespace GI_StarFieldImg
