#pragma once
#include "types/aMissile.hpp"

namespace aMissile {
    void TMissile_Create(TMissile* Self);

    void TMissile_Destroy(TMissile* Self);

    void TMissile_PrepareTurnMovement(TMissile* Self, std::int32_t StepIndex, std::uint8_t RecordFilm, std::uint8_t PlayShotSound);

} // namespace aMissile
