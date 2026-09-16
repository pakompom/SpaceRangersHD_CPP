#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilm;

} // namespace aEFilm

namespace aEFilmEnd {
    struct TEFilmEndEntry;

    struct TEFilmEnd;

    using PEFilmEndEntry = TEFilmEndEntry*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEFilmEnd : EC_Struct::TObjectEx {
        PAS_CLASS_META(TEFilmEnd, EC_Struct::TObjectEx, "TEFilmEnd", 12)
        void p_destroy() override;
        void Clear();
        PEFilmEndEntry AppendEntry();
        void RemoveEntry(PEFilmEndEntry Entry);
        void TakeTrailingEffects(aEFilm::TEFilm* Film);
        void AdvanceEffects();
        void ReleaseObjectReferences(SE_Space::TObjectSE* Obj);
        void RemoveLinkedWeaponEffects();
        PEFilmEndEntry FirstEntry;
        PEFilmEndEntry LastEntry;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TEFilmEndEntry {
        PEFilmEndEntry Prev;
        PEFilmEndEntry Next;
        SE_Space::TObjectSE* SceneObject;
        SE_Space::TObjectSE* RelatedObject1;
        SE_Space::TObjectSE* RelatedObject2;
    };
    #pragma pack(pop)

} // namespace aEFilmEnd
