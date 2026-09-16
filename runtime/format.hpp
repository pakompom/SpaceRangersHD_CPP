#pragma once
#include "runtime.hpp"
#include <variant>
#include <cctype>

namespace pas {
// The literal Format modes present in this game: integers, text and real values.
// Other modes are rejected by the binder at their source location.
using FormatValue = std::variant<std::int64_t, long double, AnsiString>;
template<class T> FormatValue format_value(T value) {
  if constexpr (is_real_v<T>) return static_cast<long double>(value);
  else return static_cast<std::int64_t>(value);
}
inline FormatValue format_value(const AnsiString& value) { return value; }
template<class... Args> AnsiString format(const AnsiString& pattern, const Args&... args) {
  const std::array<FormatValue, sizeof...(Args)> values{format_value(args)...};
  std::string result;
  std::size_t argument = 0;
  const auto text = std::string_view(pattern.c_str(), pattern.length());
  for (std::size_t i = 0; i < text.size(); ++i) {
    if (text[i] != '%') { result.push_back(text[i]); continue; }
    ++i;
    if (text[i] == '%') { result.push_back('%'); continue; }
    std::string spec = "%";
    while (i < text.size() && (text[i] == '-' || text[i] == '.' || (text[i] >= '0' && text[i] <= '9'))) spec.push_back(text[i++]);
    const char mode = char(std::tolower(static_cast<unsigned char>(text[i])));
    const auto& value = values.at(argument++);
    auto append = [&](auto data) {
      int count = std::snprintf(nullptr, 0, spec.c_str(), data);
      if (count < 0) throw std::runtime_error("Format conversion failed");
      const auto start = result.size();
      result.resize(start + std::size_t(count) + 1);
      std::snprintf(result.data() + start, std::size_t(count) + 1, spec.c_str(), data);
      result.resize(start + std::size_t(count));
    };
    if (mode == 's') {
      const auto& string = std::get<AnsiString>(value);
      auto length = std::size_t(string.length());
      const auto dot = spec.find('.');
      if (dot != std::string::npos) length = std::min<std::size_t>(length, std::stoul(spec.substr(dot + 1)));
      const bool left = spec.size() > 1 && spec[1] == '-';
      const auto start = left ? 2U : 1U;
      const auto width_text = spec.substr(start, dot == std::string::npos ? dot : dot - start);
      const auto width = width_text.empty() ? 0U : std::stoul(width_text);
      const auto padding = width > length ? width - length : 0;
      if (!left) result.append(padding, ' ');
      result.append(string.c_str(), length);
      if (left) result.append(padding, ' ');
    } else if (mode == 'd') {
      spec += "lld";
      append(static_cast<long long>(std::get<std::int64_t>(value)));
    } else {
      spec += 'L'; spec += mode;
      append(std::get<long double>(value));
    }
  }
  return AnsiString(result.data(), std::int32_t(result.size()));
}
}
