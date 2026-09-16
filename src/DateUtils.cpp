#include "units/DateUtils.hpp"
#include "units/System.hpp"

namespace DateUtils {
    std::int64_t DateTimeToUnix(System::TDateTime AValue) {
        return System::Round((AValue - 25569.0L) * 8.64E+4L);
    }

    System::TDateTime UnixToDateTime(std::int64_t AValue) {
        return pas::real_divide(AValue, 8.64E+4L) + 25569.0L;
    }

} // namespace DateUtils
