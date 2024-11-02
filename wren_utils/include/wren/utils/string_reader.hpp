#pragma once

#include <string_view>

namespace wren::utils {

class StringReader {
 public:
  StringReader(const std::string_view& input) : input_(input) {}

  void skip_to_text_end(const std::string_view& text);

  auto read_to_end_line() -> std::string_view;

  auto read_to_text_start(const std::string_view& text) -> std::string_view;

  [[nodiscard]] auto at_end() const { return position_ > input_.size(); }

 private:
  auto substr(std::string_view::size_type start,
              std::string_view::size_type end) -> std::string_view;

  std::string_view input_;
  std::string_view::size_type position_{};
};

}  // namespace wren::utils
