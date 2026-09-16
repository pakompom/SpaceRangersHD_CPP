#pragma once
// Minimal image RTL for the game's screenshot path; no forms or VCL event loop.
#include "runtime.hpp"
#include "windows.hpp"
#include <vector>

namespace pas {
namespace graphics_detail {
[[noreturn]] inline void fail(const char* action) {
  raise(make_exception<Exception>(AnsiString(action)));
}
inline void check(std::int32_t status, const char* action) {
  if (status != 0) fail(action);
}
// Flat GDI+ ABI, from the installed Win32 SDK headers. Keeping these declarations
// here avoids importing Windows macros into mechanically generated source.
struct Guid { std::uint32_t a; std::uint16_t b, c; std::uint8_t d[8]; };
struct StartupInput {
  std::uint32_t version = 1;
  void* callback = nullptr;
  std::int32_t suppress_thread = 0, suppress_codecs = 0;
};
struct CodecInfo {
  Guid clsid, format;
  char16_t *name, *dll, *description, *extension, *mime;
  std::uint32_t flags, version, signature_count, signature_size;
  void *signature, *mask;
};
struct EncoderParameter {
  Guid guid;
  std::uint32_t count, type;
  void* value;
};
struct EncoderParameters { std::uint32_t count; EncoderParameter parameter; };
template<class Signature> using Fn = typename ProcedureCode<Signature, CallbackAbi::stdcall>::type;
struct Api {
  Fn<std::int32_t(std::uintptr_t*, const StartupInput*, void*)> startup = win::load_import<decltype(startup)>("gdiplus.dll", "GdiplusStartup");
  Fn<void(std::uintptr_t)> shutdown = win::load_import<decltype(shutdown)>("gdiplus.dll", "GdiplusShutdown");
  Fn<std::int32_t(const char16_t*, void**)> load = win::load_import<decltype(load)>("gdiplus.dll", "GdipLoadImageFromFile");
  Fn<std::int32_t(void*)> dispose = win::load_import<decltype(dispose)>("gdiplus.dll", "GdipDisposeImage");
  Fn<std::int32_t(void*, std::uint32_t*)> width = win::load_import<decltype(width)>("gdiplus.dll", "GdipGetImageWidth");
  Fn<std::int32_t(void*, std::uint32_t*)> height = win::load_import<decltype(height)>("gdiplus.dll", "GdipGetImageHeight");
  Fn<std::int32_t(std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t, void*, void**)> clone = win::load_import<decltype(clone)>("gdiplus.dll", "GdipCloneBitmapAreaI");
  Fn<std::int32_t(std::uint32_t*, std::uint32_t*)> encoder_size = win::load_import<decltype(encoder_size)>("gdiplus.dll", "GdipGetImageEncodersSize");
  Fn<std::int32_t(std::uint32_t, std::uint32_t, CodecInfo*)> encoders = win::load_import<decltype(encoders)>("gdiplus.dll", "GdipGetImageEncoders");
  Fn<std::int32_t(void*, void*, const Guid*, const EncoderParameters*)> encode = win::load_import<decltype(encode)>("gdiplus.dll", "GdipSaveImageToStream");
  Fn<std::int32_t(void*, std::int32_t, void**)> memory_stream = win::load_import<decltype(memory_stream)>("ole32.dll", "CreateStreamOnHGlobal");
  std::uintptr_t token{};
  Guid jpeg{};
  Api() {
    const StartupInput input;
    check(startup(&token, &input, nullptr), "GDI+ startup failed");
    try {
      std::uint32_t count{}, bytes{};
      check(encoder_size(&count, &bytes), "Cannot enumerate image encoders");
      std::vector<std::uint8_t> buffer(bytes);
      auto* entries = reinterpret_cast<CodecInfo*>(buffer.data());
      check(encoders(count, bytes, entries), "Cannot read image encoders");
      for (std::uint32_t i = 0; i < count; ++i) {
        if (entries[i].mime && std::u16string_view(entries[i].mime) == u"image/jpeg") {
          jpeg = entries[i].clsid;
          return;
        }
      }
      fail("JPEG encoder is unavailable");
    } catch (...) {
      shutdown(token);
      throw;
    }
  }
  ~Api() { shutdown(token); }
};
inline Api& api() { static Api instance; return instance; }
struct Image {
  void* value = nullptr;
  ~Image() { if (value) api().dispose(value); }
  Image() = default;
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
};
inline std::unique_ptr<Image> copy_image(void* source) {
  if (!source) fail("Cannot assign an empty bitmap");
  auto& gdi = api();
  std::uint32_t width{}, height{};
  check(gdi.width(source, &width), "Cannot read bitmap width");
  check(gdi.height(source, &height), "Cannot read bitmap height");
  auto result = std::make_unique<Image>();
  // PixelFormat24bppRGB: deep pixel copy, independent of any input-file lock.
  check(gdi.clone(0, 0, width, height, 0x21808, source, &result->value), "Cannot copy bitmap pixels");
  return result;
}
inline std::unique_ptr<Image> load_bitmap(const AnsiString& filename) {
  Image input;
  const WideString wide(filename);
  check(api().load(wide.pchar(), &input.value), "Cannot load bitmap file");
  return copy_image(input.value);
}
inline std::vector<std::uint8_t> compress(void* image, std::uint32_t quality) {
  if (!image) fail("Cannot compress an empty JPEG image");
  quality = std::min(quality, 100u);
  // EncoderQuality and EncoderParameterValueTypeLong from the Win32 SDK.
  EncoderParameters parameters{1, {{0x1d5be4b5, 0xfa4a, 0x452d, {0x9c,0xdd,0x5d,0xb3,0x51,0x05,0xe7,0xeb}}, 1, 4, &quality}};
  ComPtr<void> stream;
  check(api().memory_stream(nullptr, 1, stream.address()), "Cannot create JPEG stream");
  check(api().encode(image, stream.get(), &api().jpeg, &parameters), "JPEG compression failed");
  std::uint64_t size{};
  check(com_invoke<5, std::int32_t>(stream.get(), std::int64_t(0), std::uint32_t(2), &size), "Cannot size JPEG stream");
  if (size > UINT32_MAX) fail("JPEG stream is too large");
  check(com_invoke<5, std::int32_t>(stream.get(), std::int64_t(0), std::uint32_t(0), static_cast<std::uint64_t*>(nullptr)), "Cannot rewind JPEG stream");
  std::vector<std::uint8_t> result(static_cast<std::size_t>(size));
  std::uint32_t read{};
  check(com_invoke<3, std::int32_t>(stream.get(), result.data(), std::uint32_t(size), &read), "Cannot read JPEG stream");
  if (read != size) fail("Incomplete JPEG stream");
  return result;
}
inline void save(const AnsiString& filename, const std::vector<std::uint8_t>& bytes) {
  FILE* file = std::fopen(reinterpret_cast<const char*>(filename.pchar()), "wb");
  if (!file) fail("Cannot create JPEG file");
  const bool written = std::fwrite(bytes.data(), 1, bytes.size(), file) == bytes.size();
  const bool closed = std::fclose(file) == 0;
  if (!written || !closed) fail("Cannot write JPEG file");
}
} // namespace graphics_detail

struct Persistent : Object { PAS_CLASS_META(Persistent, Object, "TPersistent", 4) };
struct InterfacedPersistent : Persistent {
  PAS_CLASS_META(InterfacedPersistent, Persistent, "TInterfacedPersistent", 12)
  ComPtr<void> OwnerInterface;
};
struct Graphic : InterfacedPersistent {
  PAS_CLASS_META(Graphic, InterfacedPersistent, "TGraphic", 40)
  virtual void load(const AnsiString&) { graphics_detail::fail("Loading this graphic type is unsupported"); }
  virtual void save(const AnsiString&) { graphics_detail::fail("Saving this graphic type is unsupported"); }
};
struct Bitmap : Graphic {
  PAS_CLASS_META(Bitmap, Graphic, "TBitmap", 60)
  std::unique_ptr<graphics_detail::Image> image;
  void load(const AnsiString& file) override { image = graphics_detail::load_bitmap(file); }
};
struct JpegImage : Graphic {
  PAS_CLASS_META(JpegImage, Graphic, "TJPEGImage", 72)
  std::uint8_t Quality = 75;
  std::unique_ptr<graphics_detail::Image> image;
  std::vector<std::uint8_t> encoded;
  void assign(Persistent* source) {
    auto* bitmap = dynamic_cast<Bitmap*>(source);
    if (!bitmap || !bitmap->image) graphics_detail::fail("JPEG assignment requires a bitmap");
    image = graphics_detail::copy_image(bitmap->image->value);
    encoded.clear();
  }
  void compress() { encoded = graphics_detail::compress(image ? image->value : nullptr, Quality); }
  void save(const AnsiString& file) override {
    if (encoded.empty()) compress();
    graphics_detail::save(file, encoded);
  }
};
inline void graphic_load(Graphic* self, const AnsiString& file) { self->load(file); }
inline void graphic_save(Graphic* self, const AnsiString& file) { self->save(file); }
inline void jpeg_assign(JpegImage* self, Persistent* source) { self->assign(source); }
inline void jpeg_compress(JpegImage* self) { self->compress(); }
} // namespace pas
