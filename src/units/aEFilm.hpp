#pragma once
#include "types/System.hpp"
#include "types/aEFilm.hpp"

namespace aEFilm {
    // Native writer format, read from initialized storage.
    extern const std::int32_t FilmFormatVersion;

    void TEFilm_Create(TEFilm* Self);

    void TEFilm_Destroy(TEFilm* Self);

    // const preserves the repeated native loads when this guard is inlined.
    // With sound flags, use a separate if to avoid compiler Boolean temporaries.
    std::uint8_t HasSceneObject(PEFilmCommand Command);

    // Keep all three tests in one expression to preserve the recovered load order.
    std::uint8_t HasSceneObjectOfClass(PEFilmCommand Command, System::TClass SceneClass);

} // namespace aEFilm
