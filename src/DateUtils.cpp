#include "types/System.hpp"
#include "units/DateUtils.hpp"

// Selected Delphi 2007 Unix/date algorithms. The source values have no timezone.
namespace DateUtils {
    std::int64_t DateTimeToUnix(System::TDateTime AValue) {
        return pas::real_round(static_cast<float>((static_cast<float>(AValue - 25569.0)) * 8.64E+4));
    }

    System::TDateTime UnixToDateTime(std::int64_t AValue) {
        return static_cast<float>(static_cast<float>(AValue / 8.64E+4) + 25569.0);
    }

} // namespace DateUtils
