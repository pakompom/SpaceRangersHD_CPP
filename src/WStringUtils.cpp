#include "units/WStringUtils.hpp"

// Shared string helpers before NoSteamAchievemens. WStringUtils is the native
// linked unit in this dependency family; attribution is inferred from that context.
namespace WStringUtils {
    PStartupWideString AllocateStartupWideString(std::int32_t Length) {
        PStartupWideString Result{};
        pas::new_value(Result);
        (*Result).set_length(Length);
        return Result;
    }

    // Does not clear the disposed pointer.
    std::uint8_t FreeStartupWideString(pas::Var<PStartupWideString> Text) {
        std::uint8_t Result = false;
        if (pas::load_unaligned<PStartupWideString>(Text.address) != nullptr) {
            pas::dispose(pas::load_unaligned<PStartupWideString>(Text.address));
            return true;
        }
        return Result;
    }

    // Shrinks a caller-provided WideString to its first zero. Requires a valid pointer and a terminator within the buffer.
    PStartupWideString TruncateStartupWideString(pas::Var<PStartupWideString> Text) {
        std::int32_t Count = 0;
        while ((*pas::load_unaligned<PStartupWideString>(Text.address)).read(Count + 1) != u'\000') {
            ++Count;
        }
        (*pas::load_unaligned<PStartupWideString>(Text.address)).set_length(Count);
        return pas::load_unaligned<PStartupWideString>(Text.address);
    }

} // namespace WStringUtils
