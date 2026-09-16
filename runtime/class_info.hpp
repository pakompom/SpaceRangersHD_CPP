#pragma once
// Semantic class identities and factories. This is not a Delphi VMT image.
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace pas {
struct Object;
struct ClassInfo {
  const ClassInfo *parent;
  const char *name;
  std::uint32_t size;
  Object *(*allocate)() = nullptr;
};
// Identity and ancestry are static data. Only the factory is installed lazily
// by register_class; metadata lookups need no local-static initialization guards.
template <class T> inline constinit ClassInfo class_info_storage{
    []() -> const ClassInfo * {
      if constexpr (std::is_void_v<typename T::pas_parent>)
        return nullptr;
      else
        return &class_info_storage<typename T::pas_parent>;
    }(),
    T::pas_name, sizeof(T)};
template <class T> ClassInfo &class_info() { return class_info_storage<T>; }
template <class T> void register_class() {
  static const bool ready = [] {
    if constexpr (!std::is_void_v<typename T::pas_parent>)
      register_class<typename T::pas_parent>();
    if constexpr (!std::is_abstract_v<T>)
      class_info<T>().allocate = []() -> Object * { return new T(); };
    return true;
  }();
  (void)ready;
}
template <class T> struct ClassRef {
  const ClassInfo *info = nullptr;
  ClassRef() = default;
  ClassRef(std::nullptr_t) {}
  explicit ClassRef(const ClassInfo *p) : info(p) {}
  template <class U> ClassRef(ClassRef<U> other) : info(other.info) {}
  ClassRef &operator=(std::nullptr_t) {
    info = nullptr;
    return *this;
  }
  friend bool operator==(ClassRef a, std::nullptr_t) { return !a.info; }
  T *allocate() const {
    if (!info || !info->allocate)
      throw std::runtime_error("class reference has no constructible factory");
    return static_cast<T *>(info->allocate());
  }
};
template <class T, class U> bool operator==(ClassRef<T> a, ClassRef<U> b) {
  return a.info == b.info;
}
template <class T> ClassRef<T> class_ref() {
  register_class<T>();
  return ClassRef<T>(&class_info<T>());
}
template <class T> bool assigned(ClassRef<T> value) {
  return value.info != nullptr;
}
template <class T> ClassRef<Object> class_identity(ClassRef<T> value) {
  return ClassRef<Object>(value);
}
template <class T, class U>
bool inherits_from(ClassRef<T> value, ClassRef<U> parent) {
  for (auto *p = value.info; p; p = p->parent)
    if (p == parent.info)
      return true;
  return false;
}
template <class T> ClassRef<Object> class_parent(ClassRef<T> value) {
  if (!value.info)
    throw std::runtime_error("ClassParent on nil class reference");
  return ClassRef<Object>(value.info->parent);
}
template <class T> std::int32_t instance_size(ClassRef<T> value) {
  if (!value.info)
    throw std::runtime_error("InstanceSize on nil class reference");
  return std::int32_t(value.info->size);
}
} // namespace pas

#define PAS_CLASS_META(TYPE, PARENT, NAME, SIZE)                               \
  using pas_parent = PARENT;                                                   \
  static constexpr const char *pas_name = NAME;                                \
  static constexpr std::uint32_t pas_size = SIZE;                              \
  const pas::ClassInfo *p_class_type() const override {                        \
    return &pas::class_info<TYPE>();                                           \
  }
