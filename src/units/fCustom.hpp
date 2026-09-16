#pragma once
#include "types/fCustom.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fCustom {
    // Borrowed while ShowCustomDialog is active; nested custom dialogs restore the previous value.
    extern GI_MessageLoop::TMessageLoopGI* CurrentCustomDialog;

    // Native TfCustomLoop VMT and the surrounding modal launcher establish ownership.
    // Loads the named layout and executes its opening/closing script text. Restores the previous active custom dialog on return.
    std::int32_t ShowCustomDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ScreenName);

} // namespace fCustom
