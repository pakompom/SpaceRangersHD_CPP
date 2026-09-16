#pragma once
// Delphi 2007 Win32 semantic primitives. No emulated registers or machine IR.
#include "callables.hpp"
#include "class_info.hpp"
#include "interfaces.hpp"
#include "extended.hpp"
#include "pointer_slot.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace pas {
// Delphi PChar literals have static storage, despite their writable pointer type.
inline std::uint8_t* literal_pointer(const char* value) {
  return reinterpret_cast<std::uint8_t*>(const_cast<char*>(value));
}
inline char16_t* literal_pointer(const char16_t* value) {
  return const_cast<char16_t*>(value);
}
using Integer = std::int32_t;
using Cardinal = std::uint32_t;
// Source constants stay compile-time values, independent of runtime FPU mode.
template<class T> consteval T constant(T value) { return value; }
// Delphi loads a managed argument's pointer before taking value-parameter
// ownership at entry. Snapshot that pointer without retaining/copying its data.
// The union deliberately does not finalize this borrowed value.
template<class T> class BorrowedValue {
  union { T value_; };
public:
  explicit BorrowedValue(const T& source) : value_() { value_.data_ = source.data_; }
  template<class Element>
  explicit BorrowedValue(Element* data) noexcept : value_() { value_.data_ = data; }
  BorrowedValue(const BorrowedValue&) = delete;
  ~BorrowedValue() {}
  const T& get() const noexcept { return value_; }
};
template<class T> BorrowedValue<T> borrow(const T& value) { return BorrowedValue<T>(value); }
template <class T> void *mutable_address(T &value) {
  return std::addressof(value);
}
template <class T> const void *const_address(const T &value) {
  return std::addressof(value);
}
inline void fill_memory(void *dest, Integer count, std::uint8_t value) {
  if (count > 0)
    std::memset(dest, value, std::size_t(count));
}
inline void copy_memory(void *dest, const void *source, Cardinal count) {
  if (count) std::memmove(dest, source, std::size_t(count));
}
inline void move_memory(const void *source, void *dest, Integer count) {
  if (count > 0)
    std::memmove(dest, source, std::size_t(count));
}
// The supported Win32 target and native test host are little-endian. memcpy
// expresses the game's unaligned scalar accesses without C++ alias/alignment
// UB.
static_assert(std::endian::native == std::endian::little);
inline void *byte_offset(void *data, Integer offset) {
  return reinterpret_cast<void *>(reinterpret_cast<std::uintptr_t>(data) + std::intptr_t(offset));
}
template <class T> T load_unaligned(const void *source) {
  static_assert(std::is_trivially_copyable_v<T>);
  T value;
  std::memcpy(&value, source, sizeof(T));
  return value;
}
// Runtime UTF-16 equality compares whole words without a CRT call. Literal
// comparisons below retain their fixed extents for Clang's short-string folding.
// Never read beyond the payload, including for empty and embedded-null strings.
inline bool equal_wide(const char16_t* a, Integer a_size, const char16_t* b, Integer b_size) noexcept {
  if (a_size != b_size) return false;
  auto* left = reinterpret_cast<const std::uint8_t*>(a);
  auto* right = reinterpret_cast<const std::uint8_t*>(b);
  auto bytes = std::size_t(a_size) * sizeof(char16_t);
  while (bytes >= 8) {
    if (load_unaligned<std::uint64_t>(left) != load_unaligned<std::uint64_t>(right)) return false;
    left += 8; right += 8; bytes -= 8;
  }
  if (bytes >= 4) {
    if (load_unaligned<std::uint32_t>(left) != load_unaligned<std::uint32_t>(right)) return false;
    left += 4; right += 4; bytes -= 4;
  }
  return bytes == 0 || load_unaligned<std::uint16_t>(left) == load_unaligned<std::uint16_t>(right);
}
// A typed Pascal var argument whose original address need not be aligned.
// Reads and writes remain at their source positions, including across callbacks.
template<class T> struct Var { void* address; };
template<class T> struct ConstRef { const void* address; };
template<class T> Integer index_in_slots(const T* data, std::uint32_t count, T value) {
  const auto* bytes = reinterpret_cast<const std::uint8_t*>(data);
  for (std::uint32_t i = 0; i < count; ++i)
    if (load_unaligned<T>(bytes + i * sizeof(T)) == value) return Integer(i);
  return -1;
}
template <class T> void store_unaligned(void *dest, const T &value) {
  static_assert(std::is_trivially_copyable_v<T>);
  std::memcpy(dest, &value, sizeof(T));
}
template <class T, class Step> void inc_unaligned(void *slot, Step step) {
  auto value = load_unaligned<T>(slot);
  if constexpr (std::is_enum_v<T>) value = T(value + step);
  else value += step;
  store_unaligned(slot, value);
}
template <class T, class Step> void dec_unaligned(void *slot, Step step) {
  auto value = load_unaligned<T>(slot);
  if constexpr (std::is_enum_v<T>) value = T(value - step);
  else value -= step;
  store_unaligned(slot, value);
}
template <class Char> Integer terminated_length(const Char *text) {
  if (!text)
    return 0;
  if constexpr (sizeof(Char) == 1) {
    return Integer(std::strlen(reinterpret_cast<const char *>(text)));
  } else {
    auto *bytes = reinterpret_cast<const std::uint8_t *>(text);
    Integer count = 0;
    while (load_unaligned<Char>(bytes + std::size_t(count) * sizeof(Char)))
      ++count;
    return count;
  }
}
// Fixed Delphi character arrays stop at NUL or their declared bound.
// The pointer may refer to an unaligned packed record member.
template <class String, class Char> String array_text(const Char *text, Integer capacity) {
  Integer count = 0;
  const auto *bytes = reinterpret_cast<const std::uint8_t *>(text);
  while (count < capacity && load_unaligned<Char>(bytes + std::size_t(count) * sizeof(Char)))
    ++count;
  return String(text, count);
}
// Port of scalar readers whose asm advances the cursor before the load.
// Snapshot the address first: data can alias the cursor's own storage.
template <class T> T read_and_advance(void *data, Integer *position) {
  void *source = byte_offset(data, *position);
  *position += Integer(sizeof(T));
  return load_unaligned<T>(source);
}
enum class FinallyFlow { Normal, Return, Break, Continue };
struct ControlFlow {};
struct Exit : ControlFlow {};
struct Break : ControlFlow {};
struct Continue : ControlFlow {};
// Only for cleanup proven not to throw. The callable captures references so
// operands are evaluated at scope exit, not when the guard is constructed.
template<class Cleanup> class ScopeExit {
  Cleanup cleanup;
public:
  ScopeExit(Cleanup action) noexcept(std::is_nothrow_move_constructible_v<Cleanup>) : cleanup(std::move(action)) {
    static_assert(std::is_nothrow_invocable_v<Cleanup>);
  }
  ScopeExit(const ScopeExit&) = delete;
  ScopeExit& operator=(const ScopeExit&) = delete;
  // Inline the guard and its callable; neither should add a wrapper call on
  // frequently executed cleanup paths such as image-cache releases.
  [[gnu::always_inline]] ~ScopeExit() noexcept {
#ifdef __clang__
    [[clang::always_inline]] cleanup();
#else
    cleanup();
#endif
  }
};
// Cleanup runs outside a C++ unwinding destructor, so a cleanup exception can
// replace the pending exception, just as Delphi does.
template <class Body, class Cleanup> void finally(Body body, Cleanup cleanup) {
  std::exception_ptr pending;
  try {
    body();
  } catch (...) {
    pending = std::current_exception();
  }
  cleanup();
  if (pending)
    std::rethrow_exception(pending);
}

