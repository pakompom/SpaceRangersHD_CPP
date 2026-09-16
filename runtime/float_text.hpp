#pragma once
#include "runtime.hpp"
#include "reals.hpp"
#include "windows.hpp"
#include <cfloat>
#include "locale.hpp"

namespace pas {
// Delphi 2007 System._Pow10 uses three table factors, in this order. Keep
// ordinary floating operations and the caller's x87 precision/rounding mode.
inline long double scale_decimal(long double value, int power) {
#pragma STDC FENV_ACCESS ON
#if LDBL_MANT_DIG == 64
  static constexpr long double small[] = {1e0L, 1e1L, 1e2L, 1e3L, 1e4L, 1e5L, 1e6L, 1e7L, 1e8L, 1e9L, 1e10L, 1e11L, 1e12L, 1e13L, 1e14L, 1e15L, 1e16L, 1e17L, 1e18L, 1e19L, 1e20L, 1e21L, 1e22L, 1e23L, 1e24L, 1e25L, 1e26L, 1e27L, 1e28L, 1e29L, 1e30L, 1e31L};
  static constexpr long double middle[] = {1e32L, 1e64L, 1e96L, 1e128L, 1e160L, 1e192L, 1e224L, 1e256L, 1e288L, 1e320L, 1e352L, 1e384L, 1e416L, 1e448L, 1e480L};
  static constexpr long double large[] = {1e512L, 1e1024L, 1e1536L, 1e2048L, 1e2560L, 1e3072L, 1e3584L, 1e4096L, 1e4608L};
  bool divide = power < 0;
  unsigned n = unsigned(divide ? -power : power);
  if (!n) return value;
  if (n >= 5120) return divide ? 0.0L : std::numeric_limits<long double>::infinity();
  const auto apply = [&](long double factor) { value = divide ? value / factor : value * factor; };
  apply(small[n % 32]);
  if (unsigned index = (n / 32) % 16) apply(middle[index - 1]);
  if (unsigned index = n / 512) apply(large[index - 1]);
  return value;
#else
  return value * std::pow(10.0L, power);
#endif
}

// Selected SysUtils.TextToFloat grammar and decimal accumulation. In particular,
// a decimal point alone and an exponent without digits are accepted by this RTL.
// Conversion always uses nearest rounding and the full x87 significand.
inline long double str_to_float(const AnsiString& text, std::uint8_t decimal_separator) {
#pragma STDC FENV_ACCESS ON
#pragma STDC FP_CONTRACT OFF
  struct Control {
    std::uint16_t saved = get_fpu_control();
    Control() { set_fpu_control(0x133f); }
    ~Control() { set_fpu_control(saved); }
  } control;
  auto invalid = [&]() -> void {
    raise(make_exception<ConvertError>(AnsiString("'") + text + AnsiString("' is not a valid floating point value")));
  };
  auto* cursor = text.c_str();
  while (*cursor == ' ') ++cursor;
  const bool negative = *cursor == '-';
  if (*cursor == '+' || negative) ++cursor;
  auto* start = cursor;
  long double value = 0;
  auto digits = [&]() {
    int count = 0;
    while (*cursor >= '0' && *cursor <= '9') {
      value = value * 10;
      value = value + (*cursor++ - '0');
      ++count;
    }
    return count;
  };
  digits();
  int fraction = 0;
  if (std::uint8_t(*cursor) == decimal_separator) { ++cursor; fraction = digits(); }
  if (cursor == start) invalid();
  int exponent = 0;
  if (*cursor == 'e' || *cursor == 'E') {
    ++cursor;
    const bool exponent_negative = *cursor == '-';
    if (*cursor == '+' || exponent_negative) ++cursor;
    while (exponent < 500 && *cursor >= '0' && *cursor <= '9')
      exponent = exponent * 10 + (*cursor++ - '0');
    if (exponent_negative) exponent = -exponent;
  }
  while (*cursor == ' ') ++cursor;
  if (*cursor) invalid();
  const auto power = std::int64_t(exponent) - fraction;
  value = power <= -5120 ? 0.0L : scale_decimal(value, int(power));
  if (negative) value = -value;
  if (!std::isfinite(value) || std::fetestexcept(FE_INVALID | FE_OVERFLOW)) invalid();
  return value;
}

// Decimal digit generation shared by FloatToStr and the game's ffFixed calls.
// As in selected FloatToDecimal, the first rounding obeys x87 mode and the
// significant/fractional digit cut then rounds half-up.
struct DecimalDigits { std::string digits; int exponent = 0; };
inline DecimalDigits decimal_digits(long double value, int precision, int decimals) {
#pragma STDC FENV_ACCESS ON
  if (value == 0 || std::fpclassify(value) == FP_SUBNORMAL) return {};
  const auto exponent_product = std::int64_t(std::ilogb(value)) * 19728;
  int exponent = int((exponent_product >= 0 ? exponent_product : exponent_product - 65535) / 65536) + 1;
  long double scaled = std::rint(scale_decimal(std::fabs(value), 18 - exponent));
  if (scaled >= 1e18L) { scaled = std::rint(scaled / 10.0L); ++exponent; }
  auto digits = std::to_string(std::uint64_t(scaled));
  if (digits.size() < 18) digits.insert(0, 18 - digits.size(), '0');
  const int keep = std::min(precision, exponent + decimals);
  if (keep < 0) return {};
  if (keep < 18) {
    const bool round_up = digits[std::size_t(keep)] >= '5';
    digits.resize(std::size_t(keep));
    if (round_up) {
      int i = keep - 1;
      while (i >= 0 && digits[std::size_t(i)] == '9') digits[std::size_t(i--)] = '0';
      if (i < 0) { digits = "1"; ++exponent; }
      else ++digits[std::size_t(i)];
    }
  }
  while (!digits.empty() && digits.back() == '0') digits.pop_back();
  return {std::move(digits), exponent};
}
inline AnsiString general_decimal(DecimalDigits number, bool negative, int precision, std::uint8_t separator) {
  auto& digits = number.digits;
  const auto exponent = number.exponent;
  if (digits.empty()) return "0";
  std::string result = negative ? "-" : "";
  if (exponent > precision || exponent < -3) {
    result += digits[0];
    if (digits.size() > 1) { result += char(separator); result += digits.substr(1); }
    result += 'E'; result += std::to_string(exponent - 1);
  } else if (exponent <= 0) {
    result += '0'; result += char(separator);
    result.append(std::size_t(-exponent), '0'); result += digits;
  } else {
    if (digits.size() <= std::size_t(exponent)) digits.append(std::size_t(exponent) - digits.size(), '0');
    else digits.insert(std::size_t(exponent), 1, char(separator));
    result += digits;
  }
  return AnsiString(result.data(), std::int32_t(result.size()));
}
inline AnsiString float_to_str(long double value, std::uint8_t separator) {
  if (std::isnan(value)) return "NAN";
  if (std::isinf(value)) return std::signbit(value) ? "-INF" : "INF";
  return general_decimal(decimal_digits(value, 15, 9999), std::signbit(value), 15, separator);
}
inline AnsiString float_to_fixed(long double value, int precision, int decimals, std::uint8_t separator) {
  if (std::isnan(value)) return "NAN";
  if (std::isinf(value)) return std::signbit(value) ? "-INF" : "INF";
  precision = std::clamp(precision, 2, 18);
  // FloatToText clamps the displayed decimal count to 18. Its conversion uses
  // the supplied count first; preserve that distinction for ordinary inputs.
  if (decimals < 0 || decimals > 9999) throw std::runtime_error("Unsupported ffFixed decimal count");
  auto number = decimal_digits(value, precision, decimals);
  if (number.exponent > precision) return general_decimal(std::move(number), std::signbit(value), precision, separator);
  std::string result = std::signbit(value) && !number.digits.empty() ? "-" : "";
  auto digit = [&](int place) { return place >= 0 && std::size_t(place) < number.digits.size() ? number.digits[std::size_t(place)] : '0'; };
  if (number.exponent <= 0) result += '0';
  else for (int i = 0; i < number.exponent; ++i) result += digit(i);
  decimals = std::min(decimals, 18);
  if (decimals && separator) result += char(separator);
  for (int i = 0; i < decimals; ++i) result += digit(number.exponent + i);
  return AnsiString(result.data(), std::int32_t(result.size()));
}
}
