#pragma once
#include "runtime.hpp"

namespace pas {
inline void mask_pixels8(const void* image, const void* mask, void* destination, int columns, int rows,
                         int image_skip, int mask_skip, int dest_skip, const void* multiplication_table) {
  auto* src = static_cast<const std::uint8_t*>(image);
  auto* alpha = static_cast<const std::uint8_t*>(mask);
  auto* dst = static_cast<std::uint8_t*>(destination);
  auto* table = static_cast<const std::uint8_t*>(multiplication_table);
  for (int y = 0; y < rows; ++y, src += image_skip, alpha += mask_skip, dst += dest_skip)
    for (int x = 0; x < columns; ++x, ++src, ++alpha, ++dst)
      *dst = table[*alpha * 256 + *src];
}
// RGBA palette indices blended into packed RGB565/RGB555. Preserve the game's
// per-contribution quantization and its no-write path for transparent entries.
inline void blend_palette16(const void* source, void* destination, int columns, int rows,
                            int source_skip, int dest_skip, const void* palette,
                            const void* multiplication_table, bool rgb565) {
  auto* src = static_cast<const std::uint8_t*>(source);
  auto* dst = static_cast<std::uint8_t*>(destination);
  auto* colors = static_cast<const std::uint8_t*>(palette);
  auto* table = static_cast<const std::uint8_t*>(multiplication_table);
  const int red_shift = rgb565 ? 8 : 7, green_shift = rgb565 ? 3 : 2;
  const unsigned red_mask = rgb565 ? 0xf800 : 0x7c00, green_mask = rgb565 ? 0x7e0 : 0x3e0;
  auto pack = [=](unsigned r, unsigned g, unsigned b) {
    return ((r << red_shift) & red_mask) | ((g << green_shift) & green_mask) | (b >> 3);
  };
  for (int y = 0; y < rows; ++y, src += source_skip, dst += dest_skip)
    for (int x = 0; x < columns; ++x, ++src, dst += 2) {
      const auto color = load_unaligned<std::uint32_t>(colors + *src * 4);
      const unsigned alpha = color >> 24;
      if (!alpha) continue;
      const auto* foreground = table + alpha * 256;
      const auto* background = table + (255 - alpha) * 256;
      const auto old = load_unaligned<std::uint16_t>(dst);
      const auto value = pack(foreground[color & 255], foreground[(color >> 8) & 255], foreground[(color >> 16) & 255])
        + pack(background[(old >> red_shift) & 0xf8], background[(old >> green_shift) & (rgb565 ? 0xfc : 0xf8)], background[(old << 3) & 0xf8]);
      store_unaligned(dst, static_cast<std::uint16_t>(value));
    }
}
// Explicit semantic ports of the game's BGRA assembly kernels. Byte-based
// storage permits unaligned rows. Copying remains forward even for overlap.
inline void copy_pixels32(const void* source, void* destination, int columns, int rows, int source_skip, int dest_skip) {
  auto* src = static_cast<const std::uint8_t*>(source);
  auto* dst = static_cast<std::uint8_t*>(destination);
  for (int y = 0; y < rows; ++y, src += source_skip, dst += dest_skip)
    for (int x = 0; x < columns; ++x, src += 4, dst += 4)
      store_unaligned(dst, load_unaligned<std::uint32_t>(src));
}
inline void blend_pixels32(const void* source, void* destination, int columns, int rows, int source_skip, int dest_skip, const void* multiplication_table) {
  auto* src = static_cast<const std::uint8_t*>(source);
  auto* dst = static_cast<std::uint8_t*>(destination);
  auto* table = static_cast<const std::uint8_t*>(multiplication_table);
  for (int y = 0; y < rows; ++y, src += source_skip, dst += dest_skip)
    for (int x = 0; x < columns; ++x, src += 4, dst += 4) {
      for (int channel = 0; channel < 3; ++channel)
        dst[channel] = table[src[channel] * 256 + src[3]] + table[dst[channel] * 256 + 255 - src[3]];
      dst[3] = std::min(unsigned(src[3]) + dst[3], 255u);
    }
}
inline void shadow_pixels32(void* pixels, int columns, int rows, int row_skip) {
  auto* dst = static_cast<std::uint8_t*>(pixels);
  for (int y = 0; y < rows; ++y, dst += row_skip)
    for (int x = 0; x < columns; ++x, dst += 4)
      store_unaligned(dst, (load_unaligned<std::uint32_t>(dst) >> 2) & 0xff000000u);
}
inline void swap_red_blue32(void* pixels, int count) {
  auto* dst = static_cast<std::uint8_t*>(pixels);
  for (int i = 0; i < count; ++i, dst += 4) std::swap(dst[0], dst[2]);
}
inline void fill_line16(void* pixels, int count, int step, std::uint32_t color) {
  auto* dst = static_cast<std::uint8_t*>(pixels);
  for (int i = 0; i < count; ++i, dst += step) store_unaligned(dst, std::uint16_t(color));
}
inline void fill_pixels32(void* pixels, int columns, int rows, int row_skip, std::uint32_t color) {
  auto* dst = static_cast<std::uint8_t*>(pixels);
  for (int y = 0; y < rows; ++y, dst += row_skip)
    for (int x = 0; x < columns; ++x, dst += 4) store_unaligned(dst, color);
}
inline void scale_alpha32(void* alpha, int columns, int rows, int row_skip, const void* alpha_table) {
  auto* dst = static_cast<std::uint8_t*>(alpha);
  auto* table = static_cast<const std::uint8_t*>(alpha_table);
  for (int y = 0; y < rows; ++y, dst += row_skip)
    for (int x = 0; x < columns; ++x, dst += 4) *dst = table[*dst];
}

}
