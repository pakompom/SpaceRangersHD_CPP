#pragma once
// The required Delphi 2007 TextFile surface. File state is deliberately not
// RAII-managed: the source's explicit CloseFile and exceptional paths govern
// it.
#include "runtime.hpp"
#include <cerrno>
#include <filesystem>
#ifdef _WIN32
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

namespace pas {
struct TextFile {
  std::FILE *stream = nullptr;
  Integer mode = 0; // unassigned, closed, input, output
  char name[260]{};
  unsigned char buffer[128]{};
  Integer position = 0, available = 0;
  std::byte reserved[56]{};
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(TextFile) == 460, "Delphi 2007 TextFile size");
#endif
inline thread_local Integer in_out_res = 0;
inline Integer io_result() { return std::exchange(in_out_res, 0); }
inline void io_finish(bool checked) {
  if (checked && in_out_res) {
    Integer error = io_result();
    raise(construct<InOutError>([&](InOutError *e) {
      e->p_create(AnsiString("I/O error ") + int_to_str(error));
      e->error_code = error;
    }));
  }
}
inline Integer file_error(const char *name = nullptr) {
#ifdef _WIN32
  unsigned long error = 0;
  if (_get_doserrno(&error) == 0 && error)
    return Integer(error);
#endif
  switch (errno) {
  case ENOENT:
    if (name) {
      auto parent = std::filesystem::path(name).parent_path();
      std::error_code error;
      if (!parent.empty() && !std::filesystem::is_directory(parent, error))
        return 3;
    }
    return 2;
  case EACCES:
    return 5;
  case EMFILE:
    return 4;
  default:
    return 5;
  }
}
inline void text_assign(TextFile &f, const AnsiString &name, bool checked) {
  f.stream = nullptr;
  f.mode = 1;
  auto n = name.length();
  if (n >= Integer(sizeof(f.name))) {
    in_out_res = 3;
    io_finish(checked);
    return;
  }
  if (n)
    std::memcpy(f.name, name.data(), n);
  f.name[n] = 0;
  io_finish(checked);
}
inline void text_close(TextFile &f, bool checked) {
  // Delphi still performs Close after an unchecked failure. A closed file
  // replaces the previous error with 103; successful cleanup preserves it.
  if (f.mode == 0 || f.mode == 1)
    in_out_res = 103;
  else {
    if (std::fclose(f.stream))
      in_out_res = file_error();
    f.stream = nullptr;
    f.mode = 1;
  }
  io_finish(checked);
}
inline void text_open(TextFile &f, int mode, bool checked) {
  if (!f.mode)
    in_out_res = 102;
  else {
    if (f.stream)
      text_close(f, false);
    // Append must not create a missing file. Use binary CRT streams and
    // explicit CRLF so the native probe and Win32 output bytes agree.
    const char *open_mode = mode == 2 ? "rb" : mode == 3 ? "wb" : "r+b";
#ifdef _WIN32
    // System.TextOpen uses FILE_SHARE_READ, denying other writers.
    f.stream = _fsopen(f.name, open_mode, _SH_DENYWR);
#else
    f.stream = std::fopen(f.name, open_mode);
#endif
    if (!f.stream)
      in_out_res = file_error(f.name);
    else {
      f.mode = mode == 2 ? 2 : 3;
      f.position = f.available = 0;
      if (mode == 4) {
        if (std::fseek(f.stream, 0, SEEK_END))
          in_out_res = file_error();
        else {
          // The linked Delphi 2007 TextOpen scans the final 128 bytes for
          // byte 0x0E (its assembly `eof` operand), not DOS EOF 0x1A, and
          // truncates there before appending (System.TextOpen).
          auto end = std::ftell(f.stream);
          auto start = std::max(0L, end - 128);
          if (end < 0 || std::fseek(f.stream, start, SEEK_SET))
            in_out_res = file_error();
          else {
            std::array<unsigned char, 128> tail{};
            auto n = std::fread(tail.data(), 1, tail.size(), f.stream);
            auto eof = std::find(tail.begin(), tail.begin() + n, 14);
            if (std::ferror(f.stream))
              in_out_res = file_error();
            if (eof != tail.begin() + n) {
              end = start + (eof - tail.begin());
#ifdef _WIN32
              if (_chsize_s(_fileno(f.stream), end))
                in_out_res = file_error();
#else
              if (::ftruncate(::fileno(f.stream), end))
                in_out_res = file_error();
#endif
            }
            if (std::fseek(f.stream, end, SEEK_SET))
              in_out_res = file_error();
          }
        }
      }
    }
  }
  io_finish(checked);
}
inline bool text_input(TextFile &f) {
  if (f.mode != 2) {
    in_out_res = 104;
    return false;
  }
  return true;
}
inline bool text_output(TextFile &f) {
  if (f.mode != 3) {
    in_out_res = 105;
    return false;
  }
  return true;
}
inline int text_peek(TextFile &f) {
  if (f.position == f.available) {
    f.position = 0;
    f.available = Integer(std::fread(f.buffer, 1, sizeof(f.buffer), f.stream));
    if (std::ferror(f.stream))
      in_out_res = file_error();
  }
  return f.position < f.available ? f.buffer[f.position] : EOF;
}
inline int text_readchar(TextFile &f) {
  int c = text_peek(f);
  if (c != EOF && c != 26)
    ++f.position;
  return c;
}
inline bool text_eof(TextFile &f, bool checked) {
  bool result = true;
  if (text_input(f)) {
    int c = text_peek(f);
    result = c == EOF || c == 26;
  }
  io_finish(checked);
  return result;
}
inline bool text_eoln(TextFile &f, bool checked) {
  bool result = true;
  if (text_input(f)) {
    int c = text_peek(f);
    result = c == EOF || c == 26 || c == 13 || c == 10;
  }
  io_finish(checked);
  return result;
}
inline void text_write_bytes(TextFile &f, const char *data, std::size_t size) {
  if (text_output(f) && size && std::fwrite(data, 1, size, f.stream) != size)
    in_out_res = file_error();
}
inline void text_write_finish(TextFile &f, bool checked) {
  // DCC emits _Flush after Write, or _WriteLn (which flushes) after Writeln.
  // Ordinary files use a no-op FlushFunc; invalid file modes report 103.
  if (f.mode <= 1)
    in_out_res = 103;
  io_finish(checked);
}
inline void text_write(TextFile &f, const AnsiString &s, bool checked) {
  text_write_bytes(f, s.c_str(), std::size_t(s.length()));
  text_write_finish(f, checked);
}
inline void text_newline(TextFile &f, bool checked) {
  text_write_bytes(f, "\r\n", 2);
  text_write_finish(f, checked);
}
inline void text_writeln(TextFile &f, const AnsiString &s, bool checked) {
  text_write_bytes(f, s.c_str(), std::size_t(s.length()));
  text_newline(f, checked);
}
inline void text_flush(TextFile &f, bool checked) {
  if (f.mode <= 1)
    in_out_res = 103;
  else if (f.mode == 3 && std::fflush(f.stream))
    in_out_res = file_error();
  io_finish(checked);
}
inline void text_readln(TextFile &f, AnsiString &s, bool checked) {
  if (text_input(f)) {
    std::string line;
    while (true) {
      int c;
      if (f.position < f.available)
        c = f.buffer[f.position++];
      else {
        c = text_readchar(f);
        if (c == EOF || c == 26)
          break;
      }
      if (c == 10) {
        --f.position;
        break;
      }
      if (c == 26)
        break;
      if (c != 13)
        line.push_back(char(c));
    }
    s = AnsiString(reinterpret_cast<const std::uint8_t *>(line.data()),
                   Integer(line.size()));
    // ReadLine consumes DOS EOF encountered in its existing buffer; ReadChar
    // at a refill leaves it pending. _ReadLn then discards through LF/EOF.
    while (true) {
      int c = text_readchar(f);
      if (c == EOF || c == 26 || c == 10)
        break;
    }
  }
  io_finish(checked);
}
} // namespace pas
