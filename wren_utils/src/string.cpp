#include "string.hpp"

namespace wren::utils {

auto split(const std::string& input, const char c) -> std::vector<std::string> {
  std::vector<std::string> result;

  std::string::size_type pos = 0;
  std::string::size_type prev = 0;

  while ((pos = input.find(c, prev)) != std::string::npos) {
    result.push_back(input.substr(prev, pos - prev));
    prev = pos + 1;
  }

  result.push_back(input.substr(prev));

  return result;
}

}  // namespace wren::utils
