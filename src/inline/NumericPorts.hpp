#pragma once
#include "units/NumericPorts.hpp"

namespace NumericPorts {
    // Audited game arithmetic boundaries. Delphi executes the original formulas
    // below; the native C++ host bindings retain their selected PC24 rounding.
    inline std::int64_t RandomBound(double Value) {
        return pas::legacy_random_bound(Value);
    }

    inline double RandomMilli(std::int32_t Value) {
        return pas::legacy_random_milli(Value);
    }

    inline double SeedFraction(std::uint32_t Seed) {
        return pas::legacy_seed_fraction(Seed);
    }

    inline double FractionalQuotient(std::int32_t Numerator, std::int32_t Denominator) {
        return pas::legacy_fractional_quotient(Numerator, Denominator);
    }

    inline std::int64_t QuestRound(double Value) {
        return pas::legacy_quest_round(Value);
    }

} // namespace NumericPorts
