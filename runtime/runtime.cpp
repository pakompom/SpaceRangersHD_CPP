// Shared Delphi string ownership, conversions and exception paths.
#include "classes.hpp"
#include "reals.hpp"
#if defined(_WIN32) && !SRHD_NATIVE_REALS
#include <float.h>
#endif

namespace pas {
namespace {
#if defined(_WIN32) && !SRHD_NATIVE_REALS
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
using RaiseHook = void(PAS_STDCALL *)(Cardinal, Cardinal, Cardinal, void *);
void **raise_hook_slot = nullptr;
thread_local bool invoking_raise_hook = false;
[[noreturn]] void PAS_STDCALL default_raise_hook(Cardinal, Cardinal, Cardinal, void *payload) {
  throw *static_cast<const Raised *>(payload);
}
} // namespace

Raised::Raised(Object *p)
    : object(p == out_of_memory_object()
                 ? std::shared_ptr<Object>(std::shared_ptr<Object>{}, p)
                 : std::shared_ptr<Object>(p, [](Object *q) { pas::free(q); })) {}

const char *Raised::what() const noexcept { return "Delphi exception"; }

void install_raise_exception_hook(void *&hook) {
  // This is an authored game initialization seam, rather than an OS exception
  // record ABI: the wrapper only forwards the opaque payload to this adapter.
  if (!hook)
    hook = reinterpret_cast<void *>(&default_raise_hook);
  raise_hook_slot = &hook;
}

[[noreturn]] void raise(Object *exception) {
  Raised error(exception);
  if (raise_hook_slot && *raise_hook_slot && !invoking_raise_hook) {
    invoking_raise_hook = true;
    ScopeExit reset_hook = []() noexcept { invoking_raise_hook = false; };
    auto hook = reinterpret_cast<RaiseHook>(*raise_hook_slot);
    hook(0x0eedfade, 1, 1, &error);
  }
  // A returning observer cannot turn a Pascal raise into successful execution.
  throw error;
}

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

#if !SRHD_NATIVE_REALS
std::int64_t real_round(long double value) {
#pragma STDC FENV_ACCESS ON
#if LDBL_MANT_DIG == 113
  // Delphi ST0 has a 64-bit significand. Preserve the former by-value Extended
  // operand boundary on wider hosts; this narrowing is not PC emulation.
  return detail::extended_integer(Extended(value), false);
#else
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
#endif
}

std::int64_t real_trunc(long double value) {
#pragma STDC FENV_ACCESS ON
#if LDBL_MANT_DIG == 113
  // Keep the same binary80 operand boundary as Round before integer conversion.
  return detail::extended_integer(Extended(value), true);
#else
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
#endif
}

namespace {
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
void validate_binary80_integer(const Extended& value) {
  std::uint16_t encoded;
  std::memcpy(&encoded, value.bytes + 8, 2);
  // Unsupported integer-bit encodings are not necessarily classified as NaN
  // by the host library. Check their raw payload before an enabled error can
  // depend on that classification. Canonical inputs still use native FISTP.
  if ((encoded & 0x7fff) && !(value.bytes[7] & 0x80))
    raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
}
#else
// FISTP's integer result depends on RC, not the arithmetic precision control.
// Work from the exact field payload: converting to a narrower host real first
// can discard integer units or turn a just-in-range value into an overflow.
std::int64_t binary80_integer(const Extended& value, unsigned rounding) {
  std::uint64_t significand;
  std::uint16_t encoded;
  std::memcpy(&significand, value.bytes, 8);
  std::memcpy(&encoded, value.bytes + 8, 2);
  const bool negative = encoded & 0x8000;
  const unsigned stored_exponent = encoded & 0x7fff;
  const auto invalid = []() -> std::int64_t {
    if (!(fpu_control_word & 1))
      raise(make_exception<InvalidOp>(AnsiString("Invalid floating point operation")));
    return std::numeric_limits<std::int64_t>::min();
  };
  if (stored_exponent == 0x7fff || (stored_exponent && !(significand >> 63)))
    return invalid();
  if (!significand) return 0;
  const int exponent = int(stored_exponent ? stored_exponent : 1) - 16383;
  if (exponent > 63) return invalid();
  std::uint64_t magnitude = significand;
  bool fractional = false, above_half = false, at_half = false;
  if (exponent < 63) {
    const unsigned shift = unsigned(63 - exponent);
    if (shift > 64) {
      magnitude = 0;
      fractional = true;
    } else if (shift == 64) {
      magnitude = 0;
      fractional = true;
      above_half = significand > (1ull << 63);
      at_half = significand == (1ull << 63);
    } else {
      magnitude = significand >> shift;
      const auto remainder = significand & ((1ull << shift) - 1);
      fractional = remainder != 0;
      above_half = remainder > (1ull << (shift - 1));
      at_half = remainder == (1ull << (shift - 1));
    }
  }
  if ((rounding == 0 && (above_half || (at_half && (magnitude & 1))))
      || (rounding == 1 && negative && fractional)
      || (rounding == 2 && !negative && fractional)) ++magnitude;
  const auto limit = negative ? 0x8000000000000000ull : 0x7fffffffffffffffull;
  if (magnitude > limit) return invalid();
  if (negative && magnitude == (1ull << 63))
    return std::numeric_limits<std::int64_t>::min();
  return negative ? -std::int64_t(magnitude) : std::int64_t(magnitude);
}
#endif
}

std::int64_t detail::extended_integer(const Extended& value, bool truncate) {
#if LDBL_MANT_DIG == 64 && (defined(__i386__) || defined(__x86_64__))
  if (!(fpu_control_word & 1)) validate_binary80_integer(value);
  return truncate ? real_trunc(static_cast<long double>(value))
                  : real_round(static_cast<long double>(value));
#else
  return binary80_integer(value, truncate ? 3 : (fpu_control_word >> 10) & 3);
#endif
}

#endif

char16_t *WideString::alloc(Integer n, const char16_t *source, bool clear) {
  if (n < 0)
    throw std::range_error("negative wide string length");
  if (!n)
    return nullptr;
  if (std::size_t(n) > (std::numeric_limits<std::size_t>::max() - 4) / 2 - 1)
    throw std::bad_alloc();
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
    std::fill_n(p, std::size_t(n) + 1, char16_t{});
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
  if (count < 0)
    throw std::range_error("negative wide conversion length");
#ifdef _WIN32
  if (!count)
    return;
  int n = WideCharToMultiByte(ansi_code_page, 0, s, count, nullptr, 0, nullptr,
                              nullptr);
  if (!n)
    return;
  data_ = alloc(n);
  if (!WideCharToMultiByte(ansi_code_page, 0, s, count,
                          reinterpret_cast<char *>(data_.get()), n, nullptr,
                          nullptr))
    set_length(0);
#else
  auto result = AnsiString::uninitialized(count);
  for (Integer i = 0; i < count; ++i)
    result.data()[i] = encode_ansi(load_unaligned<char16_t>(
        reinterpret_cast<const std::uint8_t *>(s) + std::size_t(i) * 2));
  data_ = std::exchange(result.data_, nullptr);
#endif
}

AnsiString::AnsiString(char16_t c) {
  if (c < 128) {
    data_ = alloc(1);
    data_[0] = std::uint8_t(c);
    return;
  }
#ifdef _WIN32
  int n = WideCharToMultiByte(ansi_code_page, 0, &c, 1, nullptr, 0, nullptr,
                              nullptr);
  if (!n)
    return;
  data_ = alloc(n);
  if (!WideCharToMultiByte(ansi_code_page, 0, &c, 1,
                          reinterpret_cast<char *>(data_.get()), n, nullptr,
                          nullptr))
    set_length(0);
#else
  const auto encoded = encode_ansi(c);
  data_ = alloc(1);
  data_[0] = encoded;
#endif
}

AnsiString::AnsiString(std::uint8_t c) {
  data_ = alloc(1);
  data_[0] = c;
}

AnsiString::~AnsiString() { release(); }

AnsiString AnsiString::uninitialized(Integer n) {
  AnsiString result;
  result.data_ = alloc(n);
  return result;
}

void AnsiString::set_length(Integer n) {
  resize(n, true);
}

void AnsiString::resize(Integer n, bool clear_growth) {
  n = std::max(n, 0);
  if (!n) {
    release();
    return;
  }
  const auto old_length = length();
  if (data_ && header()->refs == 1) {
    if (n == old_length) {
      data_[n] = 0;
      return;
    }
    auto *resized = static_cast<Header *>(
        std::realloc(header(), sizeof(Header) + std::size_t(n) + 1));
    if (!resized)
      throw std::bad_alloc(); // realloc leaves the original owner intact.
    resized->length = n;
    data_ = reinterpret_cast<std::uint8_t *>(resized + 1);
    if (clear_growth && n > old_length)
      std::fill_n(data_.get() + old_length, n - old_length, std::uint8_t{});
    data_[n] = 0;
    return;
  }
  auto *p = alloc(n);
  const auto copied = std::min(n, old_length);
  if (copied)
    std::memcpy(p, data_.get(), std::size_t(copied));
  if (clear_growth)
    std::fill_n(p + copied, n - copied, std::uint8_t{});
  else if (n > old_length)
    p[old_length] = 0; // An internal append slice can include the old terminator.
  release();
  data_ = p;
}

void AnsiString::append(const std::uint8_t *source, Integer count) {
  if (count <= 0)
    return;
  const auto old_length = length();
  const auto new_length = string_length_sum(old_length, count);
  const auto base = reinterpret_cast<std::uintptr_t>(data_.get());
  const auto address = reinterpret_cast<std::uintptr_t>(source);
  const bool internal = data_ && address >= base && address - base <= std::size_t(old_length);
  const auto offset = internal ? address - base : 0;
  if (internal && std::size_t(count) > std::size_t(old_length) + 1 - offset)
    throw std::out_of_range("ANSI append slice exceeds source allocation");
  // The append completely writes growth. Retain the old terminator because it
  // can itself belong to the borrowed source slice, even after detachment.
  resize(new_length, false);
  if (internal)
    source = data_.get() + offset;
  std::memmove(data_.get() + old_length, source, std::size_t(count));
}

WideString::WideString(const AnsiString &s)
    : WideString(s.data(), s.length()) {}

WideString::WideString(const std::uint8_t *s, Integer count) {
  if (count < 0)
    throw std::range_error("negative ANSI conversion length");
#ifdef _WIN32
  if (!count)
    return;
  int n = MultiByteToWideChar(
      ansi_code_page, 0, reinterpret_cast<const char *>(s), count, nullptr, 0);
  if (!n)
    return;
  data_ = alloc(n, nullptr, false);
  if (!MultiByteToWideChar(ansi_code_page, 0, reinterpret_cast<const char *>(s),
                          count, data_, n))
    set_length(0);
#else
  auto result = WideString::uninitialized(count);
  for (Integer i = 0; i < count; ++i)
    result.data()[i] = decode_ansi(s[i]);
  data_ = std::exchange(result.data_, nullptr);
#endif
}

WideString::WideString(std::uint8_t c) : WideString(AnsiString(c)) {}

#if !SRHD_NATIVE_REALS
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
#endif
} // namespace pas
