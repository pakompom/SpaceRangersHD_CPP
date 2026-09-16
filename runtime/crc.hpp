#pragma once
// Data-level ports of the four CrcUnit assembly helpers. The table stays in
// source/core/CrcUnit.pas and is supplied by the generated unit.
#include <bit>
#include <cstdint>
#include <stdexcept>

namespace pas {
inline std::uint32_t crc32_update(std::uint32_t state, const void *buffer,
                                  std::int32_t count,
                                  const std::uint32_t *table) {
  if (count < 0)
    throw std::range_error("negative CRC byte count");
  auto *bytes = static_cast<const std::uint8_t *>(buffer);
  for (std::int32_t i = 0; i < count; ++i)
    state = (state >> 8) ^ table[(state ^ bytes[i]) & 255];
  return state;
}
inline std::int32_t crc32_signed_span(const void *buffer, std::int32_t count) {
  if (count <= 0)
    return 0;
  // ComputeCrc32/UpdateCrc32 use a signed comparison of Win32 start/end
  // addresses. Retain the early exit for spans crossing the sign boundary.
  auto start = std::uint32_t(reinterpret_cast<std::uintptr_t>(buffer));
  auto end = start + std::uint32_t(count);
  return std::bit_cast<std::int32_t>(start) < std::bit_cast<std::int32_t>(end)
             ? count
             : 0;
}
} // namespace pas
