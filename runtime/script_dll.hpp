#pragma once
#include "runtime.hpp"
#if defined(_WIN32) && defined(__i386__)
#include <ffi.h>
#endif
#include <vector>

namespace pas {
// The game's library ABI passes only raw 32-bit words, including Single bits
// and Win32 pointers. Floating results also come back as raw word bits.
inline std::uint32_t script_call_words(std::uint32_t address,
                                       const DynArray<std::uint32_t>& words) {
#if defined(_WIN32) && defined(__i386__)
  const auto count = static_cast<unsigned>(words.length());
  std::array<ffi_type*, 16> small_types;
  std::array<void*, 16> small_arguments;
  std::vector<ffi_type*> large_types(count > 16 ? count : 0);
  std::vector<void*> large_arguments(count > 16 ? count : 0);
  auto* types = count <= 16 ? small_types.data() : large_types.data();
  auto* arguments = count <= 16 ? small_arguments.data() : large_arguments.data();
  for (unsigned i = 0; i < count; ++i) {
    types[i] = &ffi_type_uint32;
    arguments[i] = const_cast<std::uint32_t*>(&words.data()[i]);
  }
  ffi_cif signature;
  if (ffi_prep_cif(&signature, FFI_STDCALL, count, &ffi_type_uint32, types) != FFI_OK)
    throw std::runtime_error("invalid script DLL call signature");
  ffi_arg result = 0;
  ffi_call(&signature, reinterpret_cast<void(*)()>(static_cast<std::uintptr_t>(address)),
           &result, arguments);
  return static_cast<std::uint32_t>(result);
#else
  throw std::runtime_error("script DLL calls require the Win32 target");
#endif
}
}
