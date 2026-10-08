#pragma once
#include "units/GI_Image.hpp"
#include "units/GI_MessageLoop.hpp"

namespace GI_Image {
    inline void TImageGI_Create(TImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

} // namespace GI_Image