template <class T, Integer Lo, Integer Hi> struct Array {
  static_assert(Hi >= Lo);
  T elements[Hi - Lo + 1];
  T &operator[](Integer i) { return elements[i - Lo]; }
  const T &operator[](Integer i) const { return elements[i - Lo]; }
  static constexpr Integer low = Lo, high = Hi;
  static constexpr Integer length() { return Hi - Lo + 1; }
};
template <class T> class DynArray {
  // Delphi data pointer, preceded by refcount and length. One pointer in
  // records.
  struct Header {
    Integer refs, length;
  };
  static_assert(alignof(T) <= alignof(std::uint64_t));
  PointerSlot<T> data_;
  template<class> friend class BorrowedValue;
  Header *header() const { return reinterpret_cast<Header *>(data_.get()) - 1; }
  void release() {
    if (data_ && --header()->refs == 0) {
      std::destroy_n(data_.get(), header()->length);
      std::free(header());
    }
    data_ = nullptr;
  }

public:
  DynArray() = default;
  DynArray(std::nullptr_t) noexcept {}
  DynArray(const DynArray &other) : data_(other.data_) {
    if (data_)
      ++header()->refs;
  }
  DynArray(DynArray &&other) noexcept
      : data_(std::exchange(other.data_, nullptr)) {}
  DynArray &operator=(DynArray other) {
    std::swap(data_, other.data_);
    return *this;
  }
  DynArray &operator=(std::nullptr_t) {
    release();
    return *this;
  }
  ~DynArray() { release(); }
  Integer length() const { return data_ ? header()->length : 0; }
  T *data() const { return data_; }
  // A const array parameter still allows writes to its shared elements in
  // Delphi.
  T &operator[](Integer i) const { return data_[i]; }
  void set_length(Integer n) {
    if (n < 0)
      throw std::range_error("negative dynamic array length");
    if (n == 0) {
      release();
      return;
    }
    if (data_ && header()->refs == 1 && n == length())
      return;
    auto bytes = sizeof(Header) + std::size_t(n) * sizeof(T);
    if constexpr (std::is_trivially_copyable_v<T> && std::is_nothrow_default_constructible_v<T>) {
      if (!data_ || header()->refs == 1) {
        const Integer old_length = length();
        auto *h = static_cast<Header *>(std::realloc(data_ ? header() : nullptr, bytes));
        if (!h) throw std::bad_alloc();
        *h = {1, n};
        auto *fresh = reinterpret_cast<T *>(h + 1);
        // Existing elements keep their values; only the exposed tail is new.
        if (n > old_length) std::uninitialized_value_construct_n(fresh + old_length, n - old_length);
        data_ = fresh;
        return;
      }
    }
    auto *h = static_cast<Header *>(std::malloc(bytes));
    if (!h)
      throw std::bad_alloc();
    *h = {1, n};
    auto *fresh = reinterpret_cast<T *>(h + 1);
    const Integer retained = std::min(n, length());
    Integer made = 0;
    try {
      if constexpr (std::is_trivially_copyable_v<T>) {
        if (retained) std::memcpy(fresh, data_.get(), std::size_t(retained) * sizeof(T));
        made = retained;
      } else {
        const bool move = data_ && header()->refs == 1 && std::is_nothrow_move_constructible_v<T>;
        for (; made < retained; ++made) {
          if (move)
            std::construct_at(fresh + made, std::move(data_[made]));
          else
            std::construct_at(fresh + made, data_[made]);
        }
      }
      for (; made < n; ++made)
        std::construct_at(fresh + made);
    } catch (...) {
      std::destroy_n(fresh, made);
      std::free(h);
      throw;
    }
    release();
    data_ = fresh;
  }
  friend bool operator==(const DynArray &a, std::nullptr_t) { return !a.data_; }
};

