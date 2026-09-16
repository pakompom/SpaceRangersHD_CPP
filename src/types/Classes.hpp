#pragma once
#include "runtime_support.hpp"

namespace Classes {
    using TNotifyEvent = pas::Method<void(pas::Object*)>;

    using TPointerList = pas::Array<void*, 0, 65535>;

    using PPointerList = TPointerList*;

} // namespace Classes
