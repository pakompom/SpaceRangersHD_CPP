#pragma once
#include <cfloat>
#include <limits>

// This backend depends on value widths, not an ARM instruction set. In
// particular, x86-32 must use SSE2 rather than excess-precision x87 evaluation.
static_assert(sizeof(float) == 4 && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
              FLT_MAX_EXP == 128 && std::numeric_limits<float>::is_iec559);
static_assert(sizeof(double) == 8 && DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024 &&
              std::numeric_limits<double>::is_iec559);
static_assert(FLT_EVAL_METHOD == 0,
              "PC24 needs float/double evaluation at source precision; use SSE2 on x86-32");
#ifdef __FAST_MATH__
#error PC24 rounding boundaries require strict arithmetic, without fast-math
#endif