// A Windows WideString is a deep-copy BSTR. The portable probe allocator keeps
// the same length prefix; Win32 uses OleAut32's allocator for COM
// interoperability.
#ifdef _WIN32
extern "C" __declspec(dllimport) char16_t *__stdcall
SysAllocStringLen(const char16_t *, unsigned int) noexcept;
extern "C" __declspec(dllimport) void __stdcall SysFreeString(char16_t *) noexcept;
#endif
class AnsiString;
class WideString {
  PointerSlot<char16_t> data_;
  template<class> friend class BorrowedValue;
  static char16_t *alloc(Integer n, const char16_t *source = nullptr, bool clear = true);
  static void release(char16_t *p) noexcept;

public:
  WideString() = default;
  WideString(const AnsiString &);
  WideString(const std::uint8_t *, Integer);
  WideString(const std::uint8_t *p) : WideString(p, terminated_length(p)) {}
  WideString(std::uint8_t);
  // Keep allocating/copying and finalization paths shared across game callers.
  WideString(const char16_t *p, Integer n);
  WideString(const char16_t *p)
      : WideString(p, terminated_length(p)) {}
  WideString(char16_t c) : WideString(&c, 1) {}
  WideString(const WideString &s);
  WideString(WideString &&s) noexcept
      : data_(std::exchange(s.data_, nullptr)) {}
  WideString &operator=(WideString s) noexcept {
    std::swap(data_, s.data_);
    return *this;
  }
  ~WideString();
  Integer length() const {
    return data_ ? Integer(reinterpret_cast<std::uint32_t *>(data_.get())[-1] / 2)
                 : 0;
  }
  char16_t *data() const { return data_; }
  char16_t *pchar() const {
    static char16_t empty[1]{};
    return data_ ? data_.get() : empty;
  }
  const char16_t *c_str() const { return data_ ? data_.get() : u""; }
  char16_t read(Integer i) const { return data_[i - 1]; }
  char16_t &write(Integer i) { return data_[i - 1]; }
  char16_t &operator[](Integer i) { return data_[i - 1]; }
  const char16_t &operator[](Integer i) const { return data_[i - 1]; }
  // For builders that overwrite every character before exposing the string.
  static WideString uninitialized(Integer n);
  void set_length(Integer n);
  friend WideString operator+(const WideString &a, const WideString &b) {
    WideString s;
    s.set_length(a.length() + b.length());
    if (a.length())
      std::copy_n(a.data_.get(), a.length(), s.data_.get());
    if (b.length())
      std::copy_n(b.data_.get(), b.length(), s.data_.get() + a.length());
    return s;
  }
  friend auto operator<=>(const WideString &a, const WideString &b) {
    return std::u16string_view(a.c_str(), a.length()) <=>
           std::u16string_view(b.c_str(), b.length());
  }
  friend bool operator==(const WideString &a, const WideString &b) {
    return equal_wide(a.data(), a.length(), b.data(), b.length());
  }
};

// A Delphi const WideString literal is a static length-prefixed payload, not an
// owned BSTR. The template parameter supplies static storage, including embedded
// nulls. The borrowed pointer slot lives through the enclosing call; copying it
// into an owning WideString still performs the normal deep copy.
template<std::size_t N> struct WideLiteral {
  std::uint32_t bytes = (N - 1) * sizeof(char16_t);
  char16_t text[N];
  consteval WideLiteral(const char16_t (&value)[N]) {
    std::copy_n(value, N, text);
  }
};
namespace literals {
template<WideLiteral Text> auto operator""_wref() noexcept {
  return BorrowedValue<WideString>(Text.bytes ? const_cast<char16_t*>(Text.text) : nullptr);
}
inline WideString operator""_w(const char16_t* text, std::size_t length) {
  return WideString(text, Integer(length));
}
}

