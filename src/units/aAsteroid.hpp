#pragma once
#include "types/aAsteroid.hpp"

namespace aAsteroid {
    extern const float AsteroidGravitationalConstant;

    void TAsteroid_Create(TAsteroid* Self);

    void TAsteroid_Destroy(TAsteroid* Self);

} // namespace aAsteroid
