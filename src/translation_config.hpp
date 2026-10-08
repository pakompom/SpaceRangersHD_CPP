#pragma once
#ifndef SRHD_NATIVE_REALS
#define SRHD_NATIVE_REALS 1
#elif SRHD_NATIVE_REALS != 1
#error Floating mode differs from the generated translation
#endif
#ifndef SRHD_PORTABLE_REALS
#define SRHD_PORTABLE_REALS 0
#elif SRHD_PORTABLE_REALS != 0
#error Floating mode differs from the generated translation
#endif
#ifndef SRHD_FP_REFERENCE
#define SRHD_FP_REFERENCE 0
#endif

// Every translation unit must preserve the hardware-PC24 operation boundaries.
#include "pc24_contract.hpp"
