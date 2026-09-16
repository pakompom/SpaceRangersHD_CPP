#pragma once
// Delphi 2007 Classes.TList. The base list borrows its item pointers.
#include "runtime.hpp"

namespace pas {
class List : public Object {
public:
  PAS_CLASS_META(List, Object, "TList", 16)
  void **p_list = nullptr;
  Integer p_count = 0;
  Integer p_capacity = 0;
  static constexpr Integer max_size = 0x07ffffff;

  [[noreturn]] static void list_error(const char *message, Integer value);
  void check_index(Integer i) const {
    if (i < 0 || i >= p_count)
      list_error("List index out of bounds", i);
  }
  Integer p_getcount() const { return p_count; }
  Integer p_getcapacity() const { return p_capacity; }
  Array<void *, 0, 65535> *p_getlist() const {
    return reinterpret_cast<Array<void *, 0, 65535> *>(p_list);
  }
  virtual void p_notify(void *, std::uint8_t) {
  } // RTL base hook deliberately has no effects.
  virtual void p_grow() {
    auto delta = p_capacity > 64 ? p_capacity / 4 : p_capacity > 8 ? 16 : 4;
    p_setcapacity(p_capacity + delta);
  }
  void p_setcapacity(Integer n) {
    if (n < p_count || n > max_size)
      list_error("List capacity out of bounds", n);
    if (n == p_capacity)
      return;
    if (!n) {
      std::free(p_list);
      p_list = nullptr;
    } else {
      auto *data = std::realloc(p_list, std::size_t(n) * sizeof(void *));
      if (!data)
        throw std::bad_alloc();
      p_list = static_cast<void **>(data);
    }
    p_capacity = n;
  }
  void p_setcount(Integer n) {
    if (n < 0 || n > max_size)
      list_error("List count out of bounds", n);
    if (n > p_capacity)
      p_setcapacity(n);
    if (n > p_count)
      std::fill(p_list + p_count, p_list + n, nullptr);
    else
      for (Integer i = p_count - 1; i >= n; --i)
        p_delete(i);
    p_count = n;
  }
  virtual void p_clear() {
    p_setcount(0);
    p_setcapacity(0);
  }
  void p_destroy() override { p_clear(); }
  Integer p_add(void *item) {
    auto result = p_count;
    if (result == p_capacity)
      p_grow();
    p_list[result] = item;
    ++p_count;
    if (item)
      p_notify(item, 0);
    return result;
  }
  void *p_get(Integer i) const {
    check_index(i);
    return p_list[i];
  }
  void p_put(Integer i, void *item) {
    check_index(i);
    if (item != p_list[i]) {
      auto *previous = p_list[i];
      p_list[i] = item;
      if (previous)
        p_notify(previous, 2);
      if (item)
        p_notify(item, 0);
    }
  }
  void p_delete(Integer i) {
    check_index(i);
    auto *previous = p_list[i];
    --p_count;
    if (i < p_count)
      std::memmove(p_list + i, p_list + i + 1,
                   std::size_t(p_count - i) * sizeof(void *));
    if (previous)
      p_notify(previous, 2);
  }
  Integer p_indexof(void *item) const {
    for (Integer i = 0; i < p_count; ++i)
      if (p_list[i] == item)
        return i;
    return -1;
  }
  Integer p_remove(void *item) {
    auto i = p_indexof(item);
    if (i >= 0)
      p_delete(i);
    return i;
  }
  void p_insert(Integer i, void *item) {
    if (i < 0 || i > p_count)
      list_error("List index out of bounds", i);
    if (p_count == p_capacity)
      p_grow();
    if (i < p_count)
      std::memmove(p_list + i + 1, p_list + i,
                   std::size_t(p_count - i) * sizeof(void *));
    p_list[i] = item;
    ++p_count;
    if (item)
      p_notify(item, 0);
  }
  void p_exchange(Integer a, Integer b) {
    check_index(a);
    check_index(b);
    std::swap(p_list[a], p_list[b]);
  }
  List *p_expand() {
    if (p_count == p_capacity)
      p_grow();
    return this;
  }
  void *p_first() const { return p_get(0); }
  void *p_last() const { return p_get(p_count - 1); }
};
inline void* list_get(List* self, Integer index) { return self->p_get(index); }
template<class T> T* list_at(List* self, Integer index) {
  return static_cast<T*>(list_get(self, index));
}
inline void list_put(List* self, Integer index, void* value) { self->p_put(index,value); }
inline Integer list_count(List* self) { return self->p_getcount(); }
inline Integer list_capacity(List* self) { return self->p_getcapacity(); }
inline auto list_data(List* self) { return self->p_getlist(); }
inline void list_setcount(List* self, Integer count) { self->p_setcount(count); }
inline void list_setcapacity(List* self, Integer count) { self->p_setcapacity(count); }
inline Integer list_add(List* self, void* value) { return self->p_add(value); }
inline void list_insert(List* self, Integer index, void* value) { self->p_insert(index,value); }
inline void list_delete(List* self, Integer index) { self->p_delete(index); }
inline void list_exchange(List* self, Integer left, Integer right) { self->p_exchange(left,right); }
inline Integer list_indexof(List* self, void* value) { return self->p_indexof(value); }
inline Integer list_remove(List* self, void* value) { return self->p_remove(value); }
inline void* list_first(List* self) { return self->p_first(); }
inline void* list_last(List* self) { return self->p_last(); }
inline List* list_expand(List* self) { return self->p_expand(); }
inline void list_clear(List* self) { self->p_clear(); }
inline void list_destroy(List* self) { self->List::p_destroy(); }
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(List) == 16, "Delphi TList instance size");
#endif
} // namespace pas