template <Integer Lo, Integer Hi> struct Set {
  static_assert(Lo >= 0 && Hi <= 255 && Hi >= Lo);
  // DCC32 rounds three storage bytes up to four. Nonzero lower bounds use byte
  // bias.
  static constexpr int bytes =
      (Hi / 8 - Lo / 8 + 1) == 3 ? 4 : Hi / 8 - Lo / 8 + 1;
  std::array<std::uint8_t, bytes> bits{};
  constexpr Set() = default;
  template<Integer OtherLo, Integer OtherHi> constexpr Set(const Set<OtherLo, OtherHi>& other) {
    for (int i = 0; i < bytes; ++i) {
      int source = i + Lo / 8 - OtherLo / 8;
      if (source >= 0 && source < other.bytes) bits[i] = other.bits[source];
    }
  }
  constexpr Set(std::initializer_list<Integer> values) {
    for (auto v : values)
      include(v);
  }
  constexpr void include(Integer i) {
    if (i >= Lo && i <= Hi)
      bits[i / 8 - Lo / 8] |= std::uint8_t(1u << (i % 8));
  }
  constexpr void exclude(Integer i) {
    if (i >= Lo && i <= Hi)
      bits[i / 8 - Lo / 8] &= std::uint8_t(~(1u << (i % 8)));
  }
  constexpr bool contains(Integer i) const {
    return i >= Lo && i <= Hi && (bits[i / 8 - Lo / 8] & (1u << (i % 8)));
  }
  friend bool operator==(const Set &, const Set &) = default;
  friend bool operator<=(const Set &a, const Set &b) {
    for (int i = 0; i < bytes; ++i)
      if ((a.bits[i] & ~b.bits[i]) != 0)
        return false;
    return true;
  }
  friend bool operator>=(const Set &a, const Set &b) { return b <= a; }
  friend Set operator+(Set a, const Set &b) {
    for (int i = 0; i < bytes; ++i)
      a.bits[i] |= b.bits[i];
    return a;
  }
  friend Set operator-(Set a, const Set &b) {
    for (int i = 0; i < bytes; ++i)
      a.bits[i] &= ~b.bits[i];
    return a;
  }
  friend Set operator*(Set a, const Set &b) {
    for (int i = 0; i < bytes; ++i)
      a.bits[i] &= b.bits[i];
    return a;
  }
};
struct SetRange {
  Integer low, high;
  constexpr SetRange(Integer value) : low(value), high(value) {}
  constexpr SetRange(Integer low, Integer high) : low(low), high(high) {}
};
template<class SetType> constexpr SetType make_set(std::initializer_list<SetRange> ranges) {
  SetType result;
  for (const auto& range : ranges)
    for (Integer i = std::max(range.low, 0); i <= std::min(range.high, 255); ++i) result.include(i);
  return result;
}
template<class SetType> consteval SetType constant_set(std::initializer_list<SetRange> ranges) {
  return make_set<SetType>(ranges);
}
template<class SetType, class Ordinal> bool contains(const SetType& set, Ordinal value) { return set.contains(Integer(value)); }
template<class Ordinal> bool in_range(Ordinal value, Integer low, Integer high) { return value >= low && value <= high; }
// Constant membership needs no temporary set. Bounds are normalized by lowering,
// and the function parameter evaluates the tested expression exactly once.
template<Integer Low, Integer High, Integer... Rest>
constexpr bool in_set(Integer value) noexcept {
  const bool here = Cardinal(value) - Cardinal(Low) <= Cardinal(High - Low);
  if constexpr (sizeof...(Rest) == 0) return here;
  else return here || in_set<Rest...>(value);
}
template<Integer... Values>
constexpr bool is_one_of(Integer value) noexcept {
  return ((value == Values) || ...);
}
struct Object {
  using pas_parent = void;
  static constexpr const char *pas_name = "TObject";
  static constexpr std::uint32_t pas_size = 4;
  virtual const ClassInfo *p_class_type() const {
    return &class_info<Object>();
  }
  virtual ~Object() = default;
  void p_create() {}
  virtual void p_destroy() {}
};
template <class T, class Init> T *construct(Init init) {
  register_class<T>();
  auto *value = new T();
  try {
    init(value);
  } catch (...) {
    auto error = std::current_exception();
    try {
      value->p_destroy();
    } catch (...) {
      delete value;
      throw;
    }
    delete value;
    std::rethrow_exception(error);
  }
  return value;
}
// Constructor formal types materialize value parameters before allocation.
// Forward them into the initializer without a second managed-string copy.
template <class T, class Self, class... Args>
T *construct_call(void (*init)(Self *, Args...), std::type_identity_t<Args>... args) {
  return construct<T>([&](T *value) { init(value, std::forward<Args>(args)...); });
}
inline void object_create(Object *) {}
inline void object_destroy(Object *) {}
template <class T> T *make_object() {
  return construct_call<T>(object_create);
}
template <class T, class Init> T *construct_from(ClassRef<T> type, Init init) {
  auto *value = type.allocate();
  try {
    init(value);
  } catch (...) {
    auto error = std::current_exception();
    try {
      value->p_destroy();
    } catch (...) {
      delete value;
      throw;
    }
    delete value;
    std::rethrow_exception(error);
  }
  return value;
}
inline ClassRef<Object> class_type(const Object *value) {
  if (!value)
    throw std::runtime_error("ClassType on nil object");
  return ClassRef<Object>(value->p_class_type());
}
template <class To> To class_cast_if(Object *value) {
  if (!value)
    return nullptr;
  auto *target = &class_info<std::remove_pointer_t<To>>();
  for (auto *type = value->p_class_type(); type; type = type->parent)
    if (type == target)
      return static_cast<To>(value);
  return nullptr;
}
template <class T, class Destroy> void destroy(T *p, Destroy cleanup) {
  try {
    cleanup(p);
  } catch (...) {
    delete p;
    throw;
  }
  delete p;
}
template <class T> void destroy(T *p) {
  destroy(p, [](auto *value) { value->p_destroy(); });
}
template <class T> void free(T *p) {
  if (p) destroy(p);
}

template <class T> Integer length(const T &v) { return v.length(); }
template <class T> Integer high(const T &v) { return length(v) - 1; }
template <class T, Integer L, Integer H>
constexpr Integer high(const Array<T, L, H> &) {
  return H;
}
template <class T> constexpr Integer low(const T &) { return 0; }
template <class T, Integer L, Integer H>
constexpr Integer low(const Array<T, L, H> &) {
  return L;
}
inline Integer low(const WideString &) { return 1; }
inline Integer high(const WideString &v) { return v.length(); }
template <class T> void set_length(T &v, Integer n) { v.set_length(n); }
template <class T, class... D>
void set_length(DynArray<T> &v, Integer n, D... dims) {
  v.set_length(n);
  if constexpr (sizeof...(dims))
    for (Integer i = 0; i < n; ++i)
      set_length(v[i], dims...);
}
// Delphi New/GetMem and Dispose/FreeMem use the same heap. The game mixes
// GetMem with Dispose for unmanaged geometry records.
template <class T> void new_value(T *&p) {
  auto *storage = static_cast<T *>(std::malloc(sizeof(T)));
  if (!storage)
    throw std::bad_alloc();
  try {
    p = std::construct_at(storage);
  } catch (...) {
    std::free(storage);
    throw;
  }
}
template <class T> void dispose(T *p) {
  if (p) {
    std::destroy_at(p);
    std::free(p);
  }
} // Delphi Dispose does not nil the variable.
template <class T, class N> void inc(T &p, N n) {
  if constexpr (std::is_enum_v<T>)
    p = T(p + n);
  else
    p += n;
}
// Untyped Dispose carries no finalization type; Delphi releases only the block.
inline void dispose(void* p) { std::free(p); }
template <class T> void inc(T &p) { inc(p, 1); }
template <class T, class N> void dec(T &p, N n) {
  if constexpr (std::is_enum_v<T>)
    p = T(p - n);
  else
    p -= n;
}
template <class T> void dec(T &p) { dec(p, 1); }
template <class T> T *pointer(const DynArray<T> &a) { return a.data(); }
inline char16_t *pointer(const WideString &s) { return s.data(); }
template <class T> T *pointer(T *p) { return p; }
template <class T>
void *pointer(T n)
  requires std::is_integral_v<T>
{
  return reinterpret_cast<void *>(std::uintptr_t(n));
}
// Shifts are logical even for Delphi signed ordinals; x86 masks the count.
template <class T, class U> auto shr(T a, U b) {
  using R = std::conditional_t<(sizeof(T) < 4), Integer, T>;
  return R(std::make_unsigned_t<R>(a) >> (unsigned(b) & (sizeof(R) * 8 - 1)));
}
template <class T, class U> auto shl(T a, U b) {
  using R = std::conditional_t<(sizeof(T) < 4), Integer, T>;
  return R(std::make_unsigned_t<R>(a) << (unsigned(b) & (sizeof(R) * 8 - 1)));
}
[[noreturn]] void integer_divide_error();
template <class T, class U> auto idiv(T a, U b) {
  using R = decltype(a / b);
  if (b == 0)
    integer_divide_error();
  if constexpr (std::is_signed_v<R>)
    if (R(a) == std::numeric_limits<R>::min() && R(b) == -1)
      integer_divide_error();
  return a / b;
}
template <class T, class U> auto imod(T a, U b) {
  (void)idiv(a, b);
  return a % b;
}
inline void write_boolean(std::uint8_t value) {
  if (value > 1)
    throw std::runtime_error("Delphi Write(Boolean) indexes outside its string "
                             "table for noncanonical bytes");
  std::fputs(value ? "TRUE" : "FALSE", stdout);
}
template <class... T> void write(const T &...values) {
  auto write = [](const auto &v) {
    using V = std::decay_t<decltype(v)>;
    if constexpr (is_real_v<V>)
      std::printf("%.12g", double(v));
    else if constexpr (std::is_integral_v<V>) {
      if constexpr (std::is_signed_v<V>)
        std::printf("%lld", (long long)v);
      else
        std::printf("%llu", (unsigned long long)v);
    } else {
      for (Integer i = 1; i <= v.length(); ++i)
        std::putchar(v[i] < 128 ? char(v[i]) : '?');
    }
  };
  (write(values), ...);
}
} // namespace pas

