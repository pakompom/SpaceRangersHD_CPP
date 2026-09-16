#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"

// Ordinary Delphi 2007 Math RTL algorithms used by equipment calculations.
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
        // All game calls pass division expressions, selecting the Extended overload.
        return Math::ArcTan2(X, System::Sqrt(1.0L - pas::sqr(X)));
    }

    pas::Extended ArcCos(pas::Extended X) {
        // Selected RTL's Extended formula avoids cancellation in 1-X*X near one.
        return Math::ArcTan2(System::Sqrt((1.0L + X) * (1.0L - X)), X);
    }

    std::int32_t Sign(double AValue) {
        std::int64_t Bits{};
        // Delphi's real overload tests the sign bit, including signed NaNs.
        // The game's other calls pass Integer values, exactly representable here.
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
