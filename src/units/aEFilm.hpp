#pragma once
#include "types/aEFilm.hpp"

namespace aEFilm {
    extern const std::int32_t FilmFormatVersion;

    void TEFilm_Create(TEFilm* Self);

    void TEFilm_Destroy(TEFilm* Self);

} // namespace aEFilm