namespace pas {
template <class T> auto sqr(T value) { return value * value; }
template <class T> auto abs(T value) { return value < 0 ? -value : value; }
template <class T> std::int64_t trunc(T value) {
  auto result = std::trunc(static_cast<long double>(value));
  if (!std::isfinite(result) || result < -9223372036854775808.0L ||
      result >= 9223372036854775808.0L)
    return std::numeric_limits<std::int64_t>::min();
  return std::int64_t(result);
}
template <class T> std::int64_t round(T value) {
  auto result = std::nearbyint(static_cast<long double>(value));
  if (!std::isfinite(result) || result < -9223372036854775808.0L ||
      result >= 9223372036854775808.0L)
    return std::numeric_limits<std::int64_t>::min();
  return std::int64_t(result);
}
template <class T> bool assigned(T *value) { return value != nullptr; }
template <class T> Integer ord(T value) { return Integer(value); }
template <class T> std::uint8_t chr(T value) { return std::uint8_t(value); }
template <class T, class U> void include(T &set, U value) {
  set.include(Integer(value));
}
template <class T, class U> void include_at(T* set, U value) { set->include(Integer(value)); }
template <class T, class U> void exclude_at(T* set, U value) { set->exclude(Integer(value)); }
template <class T, class U> void exclude(T &set, U value) {
  set.exclude(Integer(value));
}
} // namespace pas

