#pragma once
#include <cstdint>
#include <stdexcept>
#if defined(__i386__) || defined(__x86_64__)
#include <x86intrin.h>
#endif

namespace pas {
// The game reports the low 32 bits of a sampled TSC delta as CPU MHz.
inline std::uint32_t read_cycle_counter() {
#if defined(__i386__) || defined(__x86_64__)
  return static_cast<std::uint32_t>(__rdtsc());
#else
  throw std::runtime_error("CPU cycle counter requires an x86 target");
#endif
}
}
