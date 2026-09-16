#pragma once
#include "windows.hpp"
#include <clocale>

namespace pas {
inline AnsiString initial_locale_text(std::uint32_t kind, const char* fallback) {
#ifdef _WIN32
  using LocaleInfo = int (PAS_STDCALL *)(std::uint32_t, std::uint32_t, char*, int);
  using ThreadLocale = std::uint32_t (PAS_STDCALL *)();
  static auto info = reinterpret_cast<LocaleInfo>(win::resolve_import("kernel32.dll", "GetLocaleInfoA"));
  static auto locale = reinterpret_cast<ThreadLocale>(win::resolve_import("kernel32.dll", "GetThreadLocale"));
  char value[256]{};
  const int length = info(locale(), kind, value, sizeof(value));
  if (length > 0) return AnsiString(value, length - 1);
#endif
  return AnsiString(fallback);
}
inline AnsiString initial_short_date_format() { return initial_locale_text(0x001f, "m/d/yy"); }
inline AnsiString initial_time_am_string() { return initial_locale_text(0x0028, "am"); }
inline AnsiString initial_time_pm_string() { return initial_locale_text(0x0029, "pm"); }
inline AnsiString initial_long_time_format() {
  // Delphi 2007 constructs this from the flags, not LOCALE_STIMEFORMAT.
  std::string format = initial_locale_text(0x0025, "0") == "0" ? "h:nn:ss" : "hh:nn:ss";
  if (initial_locale_text(0x0023, "0") == "0") {
    if (initial_locale_text(0x1005, "0") == "0") format += " AMPM";
    else format = "AMPM " + format;
  }
  return AnsiString(format.c_str());
}
inline std::uint8_t initial_locale_separator(std::uint32_t kind, std::uint8_t fallback) {
#ifdef _WIN32
  using LocaleInfo = int (PAS_STDCALL *)(std::uint32_t, std::uint32_t, char*, int);
  static auto get_locale_info = reinterpret_cast<LocaleInfo>(win::resolve_import("kernel32.dll", "GetLocaleInfoA"));
  char value[4]{};
  return get_locale_info(0x0400, kind, value, sizeof(value)) > 1 ? std::uint8_t(value[0]) : fallback;
#else
  return kind == 0x000e ? std::uint8_t(std::localeconv()->decimal_point[0]) : fallback;
#endif
}
inline std::uint8_t initial_decimal_separator() { return initial_locale_separator(0x000e, '.'); }
inline std::uint8_t initial_date_separator() { return initial_locale_separator(0x001d, '/'); }
inline std::uint8_t initial_time_separator() { return initial_locale_separator(0x001e, ':'); }
}
