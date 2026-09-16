#pragma once
#include "types/aEFilm.hpp"

namespace aEFilm {
    // Native writer format, read from initialized storage.
    extern const std::int32_t FilmFormatVersion;

    void TEFilm_Create(TEFilm* Self);

    void TEFilm_Destroy(TEFilm* Self);

} // namespace aEFilm