namespace pas {
inline constexpr char16_t cp1251_high[128] = {
    0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021, 0x20ac,
    0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f, 0x0452, 0x2018,
    0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x0098, 0x2122, 0x0459,
    0x203a, 0x045a, 0x045c, 0x045b, 0x045f, 0x00a0, 0x040e, 0x045e, 0x0408,
    0x00a4, 0x0490, 0x00a6, 0x00a7, 0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac,
    0x00ad, 0x00ae, 0x0407, 0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5,
    0x00b6, 0x00b7, 0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455,
    0x0457, 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, 0x0420,
    0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429,
    0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f, 0x0430, 0x0431, 0x0432,
    0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043a, 0x043b,
    0x043c, 0x043d, 0x043e, 0x043f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0444,
    0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d,
    0x044e, 0x044f,
};
inline char16_t decode_cp1251(std::uint8_t c) {
  return c < 128 ? char16_t(c) : cp1251_high[c - 128];
}
inline std::uint8_t encode_cp1251(char16_t c) {
  if (c < 128)
    return std::uint8_t(c);
  for (int i = 0; i < 128; ++i)
    if (cp1251_high[i] == c)
      return std::uint8_t(i + 128);
  return '?';
}
// Source literals use CP1251; runtime conversions use Delphi's system code
// page.
#ifdef _WIN32
extern "C" __declspec(dllimport) int __stdcall
MultiByteToWideChar(unsigned, unsigned, const char *, int, char16_t *, int);
extern "C" __declspec(dllimport) int __stdcall
WideCharToMultiByte(unsigned, unsigned, const char16_t *, int, char *, int,
                    const char *, int *);
// CP_ACP asks NLS for the process ANSI code page, including during C++ global
// initialization. No dynamic initializer or per-translation-unit guard is needed.
inline constinit unsigned ansi_code_page = 0;
#else
// Native tests emulate the current Wine reference's ACP. The game target uses
// NLS.
inline constinit unsigned ansi_code_page = 1252;
#endif
#ifndef _WIN32
inline constexpr char16_t cp1252_controls[32] = {
    0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021,
    0x2c6,  0x2030, 0x160,  0x2039, 0x152,  0x8d,   0x17d,  0x8f,
    0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
    0x2dc,  0x2122, 0x161,  0x203a, 0x153,  0x9d,   0x17e,  0x178};
inline char16_t decode_ansi(std::uint8_t c) {
  if (ansi_code_page == 1251)
    return decode_cp1251(c);
  if (ansi_code_page == 1252)
    return c >= 128 && c < 160 ? cp1252_controls[c - 128] : char16_t(c);
  throw std::invalid_argument("native probe code page unsupported");
}
inline std::uint8_t encode_ansi(char16_t c) {
  if (ansi_code_page == 1251)
    return encode_cp1251(c);
  if (ansi_code_page == 1252) {
    if (c < 128 || (c >= 160 && c <= 255))
      return std::uint8_t(c);
    for (int i = 0; i < 32; ++i)
      if (cp1252_controls[i] == c)
        return std::uint8_t(i + 128);
    return '?';
  }
  throw std::invalid_argument("native probe code page unsupported");
}
#endif
class AnsiString {
  template<class> friend class BorrowedValue;
  struct Header {
    Integer refs, length;
  };
  PointerSlot<std::uint8_t> data_;
  Header *header() const { return reinterpret_cast<Header *>(data_.get()) - 1; }
  static std::uint8_t *alloc(Integer n, const std::uint8_t *source = nullptr);
  void release() {
    if (data_ && header()->refs > 0 && --header()->refs == 0)
      std::free(header());
    data_ = nullptr;
  }
  void unique() {
    if (data_ && header()->refs != 1)
      set_length(length());
  }

public:
  AnsiString() = default;
  AnsiString(const std::uint8_t *s, Integer n);
  AnsiString(const char *s, Integer n)
      : AnsiString(reinterpret_cast<const std::uint8_t *>(s), n) {}
  AnsiString(const std::uint8_t *s) : AnsiString(s, terminated_length(s)) {}
  AnsiString(const char *s)
      : AnsiString(reinterpret_cast<const std::uint8_t *>(s),
                   terminated_length(s)) {}
  AnsiString(const WideString &s) : AnsiString(s.data(), s.length()) {}
  AnsiString(const char16_t *s) : AnsiString(s, terminated_length(s)) {}
  AnsiString(const char16_t *s, Integer count);
  AnsiString(char16_t c);
  AnsiString(std::uint8_t c);
  AnsiString(const AnsiString &s) noexcept : data_(s.data_) {
    if (data_ && header()->refs > 0)
      ++header()->refs;
  }
  AnsiString(AnsiString &&s) noexcept
      : data_(std::exchange(s.data_, nullptr)) {}
  AnsiString &operator=(AnsiString s) noexcept {
    std::swap(data_, s.data_);
    return *this;
  }
  ~AnsiString();
  Integer length() const { return data_ ? header()->length : 0; }
  std::uint8_t *data() const { return data_; }
  std::uint8_t *pchar() const {
    static std::uint8_t empty[1]{};
    return data_ ? data_.get() : empty;
  }
  const char *c_str() const {
    return data_ ? reinterpret_cast<const char *>(data_.get()) : "";
  }
  std::uint8_t read(Integer i) const { return data_[i - 1]; }
  std::uint8_t &write(Integer i) {
    unique();
    return data_[i - 1];
  }
  std::uint8_t operator[](Integer i) const { return read(i); }
  static AnsiString uninitialized(Integer n);
  void set_length(Integer n);
  friend AnsiString operator+(const AnsiString &a, const AnsiString &b) {
    AnsiString s;
    s.set_length(a.length() + b.length());
    if (a.length())
      std::copy_n(a.data_.get(), a.length(), s.data_.get());
    if (b.length())
      std::copy_n(b.data_.get(), b.length(), s.data_.get() + a.length());
    return s;
  }
  friend auto operator<=>(const AnsiString &a, const AnsiString &b) {
    return std::string_view(a.c_str(), a.length()) <=>
           std::string_view(b.c_str(), b.length());
  }
  friend bool operator==(const AnsiString &a, const AnsiString &b) {
    return (a <=> b) == 0;
  }
};
namespace literals {
inline AnsiString operator""_a(const char* text, std::size_t length) {
  return AnsiString(text, Integer(length));
}
}
// Borrow string operands without allocating, including embedded-null literals.
inline std::u16string_view view(const WideString &s) { return {s.c_str(), std::size_t(s.length())}; }
inline std::string_view view(const AnsiString &s) { return {s.c_str(), std::size_t(s.length())}; }
template<class Char, std::size_t N>
std::basic_string_view<Char> view(const Char (&literal)[N]) { return {literal, N-1}; }
template<std::size_t N>
bool operator==(const WideString &s, const char16_t (&literal)[N]) { return view(s) == view(literal); }
template<std::size_t N>
auto operator<=>(const WideString &s, const char16_t (&literal)[N]) { return view(s) <=> view(literal); }
template<std::size_t N>
bool operator==(const AnsiString &s, const char (&literal)[N]) { return view(s) == view(literal); }
template<std::size_t N>
auto operator<=>(const AnsiString &s, const char (&literal)[N]) { return view(s) <=> view(literal); }
template<class Source>
auto copy(const Source &source, Integer index, Integer count) {
  auto text = view(source);
  using Result = std::conditional_t<std::is_same_v<typename decltype(text)::value_type, char16_t>, WideString, AnsiString>;
  index = std::max(1, index);
  count = std::max(0, std::min(count, Integer(text.size()) - index + 1));
  return count ? Result(text.data() + index - 1, count) : Result();
}
template<class Needle, class Haystack>
Integer pos(const Needle &needle, const Haystack &haystack) {
  auto n = view(needle), h = view(haystack);
  if (n.empty()) return 0;
  auto offset = h.find(n);
  return offset == decltype(h)::npos ? 0 : Integer(offset + 1);
}
inline bool compare_memory(const void* left, const void* right, Integer count) {
  return count <= 0 || std::memcmp(left, right, std::size_t(count)) == 0;
}
inline char16_t wide_character(std::uint8_t value) {
  if (value < 128) return char16_t(value);
#ifdef _WIN32
  char byte = char(value);
  char16_t result{};
  if (MultiByteToWideChar(ansi_code_page, 0, &byte, 1, &result, 1) != 1)
    throw std::runtime_error("MultiByteToWideChar failed");
  return result;
#else
  return decode_ansi(value);
#endif
}

// Borrow concatenation inputs; braced argument lists evaluate left to right.
// Returned string temporaries stay alive through the enclosing full expression.
template <class String, class Char> struct StringPart {
  const Char *data;
  Integer size;
  Char character{};
  // Keep scalar characters inline; copies never point into another part.
  StringPart(Char value) : data(nullptr), size(1), character(value) {}
  StringPart(const String &value)
      : data(reinterpret_cast<const Char *>(value.data())), size(value.length()) {}
  template <std::size_t N> StringPart(const Char (&value)[N])
      : data(value), size(Integer(N - 1)) {}
};
template <class String, class Char>
String concat_parts(std::initializer_list<StringPart<String, Char>> parts, bool reverse = false) {
  Integer size = 0;
  for (auto part : parts) size += part.size;
  auto result = String::uninitialized(size);
  auto *cursor = result.data();
  for (std::size_t i = 0; i < parts.size(); ++i) {
    auto part = parts.begin()[reverse ? parts.size() - i - 1 : i];
    if (part.size) {
      std::memcpy(cursor, part.data ? part.data : &part.character, std::size_t(part.size) * sizeof(Char));
      cursor += part.size;
    }
  }
  return result;
}
inline AnsiString concat_ansi(std::initializer_list<StringPart<AnsiString, char>> parts) {
  return concat_parts<AnsiString, char>(parts);
}
inline WideString concat_wide(std::initializer_list<StringPart<WideString, char16_t>> parts) {
  return concat_parts<WideString, char16_t>(parts);
}
// Delphi's two-operand concat prepares the right argument first. The generated
// initializer list follows that order; these helpers restore output order.
inline AnsiString concat_ansi_reverse(std::initializer_list<StringPart<AnsiString, char>> parts) {
  return concat_parts<AnsiString, char>(parts, true);
}
inline WideString concat_wide_reverse(std::initializer_list<StringPart<WideString, char16_t>> parts) {
  return concat_parts<WideString, char16_t>(parts, true);
}



template <Integer L, Integer H>
AnsiString string_from_array(const Array<std::uint8_t, L, H> &value) {
  Integer count = 0;
  while (count < value.length() && value.elements[count])
    ++count;
  return AnsiString(value.elements, count);
}
template <Integer L, Integer H>
WideString string_from_array(const Array<char16_t, L, H> &value) {
  Integer count = 0;
  while (count < value.length() && value.elements[count])
    ++count;
  return WideString(value.elements, count);
}
inline std::uint8_t *pointer(const AnsiString &s) { return s.data(); }

// Delphi 2007 IntToHex uses a 32-character zero-fill buffer and unsigned bits.
template<class T> AnsiString int_to_hex(T value, Integer width) {
  using U = std::make_unsigned_t<T>;
  U bits = static_cast<U>(value);
  char buffer[32];
  unsigned length = 0;
  if (width < 0 || width > 32) width = 0;
  do {
    buffer[31 - length++] = "0123456789ABCDEF"[bits & 15];
    bits >>= 4;
  } while (bits);
  while (length < static_cast<unsigned>(width)) buffer[31 - length++] = '0';
  return AnsiString(buffer + 32 - length, length);
}

WideString wide_int_to_str(std::int32_t value);
WideString wide_int64_to_str(std::int64_t value);
template<class Signed> AnsiString int_to_str(Signed value) {
  // Decimal digits, a possible sign, and the extra digit beyond digits10.
  char buffer[std::numeric_limits<Signed>::digits10 + 2];
  auto result = std::to_chars(std::begin(buffer), std::end(buffer), value);
  return AnsiString(buffer, Integer(result.ptr - buffer));
}
[[noreturn]] void invalid_integer(const AnsiString &s);
template<class Signed> Signed parse_integer(const AnsiString &s) {
  // Delphi 2007 _ValLong/_ValInt64: spaces, optional sign, decimal or $/x/0x
  // hexadecimal, terminated at the first null (even within an AnsiString).
  const auto *cursor = reinterpret_cast<const std::uint8_t*>(s.c_str());
  unsigned base = 10;
  bool negative = false;
  while (*cursor == ' ') ++cursor;
  if (*cursor == '-' || *cursor == '+') {
    negative = *cursor == '-'; ++cursor;
  }
  if (*cursor == '$' || *cursor == 'x' || *cursor == 'X') {
    base = 16; ++cursor;
  } else if (*cursor == '0' && (cursor[1] == 'x' || cursor[1] == 'X')) {
    base = 16; cursor += 2;
  }
  if (!*cursor) invalid_integer(s);
  using Unsigned = std::make_unsigned_t<Signed>;
  Unsigned value = 0;
  const Unsigned limit = base == 16 ? std::numeric_limits<Unsigned>::max()
    : Unsigned(std::numeric_limits<Signed>::max()) + Unsigned(negative);
  const Unsigned quotient = limit / base;
  const unsigned remainder = limit % base;
  for (; *cursor; ++cursor) {
    unsigned c = *cursor;
    unsigned digit = c >= '0' && c <= '9' ? c - '0'
      : c >= 'a' && c <= 'f' ? c - 'a' + 10
      : c >= 'A' && c <= 'F' ? c - 'A' + 10 : 255;
    if (digit >= base) invalid_integer(s);
    if (value > quotient || (value == quotient && digit > remainder)) invalid_integer(s);
    value = value * base + digit;
  }
  return std::bit_cast<Signed>(negative ? Unsigned(0) - value : value);
}
inline Integer str_to_int(const AnsiString& s) { return parse_integer<Integer>(s); }
inline std::int64_t str_to_int64(const AnsiString& s) { return parse_integer<std::int64_t>(s); }
inline AnsiString trim_right(const AnsiString &s) {
  Integer n = s.length();
  while (n && s[n] <= 32)
    --n;
  return AnsiString(s.data(), n);
}
struct Exception : Object {
  PAS_CLASS_META(Exception, Object, "Exception", 12)
  AnsiString message;
  Integer help_context = 0;
  void p_create(const AnsiString &text) { message = text; }
};
struct Abort : Exception {
  PAS_CLASS_META(Abort, Exception, "EAbort", 12)
};
struct HeapException : Exception {
  PAS_CLASS_META(HeapException, Exception, "EHeapException", 16)
  bool allow_free = false;
};
struct OutOfMemory : HeapException {
  PAS_CLASS_META(OutOfMemory, HeapException, "EOutOfMemory", 16)
};
struct OSError : Exception {
  PAS_CLASS_META(OSError, Exception, "EOSError", 16)
  Integer ErrorCode = 0;
};
struct ExternalError : Exception {
  PAS_CLASS_META(ExternalError, Exception, "EExternal", 16)
  void *exception_record = nullptr;
};
struct IntError : ExternalError {
  PAS_CLASS_META(IntError, ExternalError, "EIntError", 16)
};
struct DivByZero : IntError {
  PAS_CLASS_META(DivByZero, IntError, "EDivByZero", 16)
};
struct MathError : ExternalError {
  PAS_CLASS_META(MathError, ExternalError, "EMathError", 16)
};
struct InvalidOp : MathError {
  PAS_CLASS_META(InvalidOp, MathError, "EInvalidOp", 16)
};
struct ZeroDivide : MathError {
  PAS_CLASS_META(ZeroDivide, MathError, "EZeroDivide", 16)
};
struct Overflow : MathError {
  PAS_CLASS_META(Overflow, MathError, "EOverflow", 16)
};
struct ConvertError : Exception {
  PAS_CLASS_META(ConvertError, Exception, "EConvertError", 12)
};
struct ListError : Exception {
  PAS_CLASS_META(ListError, Exception, "EListError", 12)
};
struct InvalidCast : Exception {
  PAS_CLASS_META(InvalidCast, Exception, "EInvalidCast", 12)
};
struct InOutError : Exception {
  PAS_CLASS_META(InOutError, Exception, "EInOutError", 16)
  Integer error_code = 0;
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(Exception) == 12);
static_assert(sizeof(ExternalError) == 16);
static_assert(sizeof(InOutError) == 16);
#endif
inline void exception_create(Exception *value, const AnsiString &message) {
  value->p_create(message);
}
template <class T> T *make_exception(const AnsiString &message) {
  return construct<T>([&](T *value) { value->p_create(message); });
}
struct Raised : std::exception {
  std::shared_ptr<Object> object;
  explicit Raised(Object *p);
  const char *what() const noexcept override;
};
[[noreturn]] void raise(Object *exception);
Object *caught_object() noexcept;
[[noreturn]] void raise_os_error(const AnsiString &message, Integer code);
[[noreturn]] void invalid_class_cast();
template <class To, class From> To checked_cast(From *value) {
  if (!value)
    return nullptr;
  if (auto *result = class_cast_if<To>(value))
    return result;
  invalid_class_cast();
}
} // namespace pas
namespace pas {
template <class T> T succ(T v) { return T(v + 1); }
template <class T> T pred(T v) { return T(v - 1); }
inline Cardinal rand_seed = 0;
inline Cardinal next_random(Cardinal *seed) { return *seed = *seed * 0x08088405u + 1; }
inline Integer random(Integer range, Cardinal *seed) {
  return Integer((std::uint64_t(Cardinal(range)) * next_random(seed)) >> 32);
}
inline Integer random(Integer range) { return random(range, &rand_seed); }
inline WideString trim_right(const WideString &s) {
  Integer n = s.length();
  while (n && s[n] <= 32)
    --n;
  return WideString(s.data(), n);
}
} // namespace pas

