#pragma once
#include "types/GI_PSMissileHit.hpp"

namespace GI_PSMissileHit {
    extern pas::DynArray<GI_PSMissileHit::TGAISet> MissileHitAnimationPaths;

    void LoadMissileHitAnimationPaths();

} // namespace GI_PSMissileHit
