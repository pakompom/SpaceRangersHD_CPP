// Shared Delphi string ownership, conversions and exception paths.
#include "classes.hpp"
#include "reals.hpp"
#ifdef _WIN32
#include <float.h>
#endif

namespace pas {
namespace {
#ifdef _WIN32
// Run once before main and the translated Delphi unit initializers. Source
// global real initializers are constants; their storage needs no FPU setup.
const bool extended_precision = [] {
  _controlfp(_PC_64, _MCW_PC);
  return true;
}();
#endif
template <class Signed> WideString decimal_wide(Signed value) {
  char digits[std::numeric_limits<Signed>::digits10 + 2];
  const auto last = std::to_chars(digits, digits + sizeof(digits), value).ptr;
  auto result = WideString::uninitialized(Integer(last - digits));
  for (Integer i = 0; i < result.length(); ++i)
    result.data()[i] = char16_t(digits[i]);
  return result;
}
OutOfMemory *out_of_memory_object() noexcept {
  // Delphi also keeps one EOutOfMemory alive. The static ANSI message uses
  // Delphi's -1 refcount: copies neither retain nor free it; writes detach.
  static struct {
    Integer refs = -1, length = 13;
    std::uint8_t text[14] = "Out of memory";
  } storage;
  static OutOfMemory error = [] {
    OutOfMemory e;
    e.message = BorrowedValue<AnsiString>(storage.text).get();
    return e;
  }();
  return &error;
}
} // namespace

Raised::Raised(Object *p)
    : object(p == out_of_memory_object()
                 ? std::shared_ptr<Object>(std::shared_ptr<Object>{}, p)
                 : std::shared_ptr<Object>(p, [](Object *q) { pas::free(q); })) {}

const char *Raised::what() const noexcept { return "Delphi exception"; }

[[noreturn]] void raise(Object *exception) { throw Raised(exception); }

Object *caught_object() noexcept {
  try {
    throw;
  } catch (const Raised &error) {
    return error.object.get();
  } catch (const std::bad_alloc &) {
    return out_of_memory_object();
  } catch (...) {
    return nullptr;
  }
}

[[noreturn]] void raise_os_error(const AnsiString &message, Integer code) {
  auto *error = make_exception<OSError>(message);
  error->ErrorCode = code;
  raise(error);
}

[[noreturn]] void invalid_class_cast() {
  raise(construct<InvalidCast>([](InvalidCast *e) {
    e->p_create(AnsiString("Invalid class typecast"));
  }));
}

[[noreturn]] void invalid_integer(const AnsiString &s) {
  raise(construct<ConvertError>([&](ConvertError *e) {
    e->p_create(AnsiString("'") + s +
                AnsiString("' is not a valid integer value"));
  }));
}

[[noreturn]] void integer_divide_error() {
  raise(construct<DivByZero>(
      [](DivByZero *e) { e->p_create(AnsiString("Division by zero")); }));
}

[[noreturn]] void List::list_error(const char *message, Integer value) {
  auto *error = construct<ListError>([&](ListError *e) {
    e->p_create(AnsiString(message) + AnsiString(" (") + int_to_str(value) +
                AnsiString(")"));
  });
  raise(error);
}

WideString wide_int_to_str(std::int32_t value) { return decimal_wide(value); }
WideString wide_int64_to_str(std::int64_t value) { return decimal_wide(value); }

std::int64_t real_round(long double value) {
#pragma STDC FENV_ACCESS ON
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  std::int64_t result;
  // Delphi _ROUND converts directly under the current x87 rounding mode.
  asm volatile("fistpll %0" : "=m"(result) : "t"(value) : "st");
  if (result != std::numeric_limits<std::int64_t>::min() ||
      (fpu_control_word & 1))
    return result;
  // Integer indefinite and the valid INT64_MIN have the same bits. Only this
  // boundary needs the checked path when invalid-operation errors are enabled.
#endif
  return real_integer(value, false);
}

std::int64_t real_trunc(long double value) {
#pragma STDC FENV_ACCESS ON
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  std::int64_t result;
  std::uint16_t saved;
  asm volatile("fnstcw %0" : "=m"(saved));
  const std::uint16_t truncate = saved | 0x0f00;
  // _TRUNC temporarily selects full precision and rounding toward zero.
  // Restore the hardware word before the C++ invalid-operation path can throw.
  asm volatile("fldcw %1; fistpll %0; fldcw %2"
               : "=m"(result)
               : "m"(truncate), "m"(saved), "t"(value)
               : "st");
  if (result != std::numeric_limits<std::int64_t>::min() ||
      (fpu_control_word & 1))
    return result;
#endif
  return real_integer(value, true);
}

char16_t *WideString::alloc(Integer n, const char16_t *source, bool clear) {
  if (!n)
    return nullptr;
#ifdef _WIN32
  auto *p = SysAllocStringLen(source, n);
#else
  auto *h =
      static_cast<std::uint32_t *>(std::malloc(4 + 2 * (std::size_t(n) + 1)));
  if (!h)
    throw std::bad_alloc();
  *h = std::uint32_t(n) * 2;
  auto *p = reinterpret_cast<char16_t *>(h + 1);
  if (source) {
    // PWideChar can point into packed storage; copy from a byte-aligned address.
    std::memcpy(p, static_cast<const void *>(source), std::size_t(n) * sizeof(char16_t));
    p[n] = 0;
  }
#endif
  if (!p)
    throw std::bad_alloc();
  if (!source && clear)
    std::fill_n(p, n + 1, char16_t{});
  else
    p[n] = 0;
  return p;
}

void WideString::release(char16_t *p) noexcept {
#ifdef _WIN32
  SysFreeString(p);
#else
  if (p)
    std::free(reinterpret_cast<std::uint32_t *>(p) - 1);
#endif
}

WideString::WideString(const char16_t *p, Integer n) : data_(alloc(n, p)) {}

WideString::WideString(const WideString &s) : WideString(s.data_, s.length()) {}

WideString::~WideString() { release(data_); }

WideString WideString::uninitialized(Integer n) {
  if (n < 0)
    throw std::range_error("negative string length");
  WideString result;
  result.data_ = alloc(n, nullptr, false);
  return result;
}

void WideString::set_length(Integer n) {
  n = std::max(n, 0);
  auto *p = alloc(n);
  if (n && data_)
    std::copy_n(data_.get(), std::min(n, length()), p);
  release(data_);
  data_ = p;
}

std::uint8_t *AnsiString::alloc(Integer n, const std::uint8_t *source) {
  if (n < 0)
    throw std::range_error("negative ANSI string length");
  if (!n)
    return nullptr;
  auto *h =
      static_cast<Header *>(std::malloc(sizeof(Header) + std::size_t(n) + 1));
  if (!h)
    throw std::bad_alloc();
  *h = {1, n};
  auto *p = reinterpret_cast<std::uint8_t *>(h + 1);
  if (source)
    std::memcpy(p, source, std::size_t(n));
  p[n] = 0;
  return p;
}

AnsiString::AnsiString(const std::uint8_t *s, Integer n) : data_(alloc(n, s)) {}

AnsiString::AnsiString(const char16_t *s, Integer count) {
#ifdef _WIN32
  if (!count)
    return;
  int n = WideCharToMultiByte(ansi_code_page, 0, s, count, nullptr, 0, nullptr,
                              nullptr);
  if (!n)
    return;
  set_length(n);
  if (!WideCharToMultiByte(ansi_code_page, 0, s, count,
                          reinterpret_cast<char *>(data_.get()), n, nullptr,
                          nullptr))
    set_length(0);
#else
  set_length(count);
  for (Integer i = 0; i < length(); ++i)
    data_[i] = encode_ansi(load_unaligned<char16_t>(
        reinterpret_cast<const std::uint8_t *>(s) + std::size_t(i) * 2));
#endif
}

AnsiString::AnsiString(char16_t c) {
  if (c < 128) {
    set_length(1);
    data_[0] = std::uint8_t(c);
    return;
  }
#ifdef _WIN32
  int n = WideCharToMultiByte(ansi_code_page, 0, &c, 1, nullptr, 0, nullptr,
                              nullptr);
  if (!n)
    return;
  set_length(n);
  if (!WideCharToMultiByte(ansi_code_page, 0, &c, 1,
                          reinterpret_cast<char *>(data_.get()), n, nullptr,
                          nullptr))
    set_length(0);
#else
  set_length(1);
  data_[0] = encode_ansi(c);
#endif
}

AnsiString::AnsiString(std::uint8_t c) {
  set_length(1);
  data_[0] = c;
}

AnsiString::~AnsiString() { release(); }

AnsiString AnsiString::uninitialized(Integer n) {
  AnsiString result;
  result.data_ = alloc(n);
  return result;
}

void AnsiString::set_length(Integer n) {
  n = std::max(n, 0);
  auto *p = alloc(n);
  if (n) {
    const auto copied = std::min(n, length());
    if (copied)
      std::memcpy(p, data_.get(), std::size_t(copied));
    std::fill_n(p + copied, n - copied, std::uint8_t{});
  }
  release();
  data_ = p;
}

WideString::WideString(const AnsiString &s)
    : WideString(s.data(), s.length()) {}

WideString::WideString(const std::uint8_t *s, Integer count) {
#ifdef _WIN32
  if (!count)
    return;
  int n = MultiByteToWideChar(
      ansi_code_page, 0, reinterpret_cast<const char *>(s), count, nullptr, 0);
  if (!n)
    return;
  set_length(n);
  if (!MultiByteToWideChar(ansi_code_page, 0, reinterpret_cast<const char *>(s),
                          count, data_, n))
    set_length(0);
#else
  set_length(count);
  for (Integer i = 0; i < length(); ++i)
    data_[i] = decode_ansi(s[i]);
#endif
}

WideString::WideString(std::uint8_t c) : WideString(AnsiString(c)) {}

long double real_divide(long double left, long double right) {
#pragma STDC FENV_ACCESS ON
  if ((fpu_control_word & 0x0d) == 0x0d)
    return left / right;
#if LDBL_MANT_DIG == 64
  // Ordinary x87 operands cannot raise any of the three exceptions below.
  // Reading their representation avoids floating comparisons and preserves the
  // accumulated status flags. Exceptional/boundary encodings keep the RTL path.
  std::uint64_t left_bits, right_bits;
  std::uint16_t left_exp, right_exp;
  std::memcpy(&left_bits, &left, 8);
  std::memcpy(&right_bits, &right, 8);
  std::memcpy(&left_exp, reinterpret_cast<const char *>(&left) + 8, 2);
  std::memcpy(&right_exp, reinterpret_cast<const char *>(&right) + 8, 2);
  left_exp &= 0x7fff;
  right_exp &= 0x7fff;
  if (left_exp < 0x7fff && right_exp > 0 && right_exp < 0x7fff &&
      (left_exp == 0 || (left_bits >> 63)) && (right_bits >> 63) &&
      int(left_exp) - int(right_exp) < 0x3fff)
    return left / right;
#endif
  const int previous = std::fetestexcept(FE_ALL_EXCEPT);
  std::feclearexcept(FE_ALL_EXCEPT);
  const long double result = left / right;
  const int raised = std::fetestexcept(FE_ALL_EXCEPT);
  std::feclearexcept(FE_ALL_EXCEPT);
  if ((raised & FE_INVALID) && !(fpu_control_word & 1))
    raise(make_exception<InvalidOp>(
        AnsiString("Invalid floating point operation")));
  if ((raised & FE_DIVBYZERO) && !(fpu_control_word & 4))
    raise(make_exception<ZeroDivide>(
        AnsiString("Floating point division by zero")));
  if ((raised & FE_OVERFLOW) && !(fpu_control_word & 8))
    raise(make_exception<Overflow>(AnsiString("Floating point overflow")));
  std::feraiseexcept(previous | raised);
  return result;
}
} // namespace pas