namespace pas {
template <class F, class Tuple, std::size_t... I>
decltype(auto) call_rtl_impl(F &&fn, Tuple &&values,
                             std::index_sequence<I...>) {
  return std::forward<F>(fn)(
      std::get<sizeof...(I) - 1 - I>(std::forward<Tuple>(values))...);
}
template <class F, class Tuple>
decltype(auto) call_rtl(F &&fn, Tuple &&values) {
  return call_rtl_impl(
      std::forward<F>(fn), std::forward<Tuple>(values),
      std::make_index_sequence<
          std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}
template <class... T> void writeln(const T &...values) {
  write(values...);
  std::putchar('\n');
}
} // namespace pas
namespace pas {
template <class T> void get_mem(T *&p, Integer bytes) {
  if (bytes < 0)
    throw std::bad_alloc();
  p = bytes ? static_cast<T *>(std::malloc(std::size_t(bytes))) : nullptr;
  if (bytes && !p)
    throw std::bad_alloc();
}
inline void *alloc_mem(Integer bytes) {
  void *data;
  get_mem(data, bytes);
  if (bytes)
    std::memset(data, 0, std::size_t(bytes));
  return data;
}
template <class T> Integer free_mem(T *p) {
  std::free(p);
  return 0;
}
template <class T> void realloc_mem(T *&p, Integer bytes) {
  if (bytes < 0)
    throw std::bad_alloc();
  if (!bytes) {
    std::free(p);
    p = nullptr;
    return;
  }
  auto *result = std::realloc(p, std::size_t(bytes));
  if (!result)
    throw std::bad_alloc();
  p = static_cast<T *>(result);
}
// An untyped var pointer can reside in packed storage. Copy the pointer value
// through a local instead of binding an aligned C++ pointer reference to it.
inline void get_mem_at(void *slot, Integer bytes) {
  void *value;
  get_mem(value, bytes);
  store_unaligned(slot, value);
}
inline void realloc_mem_at(void *slot, Integer bytes) {
  auto value = load_unaligned<void *>(slot);
  realloc_mem(value, bytes);
  store_unaligned(slot, value);
}
} // namespace pas
