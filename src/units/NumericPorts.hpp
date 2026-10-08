#pragma once
#include "runtime_support.hpp"

namespace NumericPorts {
    // Audited game arithmetic boundaries. Delphi executes the original formulas
    // below; the native C++ host bindings retain their selected PC24 rounding.
    inline std::int64_t RandomBound(double Value);

    inline double RandomMilli(std::int32_t Value);

    inline double SeedFraction(std::uint32_t Seed);

    inline double FractionalQuotient(std::int32_t Numerator, std::int32_t Denominator);

    inline std::int64_t QuestRound(double Value);

} // namespace NumericPorts

#include "inline/NumericPorts.hpp"
