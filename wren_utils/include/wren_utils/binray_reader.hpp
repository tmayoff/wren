#pragma once

#include <cstdint>
#include <span>

namespace wren::utils {

class BinaryReader {
 public:
  BinaryReader(std::span<uint8_t>& data) : data_(data), pos_(data_.begin()) {}

  void skip(size_t byte_count) { pos_ += static_cast<long>(byte_count); }

  template <typename T>
  auto read() {
    std::span sub(pos_, pos_ + sizeof(T));
    pos_ += sizeof(T);

    return *reinterpret_cast<T*>(sub.data());
  }

  template <typename T, std::size_t N>
  auto read_list() {
    std::array<T, N> list;

    for (auto i = 0; i < N; ++i) {
      list.at(i) = read<T>();
    }

    return list;
  }

 private:
  std::span<uint8_t> data_;
  std::span<uint8_t>::iterator pos_;
};

}  // namespace wren::utils
