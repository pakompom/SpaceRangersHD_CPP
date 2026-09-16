#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"

namespace MathImports {
    double RoundTo(double AValue, TRoundToRange ADigit) {
        double Factor = Math::IntPower(1.0E+1L, ADigit);
        return static_cast<long double>(System::Round(pas::real_divide(AValue, Factor))) * Factor;
    }

    std::int32_t Ceil(pas::Extended X) {
        std::int32_t Result = System::Trunc(X);
        if (System::Frac(X) > 0.0L) {
            ++Result;
        }
        return Result;
    }

    std::int32_t Floor(pas::Extended X) {
        std::int32_t Result = System::Trunc(X);
        if (System::Frac(X) < 0.0L) {
            --Result;
        }
        return Result;
    }

    pas::Extended ArcSin(pas::Extended X) {
        return Math::ArcTan2(X, System::Sqrt(1.0L - pas::sqr(X)));
    }

    pas::Extended ArcCos(pas::Extended X) {
        return Math::ArcTan2(System::Sqrt((1.0L + X) * (1.0L - X)), X);
    }

    std::int32_t Sign(double AValue) {
        std::int64_t Bits{};
        pas::move_memory(&AValue, &Bits, static_cast<std::int32_t>(sizeof(std::int64_t)));
        if ((Bits & 0x7fffffffffffffff) == 0) {
            return 0;
        } else if (Bits < 0) {
            return -1;
        } else {
            return 1;
        }
    }

} // namespace MathImports
