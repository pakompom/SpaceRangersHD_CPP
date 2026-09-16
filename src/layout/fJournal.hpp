#pragma once
#include "types/fJournal.hpp"

namespace fJournal {
    static_assert(sizeof(void*) != 4 || sizeof(fJournal::TfJournal) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fJournal::TfJournal, InfoPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fJournal::TfJournal, ContentHeight) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fJournal::TfJournal, JournalSelected) == 220);

} // namespace fJournal
