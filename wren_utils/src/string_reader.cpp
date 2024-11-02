#include "string_reader.hpp"

namespace wren::utils {

void StringReader::skip_to_text_end(const std::string_view& text) {
  const auto size = text.size();
  position_ = input_.find(text, position_) + size;
}

void StringReader::skip_to_end_line() {
  auto pos = input_.find("\n", position_);
  position_ = pos + 1;
}

auto StringReader::read_to_end_line() -> std::string_view {
  auto pos = input_.find("\n", position_);

  const auto text = substr(position_, pos);
  position_ = pos + 1;
  return text;
}

auto StringReader::read_to_text_start(const std::string_view& text)
    -> std::string_view {
  auto end = input_.find(text, position_);

  const auto content = substr(position_, end);
  position_ = end;
  return content;
}

auto StringReader::read_to_text_end(const std::string_view& text)
    -> std::string_view {
  auto end = input_.find(text, position_) + text.size();

  const auto content = substr(position_, end);
  position_ = end;
  return content;
}

auto StringReader::substr(std::string_view::size_type start,
                          std::string_view::size_type end) -> std::string_view {
  return input_.substr(start, end - start);
}

}  // namespace wren::utils
